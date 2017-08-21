#include "zk.h"

void zk_watcher(zhandle_t *zh, int type, int state, const char *path,void *watcherCtx)
{
}

void zk_children_watcher(zhandle_t *zh, int type, int state, const char *path,void *watcherCtx)
{
}

void get_children(zk* pzk, const char* path, struct String_vector& strings)
{
	struct Stat stat;

	int flag = zoo_wget_children2(pzk->zk_handle, path, zk_children_watcher, NULL, &strings, &stat)
	if (ZOK == flag) {
		for (int i = 0; i < strings.count; ++i)
			printf("%s\n", strings.data[i]);
	}
}

int read_all_node(zk* pzk)
{
	struct String_vector nodes;
	int rc = get_children(pzk, "/dis/", nodes);
}

int init_zk(zk* pzk)
{
	zoo_set_debug_level(ZOO_LOG_LEVEL_DEBUG);

	pzk->zk_handle = zookeeper_init("ZOOKEEPER_HOST", zk_watcher, 30000, 0, pzk, 0);
	if (NULL == pzk->zk_handle)
	{
		return -1;
	}

	if (pzk->svr == NULL) {
		pzk->svr = (svr_t*)ngx_pcalloc(pzk->pool, sizeof(svr_t));
	}

	return read_all_node(pzk);
}
