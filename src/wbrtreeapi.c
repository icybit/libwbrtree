#include "wbrtree/wbrtreeapi.h"
#include "Common.h"
#include "Context.h"
#include "Entry.h"
#include "Rectangle.h"
#include "RTree.h"

/* BEGIN RTREE_CONTEXT_API */
RTREE_PUBLIC rt_ctx_t * rtree_context_create(uint8_t m, uint8_t M, uint8_t dim, size_t(*serialize_fn)(rt_entry_t *, uint8_t **), float alloc_factor, rt_rect_t *space)
{
	return context_create(m, M, dim, serialize_fn, alloc_factor, space);
}

RTREE_PUBLIC void rtree_context_destroy(rt_ctx_t *ctx)
{
	context_destroy(ctx);
}
/* END RTREE_CONTEXT_API */

/* BEGIN RTREE_ENTRY_API */
RTREE_PUBLIC rt_entry_t * rtree_entry_create(void *tuple, rt_rect_t *MBR)
{
	return entry_create(tuple, MBR);
}

RTREE_PUBLIC void * rtree_entry_get_tuple(rt_entry_t *entry)
{
	return entry_get_tuple(entry);
}

RTREE_PUBLIC void rtree_entry_destroy(rt_entry_t *entry)
{
	entry_destroy(entry);
}
/* END RTREE_ENTRY_API */

/* BEGIN RTREE_RECTANGLE_API */
RTREE_PUBLIC rt_rect_t * rtree_rectangle_create(float *low, float *high, uint8_t dimension)
{
	return rectangle_create(low, high, dimension);
}

RTREE_PUBLIC void rtree_rectangle_destroy(rt_rect_t * rectangle)
{
	rectangle_destroy(rectangle);
}
/* END RTREE_RECTANGLE_API */

/* BEGIN RTREE_API */
RTREE_PUBLIC void rtree_insert(rt_rtree_t *rtree, rt_entry_t *entry)
{
	insert(rtree, entry);
}

RTREE_PUBLIC rt_entry_t * rtree_try_insert(rt_rtree_t *rtree, rt_entry_t *entry)
{
	return try_insert(rtree, entry);
}

RTREE_PUBLIC rt_rtree_t * rtree_create(rt_ctx_t *context)
{
	return create(context);
}

RTREE_PUBLIC int rtree_delete(rt_rtree_t *rtree, rt_entry_t *entry)
{
	return delete(rtree, entry);
}

RTREE_PUBLIC void rtree_destroy(rt_rtree_t *rtree)
{
	destroy(rtree);
}

RTREE_PUBLIC void rtree_search(rt_rtree_t *rtree, rt_rect_t *search_rectangle, rt_hset_t *results)
{
	search(rtree, search_rectangle, results);
}

RTREE_PUBLIC size_t rtree_serialize(rt_rtree_t *rtree, uint8_t **buffer)
{
	return serialize(rtree, buffer);
}

RTREE_PUBLIC rt_rtree_t * rtree_split(rt_rtree_t *rtree)
{
	return split(rtree);
}

#ifdef DEBUG
RTREE_PUBLIC void rtree_visualize(rt_rtree_t *rtree)
{
	visualize(rtree);
}
#endif
/* END RTREE_API */