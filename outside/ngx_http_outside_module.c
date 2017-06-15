#include "outside.h"

// #define HEALTH_CHECK_SUCCESS "Health check success.   "
#define HEALTH_CHECK_SUCCESS "123"

// extern void ngx_http_close_request(ngx_http_request_t *r, ngx_int_t error);

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

    // r->count = 1;
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

    // r->write_event_handler = outside_post_handler;
    // r->write_event_handler = ngx_http_request_empty_handler;
    // r->write_event_handler = ngx_http_request_finalizer;

    //  Invoke first subrequest
    return invoke_subrequest(r, (outside_service_t*)cf);
	// return (invoke_subrequest(r, (outside_service_t*)cf) != NGX_OK)?NGX_ERROR:NGX_DONE;
	// return (invoke_subrequest(r, (outside_service_t*)&(cf->finish)) != NGX_OK)?NGX_ERROR:NGX_DONE;
}

static ngx_int_t ngx_http_outside_test_handler(ngx_http_request_t* r) 
{
	r->headers_out.content_type.len = sizeof("text/plain") - 1;
	r->headers_out.content_type.data = (u_char*)"text/plain";

	r->headers_out.status = NGX_HTTP_OK;
	r->connection->buffered |= NGX_HTTP_WRITE_BUFFERED;

    ngx_buf_t* b = ngx_pcalloc(r->pool, sizeof(ngx_buf_t));
    b->pos = (u_char*)"out_side_test";
    b->last = b->pos + sizeof("out_side_test") - 1;
    b->memory = 1;
    b->last_buf = 1;
	
	ngx_chain_t out; 
    out.buf = b;
    out.next = NULL;

	ngx_http_send_header(r);
    return ngx_http_output_filter(r, &out);
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
static char* ngx_http_outside_test(ngx_conf_t* cf, ngx_command_t* cmd, void* conf)
{
	ngx_http_core_loc_conf_t* clct = ngx_http_conf_get_module_loc_conf(cf, ngx_http_core_module); 
	clct->handler = ngx_http_outside_test_handler;
	// ngx_conf_set_str_slot(cf, cmd, conf);
	return NGX_CONF_OK;
}

// post handler
static void outside_post_handler(ngx_http_request_t* r)
{
    // r->write_event_handler = ngx_http_request_empty_handler;
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
        ngx_log_error(NGX_LOG_DEBUG_HTTP, r->connection->log, 0, "[tzj] [outside_post_handler] out_l != NULL. \n");
        get_last_chain(out_l)->buf->last_buf = 1;
    } else {
        ngx_log_error(NGX_LOG_DEBUG_HTTP, r->connection->log, 0, "[tzj] [outside_post_handler] out_l == NULL. \n");
    }

    // r->out = NULL;
	ngx_http_send_header(r);
    ngx_int_t ret = ngx_http_output_filter(r, out_l);
    // ngx_http_output_filter(r, out_l);
	// ngx_http_finalize_request(r, ngx_http_output_filter(r, out_l));
	// ngx_http_finalize_request(r, ngx_http_output_filter(r, NULL));
	ngx_http_finalize_request(r, ret);
    // ngx_http_close_request(r, NGX_HTTP_OK);
}

// Child post handler
static ngx_int_t outside_subrequest_post_handler(ngx_http_request_t* r, void* data, ngx_int_t rc)
{
    outside_service_t* s = (outside_service_t*)data;
	ngx_http_request_t* pr = r->parent;

    pr->headers_out.status = r->headers_out.status;
    // pr->write_event_handler = s->fin == 1?outside_post_handler:ngx_http_request_empty_handler;

    // pr->out = NULL;
    ngx_chain_add_copy(pr->pool, (ngx_chain_t**)&pr->user_data, r->upstream->out_bufs);

        // outside_post_handler(pr);
    if (s->fin == 1) {
        outside_post_handler(pr);
    } else {
        // Invoke subrequest
        return invoke_subrequest(pr, ++s);
    }

    // pr->write_event_handler = outside_post_handler;
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