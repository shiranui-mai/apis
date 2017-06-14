#include "outside.h"

// #define HEALTH_CHECK_SUCCESS "Health check success.   "
#define HEALTH_CHECK_SUCCESS "123"

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

static ngx_int_t ngx_http_outside_handler(ngx_http_request_t* r) 
{
	ngx_http_outside_conf_t* cf = ngx_http_get_module_loc_conf(r, ngx_http_outside_module);
    if (cf == NULL) {
        ngx_log_error(NGX_LOG_DEBUG_HTTP, r->connection->log, 0, "[tzj] [ngx_http_outside_handler] get_module error. \n");
        return NGX_ERROR;
    }

    r->write_event_handler = ngx_http_request_empty_handler;

    //  Invoke first subrequest
	return (invoke_subrequest(r, (outside_service_t*)cf) != NGX_OK)?NGX_ERROR:NGX_DONE;
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

	ngx_buf_t* b = ngx_calloc_buf(r->pool);
	b->pos = (u_char*)HEALTH_CHECK_SUCCESS;
	b->last = b->pos + sizeof(HEALTH_CHECK_SUCCESS) - 1;
	b->memory = 1;
    b->last_buf = 1;

	ngx_chain_t* out = ngx_alloc_chain_link(r->pool);
	out->buf = b;
	out->next = NULL;
    // ngx_chain_t* chain_out = ngx_http_get_module_ctx(r, ngx_http_outside_module);

	r->headers_out.content_type.len = sizeof("text/plain") - 1;
	r->headers_out.content_type.data = (u_char*)"text/plain";

	r->headers_out.status = NGX_HTTP_OK;
	r->connection->buffered |= NGX_HTTP_WRITE_BUFFERED;
	
	ngx_chain_t* out_l = NULL; 
    ngx_chain_add_copy(r->pool, &out_l, r->out);
    // ngx_chain_add_copy(r->pool, &out_l, out);
    ngx_chain_t* _p_out = out_l;
    while (_p_out != NULL) {
        if (_p_out->next == NULL) break;
        else _p_out = _p_out->next;
    }
    _p_out->buf->last_buf = 1;

    r->out = NULL;
	ngx_int_t rc = ngx_http_send_header(r);
	if (rc == NGX_ERROR || rc > NGX_OK || r->header_only) {
		// return rc;
	}
    rc = ngx_http_output_filter(r, out_l);
	return ngx_http_finalize_request(r, rc);
}

// Child post handler
static ngx_int_t outside_subrequest_post_handler(ngx_http_request_t* r, void* data, ngx_int_t rc)
{
    outside_service_t* s = (outside_service_t*)data;
	ngx_http_request_t* pr = r->parent;

    pr->headers_out.status = r->headers_out.status;

    ngx_chain_add_copy(pr->pool, &pr->out, r->upstream->out_bufs);

    if (s->fin == 1) {
        outside_post_handler(pr);
    } else {
        // Invoke subrequest
        return invoke_subrequest(pr, ++s);
    }

    return NGX_OK;
}
