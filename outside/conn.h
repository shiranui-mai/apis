#ifndef __APIS_CONN__
#define __APIS_CONN__

#include <ngx_config.h>
#include <ngx_core.h>
#include <ngx_event.h>

typedef struct _conn_t {
	char host[64];
	int  port;
}conn_t;

int connect(conn_t conn);

#endif
