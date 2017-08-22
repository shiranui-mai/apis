#include "zk.h"

#include <unistd.h>

static void fn_zk_children_watcher(zhandle_t *zh, int type, int state, const char *path,void *watcherCtx);

static void fn_get_zknode_children_completion(int rc, const struct String_vector* strings, const void* data)
{
	zk* pzk = (zk*)data;
    ngx_log_debug(NGX_LOG_DEBUG_HTTP, pzk->log, 0, "[tzj] [fn_get_zknode_children_completion] rc: %d\n", rc);
	if (ZOK == rc) {
		int i = 0;
		for (; i < strings->count; ++i) {
    		ngx_log_debug(NGX_LOG_DEBUG_HTTP, pzk->log, 0, "[tzj] [fn_get_zknode_children_completion] data:  %s\n", strings->data[i]);
			char buf[128] = "/live/service/";
			strcat(buf, strings->data[i]);
			zoo_awget_children(pzk->zk_handle, buf, fn_zk_children_watcher, pzk, fn_get_zknode_children_completion, pzk);
		}
			// printf("%s\n", strings.data[i]);
	}
}

static void fn_zk_children_watcher(zhandle_t *zh, int type, int state, const char *path,void *watcherCtx)
{
	zk* pzk = (zk*)watcherCtx;
    ngx_log_debug(NGX_LOG_DEBUG_HTTP, pzk->log, 0, "[tzj] [fn_zk_children_watcher] path:  %s\n", path);
	zoo_awget_children(pzk->zk_handle, path, fn_zk_children_watcher, pzk, fn_get_zknode_children_completion, pzk);
}

int get_children(zk* pzk, const char* path, struct String_vector* strings)
{
	return zoo_awget_children(pzk->zk_handle, path,fn_zk_children_watcher, pzk, fn_get_zknode_children_completion, pzk);
}

int read_all_node(zk* pzk)
{
    ngx_log_debug(NGX_LOG_DEBUG_HTTP, pzk->log, 0, "[tzj] [read_all_node]\n");
	struct String_vector nodes;
	return get_children(pzk, "/live/service", &nodes);
}

int init_zk(zk* pzk)
{
	zoo_set_debug_level(ZOO_LOG_LEVEL_DEBUG);

	do {
		if ((pzk->zk_handle = zookeeper_init((const char*)pzk->host.data, NULL, 30000, 0, pzk, 0)) == NULL)
			sleep(1);
	} while (pzk->zk_handle == NULL);

	if (pzk->svr == NULL) {
		pzk->svr = (svr_t*)ngx_pcalloc(pzk->pool, sizeof(svr_t));
	}

	return read_all_node(pzk);
}
void close_zk(zk* pzk)
{
	zookeeper_close(pzk->zk_handle);
}
