#include "outside.h"

typedef struct _params {
	ngx_str_t  key;
	ngx_str_t  val;
}params;

static ngx_int_t invoke_subrequest(ngx_http_request_t* r, outside_service_t* s)
{
	ngx_http_outside_conf_t* cf = ngx_http_get_module_loc_conf(r, ngx_http_outside_module);
    if (cf == NULL) {
        ngx_log_error(NGX_LOG_DEBUG_HTTP, r->connection->log, 0, "[tzj] [invoke_subrequest] get_module error. \n");
        return NGX_ERROR;
    }

    // Set subrequest
	ngx_http_post_subrequest_t* psr = ngx_pcalloc(r->pool, sizeof(ngx_http_post_subrequest_t));
	psr->handler = outside_subrequest_post_handler;
    psr->data = s;

    // Invoke subrequest
    ngx_http_request_t* tmp;
    return ngx_http_subrequest(r, &s->uri, NULL, &tmp, psr, NGX_HTTP_SUBREQUEST_IN_MEMORY);
}

static ngx_int_t verify_args(ngx_http_request_t* r)
{
	ngx_list_t* tmp_list = ngx_list_create(r->pool, 24, sizeof(params));	
	if (tmp_list == NULL) {
		return NGX_ERROR;
	}

	ngx_str_t args = ngx_null_string;
	if (r->method == NGX_HTTP_GET) {
		ngx_memcpy(&args, &(r->args), sizeof(args));
	} else if (r->method == NGX_HTTP_POST) {
		args.data = r->header_in->pos;
		args.len  = r->header_in->last - r->header_in->pos; 
	}
    ngx_log_debug(NGX_LOG_DEBUG_HTTP, r->connection->log, 0, "[tzj] [verify_args] method: %d. args:  %V\n", r->method, &args);

	size_t tmp_len = args.len;
	u_char* params_begin = args.data;
	while (params_begin != NULL) {
		/* each param */
		u_char* k = (u_char*)ngx_strchr(params_begin, '=');	
		if (k == NULL) 	return NGX_ERROR;	

		params* p = (params*)ngx_list_push(tmp_list);
		if (p == NULL)  return NGX_ERROR;
		
		p->key.data = params_begin;
		p->key.len  = k - params_begin;
		tmp_len -= p->key.len + 1;

		/* Get next param */
		params_begin = (u_char*)ngx_strchr(params_begin, '&');

		p->val.data = k+1;
		p->val.len  = params_begin?(size_t)(params_begin - k):(tmp_len - 1);
		tmp_len -= p->val.len + 1;
	}

	return NGX_OK;
}

static ngx_int_t ngx_http_outside_handler(ngx_http_request_t* r) 
{
	ngx_http_outside_conf_t* cf = ngx_http_get_module_loc_conf(r, ngx_http_outside_module);
    if (cf == NULL) {
        ngx_log_error(NGX_LOG_DEBUG_HTTP, r->connection->log, 0, "[tzj] [ngx_http_outside_handler] get_module error. \n");
        return NGX_ERROR;
    }

	if (verify_args(r) == NGX_OK) {
    	//  Invoke first subrequest
    	return invoke_subrequest(r, (outside_service_t*)cf);
	} else {
		// output
		ngx_chain_t out;
		ngx_buf_t* b = ngx_pcalloc(r->pool, sizeof(ngx_buf_t));

		out.buf = b;
    	out.next = NULL;

    	b->pos = (u_char*)"error paramet.";
    	b->last = b->pos + sizeof("error paramet.") - 1;
    	b->memory = 1;
    	b->last_buf = 1;

		r->headers_out.content_type.len  = sizeof("text/plain") - 1;
		r->headers_out.content_type.data =  (u_char*)"text/plain";
		r->headers_out.status = NGX_HTTP_OK;
		r->headers_out.content_length_n = sizeof("error paramet.") - 1;
		ngx_http_send_header(r);
		return ngx_http_output_filter(r, &out);
	}

	return NGX_ERROR;
}

static void* ngx_http_outside_create_loc_conf(ngx_conf_t* cf)
{
	ngx_http_outside_conf_t* conf = ngx_pcalloc(cf->pool, sizeof(ngx_http_outside_conf_t));
	if (NULL == conf) {
		return NGX_CONF_ERROR;
	}
    ngx_str_null(&conf->user_filter.uri);
    conf->user_filter.fin = 0;
    ngx_str_null(&conf->finish.uri);
    conf->finish.fin = 1;
	return conf;
}
static char* ngx_http_outside_merge_loc_conf(ngx_conf_t* cf, void* parent, void* child)
{
	ngx_http_outside_conf_t* prev = parent;
	ngx_http_outside_conf_t* conf = child;
	ngx_conf_merge_str_value(conf->user_filter.uri, prev->user_filter.uri, "test");
	ngx_conf_merge_str_value(conf->finish.uri, prev->finish.uri, "test");
	return NGX_CONF_OK;
}

static char* ngx_http_outside(ngx_conf_t* cf, ngx_command_t* cmd, void* conf)
{
	ngx_http_core_loc_conf_t* clct = ngx_http_conf_get_module_loc_conf(cf, ngx_http_core_module); 
	clct->handler = ngx_http_outside_handler;
	ngx_conf_set_str_slot(cf, cmd, conf);
	return NGX_CONF_OK;
}

// post handler
static void outside_post_handler(ngx_http_request_t* r)
{
	if (r->headers_out.status != NGX_HTTP_OK)
	{
		ngx_http_finalize_request(r, r->headers_out.status);
		return ;
	}

	r->headers_out.content_type.len = sizeof("text/plain") - 1;
	r->headers_out.content_type.data = (u_char*)"text/plain";

	r->headers_out.status = NGX_HTTP_OK;
	r->connection->buffered |= NGX_HTTP_WRITE_BUFFERED;
	
	ngx_chain_t* out_l = NULL; 
    ngx_chain_add_copy(r->pool, &out_l, r->user_data);

    // Reset last_buf flag is 1
    if (out_l != NULL) {
        get_last_chain(out_l)->buf->last_buf = 1;
    }

	ngx_http_send_header(r);
    ngx_int_t ret = ngx_http_output_filter(r, out_l);
	ngx_http_finalize_request(r, ret);
}

// Child post handler
static ngx_int_t outside_subrequest_post_handler(ngx_http_request_t* r, void* data, ngx_int_t rc)
{
    outside_service_t* s = (outside_service_t*)data;
	ngx_http_request_t* pr = r->parent;

    pr->headers_out.status = r->headers_out.status;

    ngx_chain_add_copy(pr->pool, (ngx_chain_t**)&pr->user_data, r->upstream->out_bufs);

    if (s->fin == 1) {
        outside_post_handler(pr);
    } else {
        // Invoke subrequest
        return invoke_subrequest(pr, ++s);
    }

    return NGX_OK;
}

static ngx_chain_t* get_last_chain(ngx_chain_t* ch)
{
    while (ch != NULL) {
        if (ch->next == NULL) break;
        else ch = ch->next;
    }
    return ch;
}
