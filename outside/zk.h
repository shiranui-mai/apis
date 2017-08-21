#ifndef __OUTSIDE_ZK_HH__
#define __OUTSIDE_ZK_HH__

#include <ngx_config.h>
#include <ngx_core.h>
#include <ngx_event.h>

#include <zookeeper/zookeeper.h>
#include <zookeeper/zookeeper_log.h>

typedef struct _host_data {
	ngx_str_t host;
	ngx_int_t port;
	ngx_connection_t* c;
}host_data;

typedef struct _host_t {
	ngx_str_t svr_name;
	ngx_list_t host_list; /* host_data */
}host_t;

typedef struct _svr_t {
	ngx_str_t base_path;
	ngx_list_t svr_list; /* host_t */
}svr_t;

typedef struct _zk {
	zkhandle_t* zk_handle;
	svr_t* svr;

	ngx_log_t*  log;
	ngx_pool_t* pool;
}zk;

int init_zk(zk* pzk);

#endif
