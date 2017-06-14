#ifndef __HTTP_OUTSIDE_H__
#define __HTTP_OUTSIDE_H__

#include <ngx_config.h>
#include <ngx_core.h>
#include <ngx_http.h>

/* 3rd services */
typedef struct {
    ngx_str_t uri;
    // Specify finish
    unsigned fin:1;
} outside_service_t;

typedef struct {
    /* All 3rd services */
    outside_service_t user_filter; 
    outside_service_t finish; 
    /* End All 3rd services */
} ngx_http_outside_conf_t;

// config
static void* ngx_http_outside_create_loc_conf(ngx_conf_t* cf);
static char* ngx_http_outside_merge_loc_conf(ngx_conf_t* cf, void* parent, void* child);

static char* ngx_http_outside(ngx_conf_t* cf, ngx_command_t* cmd, void* conf);

// post handler
static void outside_post_handler(ngx_http_request_t* r);
// Child post handler
static ngx_int_t outside_subrequest_post_handler(ngx_http_request_t* r, void* data, ngx_int_t rc);

static ngx_command_t ngx_http_outside_commands[] = {
	{
		ngx_string("user_filter"),
		NGX_HTTP_LOC_CONF | NGX_CONF_TAKE1,
		ngx_conf_set_str_slot,
		NGX_HTTP_LOC_CONF_OFFSET,
		offsetof(ngx_http_outside_conf_t, user_filter.uri),
		NULL
	},
	{
		ngx_string("outside"),
		NGX_HTTP_LOC_CONF | NGX_CONF_TAKE1,
		ngx_http_outside,
		NGX_HTTP_LOC_CONF_OFFSET,
		offsetof(ngx_http_outside_conf_t, finish.uri),
		NULL
	},
	ngx_null_command	
};

static ngx_http_module_t ngx_http_outside_module_ctx = {
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	ngx_http_outside_create_loc_conf,
	ngx_http_outside_merge_loc_conf
    // NULL, NULL
};

ngx_module_t ngx_http_outside_module = {
	NGX_MODULE_V1,
	&ngx_http_outside_module_ctx,
	ngx_http_outside_commands,
	NGX_HTTP_MODULE,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NGX_MODULE_V1_PADDING
};

#endif
