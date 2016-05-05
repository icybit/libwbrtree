#ifndef __WBD_RTREE_API_H__
#define __WBD_RTREE_API_H__

#if !defined (__WBD_RTREE_H_INSIDE__) && !defined (WBD_RTREE_COMPILATION)
	#error "Only <wbdrtree.h> can be included directly."
#endif

#ifdef __cplusplus
extern "C" {
#endif

#include <stddef.h>
#include <stdint.h>

	typedef struct Context rt_ctx_t;
	typedef struct Entry rt_entry_t;
	typedef struct Node rt_node_t;
	typedef struct Rectangle rt_rect_t;
	typedef struct RTree rt_rtree_t;

	typedef struct hashset_st rt_hset_t;

	/* BEGIN RTREE_CONTEXT_API */
	rt_ctx_t * rtree_context_create(uint8_t m, uint8_t M, uint8_t dim, size_t(*serialize_fn)(rt_entry_t *, uint8_t **), float alloc_factor, rt_rect_t *space);
	void rtree_context_destroy(rt_ctx_t *ctx);
	/* END RTREE_CONTEXT_API */

	/* BEGIN RTREE_ENTRY_API */
	rt_entry_t * rtree_entry_create(void *tuple, rt_rect_t *MBR);
	void * rtree_entry_get_tuple(rt_entry_t *entry);
	void rtree_entry_destroy(rt_entry_t *entry);
	/* END RTREE_ENTRY_API */

	/* BEGIN RTREE_RECTANGLE_API */
	rt_rect_t * rtree_rectangle_create(float *low, float *high, uint8_t dimension);
	void rtree_rectangle_destroy(rt_rect_t * rectangle);
	/* END RTREE_RECTANGLE_API */

	/* BEGIN RTREE_API */
	void rtree_insert(rt_rtree_t *rtree, rt_entry_t *entry);
	rt_entry_t * rtree_try_insert(rt_rtree_t *rtree, rt_entry_t *entry);
	rt_rtree_t * rtree_create(rt_ctx_t *context);
	int rtree_delete(rt_rtree_t *rtree, rt_entry_t *entry);
	void rtree_destroy(rt_rtree_t *rtree);
	void rtree_search(rt_rtree_t *rtree, rt_rect_t *search_rectangle, rt_hset_t *results);
	size_t rtree_serialize(rt_rtree_t *rtree, uint8_t **buffer);
#ifdef DEBUG
	void rtree_visualize(rt_rtree_t *rtree);
#endif
	/* END RTREE_API */

#ifdef __cplusplus
}
#endif

#endif /* __WBD_RTREE_API_H__ */