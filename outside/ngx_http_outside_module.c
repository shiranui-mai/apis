#include "outside.h"

#include "conn.h"

typedef struct _params {
	ngx_str_t  key;
	ngx_str_t  val;
}params;

//static ngx_http_outside_conf_t* g_outside_conf = NULL; 
static ngx_str_t* g_zk_host = NULL;

static ngx_int_t invoke_subrequest(ngx_http_request_t* r, outside_service_t* s)
{
	// check param
	if (NULL == s) return NGX_ERROR;
	if (s->uri.len == 0) return invoke_subrequest(r, ++s);
	// if (s->fin == 1) return outside_post_handler(r);

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

static void ngx_outside_tcp_read(ngx_event_t *rev)
{
    u_char                     *p;
    size_t                      size;
    ssize_t                     n;
    u_short                     qlen;
    ngx_buf_t                  *b;
    ngx_connection_t           *c;

    c = rev->data;
	b = ngx_pcalloc(c->pool, sizeof(ngx_buf_t));

    while (rev->ready) {
        n = ngx_recv(c, b->last, b->end - b->last);

        if (n == NGX_AGAIN) {
            break;
        }

        if (n == NGX_ERROR || n == 0) {
            goto failed;
        }

        b->last += n;

        for ( ;; ) {
            p = b->pos;
            size = b->last - p;

            if (size < 2) {
                break;
            }

            qlen = (u_short) *p++ << 8;
            qlen += *p++;

            if (size < (size_t) (2 + qlen)) {
                break;
            }

            // ngx_resolver_process_response(r, p, qlen, 1);

            b->pos += 2 + qlen;
        }

        if (b->pos != b->start) {
            b->last = ngx_movemem(b->start, b->pos, b->last - b->pos);
            b->pos = b->start;
        }
    }

    if (ngx_handle_read_event(rev, 0) != NGX_OK) {
        goto failed;
    }

    return;

failed:

    ngx_close_connection(c);
}
static void ngx_outside_tcp_write(ngx_event_t *wev)
{
    off_t                       sent;
    ssize_t                     n;
    ngx_buf_t                  *b;
    ngx_connection_t           *c;

    c = wev->data;
	b = ngx_pcalloc(c->pool, sizeof(ngx_buf_t));

    if (wev->timedout) {
        goto failed;
    }

    sent = c->sent;

    while (wev->ready && b->pos < b->last) {
        n = ngx_send(c, b->pos, b->last - b->pos);

        if (n == NGX_AGAIN) {
            break;
        }

        if (n == NGX_ERROR) {
            goto failed;
        }

        b->pos += n;
    }

    if (b->pos != b->start) {
        b->last = ngx_movemem(b->start, b->pos, b->last - b->pos);
        b->pos = b->start;
    }

    if (c->sent != sent) {
        ngx_add_timer(wev, (ngx_msec_t) (3 * 1000));
    }

    if (ngx_handle_write_event(wev, 0) != NGX_OK) {
        goto failed;
    }

    return;

failed:

    ngx_close_connection(c);
}

static ngx_int_t ngx_http_outside_handler(ngx_http_request_t* r) 
{
	ngx_http_outside_conf_t* cf = ngx_http_get_module_loc_conf(r, ngx_http_outside_module);
    if (cf == NULL) {
        ngx_log_error(NGX_LOG_DEBUG_HTTP, r->connection->log, 0, "[tzj] [ngx_http_outside_handler] get_module error. \n");
        return NGX_ERROR;
    }

	ngx_connection_t* pc = NULL;
	conn_t c;
	ngx_memzero(&c, sizeof(c));
	strncpy(c.host, "172.16.71.180", 64);
	c.port = 123456;
	outside_connect(c, r->connection->log, &pc);
	pc->pool = r->pool;
	// ngx_close_connection(pc);
	pc->read->handler  = ngx_outside_tcp_read;
	pc->write->handler = ngx_outside_tcp_write;

	/*zk* _zk = (zk*)ngx_pcalloc(ngx_cycle->pool, sizeof(zk));
	_zk->log  = ngx_cycle->log;
	_zk->pool = ngx_cycle->pool;
	_zk->host = cf->zk_host;
	init_zk(_zk);

	sleep(2); */

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
	ngx_http_outside_conf_t* g_outside_conf = ngx_pcalloc(cf->pool, sizeof(ngx_http_outside_conf_t));
	if (NULL == g_outside_conf) {
		return NGX_CONF_ERROR;
	}

    ngx_str_null(&g_outside_conf->user_filter.uri);
    g_outside_conf->user_filter.fin = 0;
    ngx_str_null(&g_outside_conf->finish.uri);
    g_outside_conf->finish.fin = 1;

	return g_outside_conf;
}

static char* ngx_http_zk_host_post(ngx_conf_t* cf, void* data, void* conf)
{
	// printf("[tzj] [ngx_http_zk_host_post] \n");
	g_zk_host = conf;
    return NGX_CONF_OK;
}
static char* ngx_http_outside(ngx_conf_t* cf, ngx_command_t* cmd, void* conf)
{
	ngx_http_core_loc_conf_t* clct = ngx_http_conf_get_module_loc_conf(cf, ngx_http_core_module); 
	clct->handler = ngx_http_outside_handler;
	return ngx_conf_set_str_slot(cf, cmd, conf);
}

// post handler
static ngx_int_t outside_post_handler(ngx_http_request_t* r)
{
	if (r->headers_out.status != NGX_HTTP_OK)
	{
		ngx_http_finalize_request(r, r->headers_out.status);
		return NGX_ERROR;
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
	return NGX_OK;
}

// Child post handler
static ngx_int_t outside_subrequest_post_handler(ngx_http_request_t* r, void* data, ngx_int_t rc)
{
    outside_service_t* s = (outside_service_t*)data;
	ngx_http_request_t* pr = r->parent;

    pr->headers_out.status = r->headers_out.status;

    ngx_chain_add_copy(pr->pool, (ngx_chain_t**)&pr->user_data, r->upstream->out_bufs);

    if (s->fin == 1) {
        return outside_post_handler(pr);
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

static ngx_int_t init_outside(ngx_cycle_t* cycle)
{
    ngx_uint_t  i;
    ngx_log_debug(NGX_LOG_DEBUG_HTTP, cycle->log, 0, "[tzj] [init_outside] start!\n");
    for (i = 0; cycle->modules[i]; i++) {
		if ((cycle->modules[i]->name) && (ngx_strncmp(cycle->modules[i]->name, "ngx_http_outside_module", sizeof("ngx_http_outside_module")) == 0)) {
			
				// ngx_log_debug(NGX_LOG_DEBUG_HTTP, cycle->log, 0, "[tzj] [init_outside] module.name: %s. zk_host: %V\n", cycle->modules[i]->name, g_zk_host);

				zk* _zk = (zk*)ngx_pcalloc(cycle->pool, sizeof(zk));
				_zk->log  = cycle->log;
				_zk->pool = cycle->pool;
				_zk->host = *g_zk_host;
				init_zk(_zk);

				sleep(2);
				cycle->modules[i]->spare_hook1 = (uintptr_t)_zk;
				// close_zk(_zk);
				break;
		}
	}
	return NGX_OK;
}
static void exit_outside(ngx_cycle_t* cycle)
{
    ngx_uint_t  i;
    ngx_log_debug(NGX_LOG_DEBUG_HTTP, cycle->log, 0, "[tzj] [exit_outside] start!\n");
    for (i = 0; cycle->modules[i]; i++) {
		if (ngx_strncmp(cycle->modules[i]->name, "ngx_http_outside_module", 23) == 0) {
			
				ngx_log_debug(NGX_LOG_DEBUG_HTTP, cycle->log, 0, "[tzj] [exit_outside] module.name: %s \n", cycle->modules[i]->name);
				close_zk((zk*)cycle->modules[i]->spare_hook1);
				break;
		}
	}
}
