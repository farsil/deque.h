/* See LICENSE for license details. */

#ifndef _DEQUE_H_
#define _DEQUE_H_

#include <stdlib.h>
#include <assert.h>

/*
 * Defines the node structure. It holds statically allocated data and the
 * pointer to the next node, as it's only single-linked.
 *
 * Note that this is a substantially different approach from the void* generic
 * queue. It removes the need for the dynamic allocation of the data type, at
 * the cost of a slightly more verbose interface and declaration.
 *
 * Note that to properly use the deque it is necessary to invoke both
 * STRUCT_NODE(T) and STRUCT_DEQUE(T), where T is an arbitrary type, but must
 * be the same for both structures.
 *
 * The name of the struct will be constructed from the parameter type, so it is
 * recommended to typedef this struct. If you ever need to refer to the type
 * again you can use the macro NODE_TYPE(T):
 *
 * // t_node and t_node2 are the same type
 * typedef STRUCT_NODE(int) t_node;
 * typedef NODE_TYPE(int) t_node2;
 */
#define STRUCT_NODE(T)  \
NODE_TYPE(T)            \
{                       \
    T data;             \
    NODE_TYPE(T)* next; \
}

/*
 * Typename of the node which data has type T.
 */
#define NODE_TYPE(T) struct node_##T

/*
 * NODE_TYPE(T)* NODE_ALLOC(typename T)
 *
 * Allocates a node struct which data has type T. Used by DEQUE_PUSH and
 * DEQUE_APPEND. It uses malloc() internally.
 */
#define NODE_ALLOC(T) (NODE_TYPE(T)*)malloc(sizeof(NODE_TYPE(T)))

/*
 * void NODE_FREE(NODE_TYPE(T)* node)
 *
 * Deallocates a node struct. Used by DEQUE_POP. It uses free() internally.
 */
#define NODE_FREE(node) free((node))

/*
 * NODE_TYPE(T)* NODE_NEXT(NODE_TYPE(T)* node);
 *
 * Returns the pointer to the next node.
 */
#define NODE_NEXT(node) ((node)->next)

/*
 * T NODE_DATA(NODE_TYPE(T)* node);
 *
 * Returns the data that the node contains. It is possible to modify the data
 * of a node by using this macro on the left hand side.
 */
#define NODE_DATA(node) ((node)->data)

/*
 * Defines the deque structure. Holds a pointer to the head and the tail of the
 * queue. It also holds a size field that allows to check the deque size in
 * O(1).
 *
 * Note that to properly use the deque it is necessary to invoke both
 * STRUCT_NODE(T) and STRUCT_DEQUE(T), where T is an arbitrary type, but must
 * be the same for both structures.
 *
 * Again, the name of the struct will be constructed from the parameter T, so
 * it should be typedef'd. If you ever need to refer to the type
 * again you can use the macro DEQUE_TYPE(T):
 *
 * // t_deque and t_deque2 are the same type
 * typedef STRUCT_DEQUE(int) t_deque;
 * typedef DEQUE_TYPE(int) t_deque2;
 */
#define STRUCT_DEQUE(T)     \
DEQUE_TYPE(T)               \
{                           \
    NODE_TYPE(T)* first;    \
    NODE_TYPE(T)* last;     \
    size_t size;            \
}

/*
 * Typename of the deque containing nodes which data have type T.
 */
#define DEQUE_TYPE(T) struct deque_##T

/*
 * NODE_TYPE(T)* DEQUE_FIRST(DEQUE_TYPE(T)* deque);
 *
 * Returns a pointer to the first node. Mostly used with NODE_NEXT to iterate
 * over a deque, eg:
 *
 * for (NODE_TYPE(T)* node = DEQUE_FIRST(deque); node != NULL;
 *      node = NODE_NEXT(node))
 * {
 *      // your code using node here
 * }
 */
#define DEQUE_FIRST(deque) ((deque)->first)

/*
 * NODE_TYPE(T)* DEQUE_LAST(DEQUE_TYPE(T)* deque);
 *
 * Returns a pointer to the last node. Mostly used internally, but can be
 * useful sometimes.
 */
#define DEQUE_LAST(deque) ((deque)->last)

/*
 * size_t DEQUE_SIZE(DEQUE_TYPE(T)* deque);
 *
 * Returns the deque size.
 */
#define DEQUE_SIZE(deque) ((deque)->size)

/*
 * T DEQUE_HEAD(DEQUE_TYPE(T)* deque);
 *
 * Returns the content of the first node. Not NULL-safe, so you should always
 * check that the queue is not empty before using DEQUE_HEAD:
 *
 * if (DEQUE_SIZE(deque))
 *      val = DEQUE_HEAD(deque);
 */
#define DEQUE_HEAD(deque) NODE_DATA(DEQUE_FIRST((deque)))

/*
 * T DEQUE_TAIL(DEQUE_TYPE(T)* deque);
 *
 * Same as DEQUE_HEAD, but returns the content of the last node.
 */
#define DEQUE_TAIL(deque) NODE_DATA(DEQUE_LAST((deque)))

/*
 * void DEQUE_INIT(DEQUE_TYPE(T)* deque);
 *
 * Initializes the deque to an empty deque.
 */
#define DEQUE_INIT(deque) do        \
{                                   \
    DEQUE_FIRST((deque)) = NULL;    \
    DEQUE_LAST((deque)) = NULL;     \
    DEQUE_SIZE((deque)) = 0;        \
} while (0)

/*
 * Convenience macro that expands to a struct initializer:
 *
 * DEQUE_TYPE(T) deque = DEQUE_DEFAULT;
 */
#define DEQUE_DEFAULT { NULL, NULL, 0 }

/*
 * void DEQUE_PUSH_NODE(DEQUE_TYPE(T)* deque, NODE_TYPE(T)* node);
 *
 * Inserts a node at the beginning of the deque. Note that the node must be
 * preallocated before calling DEQUE_PUSH_NODE.
 */
#define DEQUE_PUSH_NODE(deque, node) do                     \
{                                                           \
    if ((NODE_NEXT((node)) = DEQUE_FIRST((deque))) == NULL) \
        DEQUE_LAST((deque)) = ((node));                     \
    DEQUE_FIRST((deque)) = (node);                          \
    DEQUE_SIZE((deque))++;                                  \
} while (0)

/*
 * void DEQUE_PUSH(DEQUE_TYPE(T)* deque, typename T, T data);
 *
 * Inserts data at the beginning of the deque. Allocates a node using
 * NODE_ALLOC, then sets its data to the provided argument. The typename must
 * be specified because there is no such thing as static type inference in C.
 */
#define DEQUE_PUSH(deque, T, value) do          \
{                                               \
    NODE_TYPE(T)* _new_node = NODE_ALLOC(T);    \
    NODE_DATA(_new_node) = (value);             \
    DEQUE_PUSH_NODE((deque), _new_node);        \
} while (0)

/*
 * void DEQUE_APPEND_NODE(DEQUE_TYPE(T)* deque, NODE_TYPE(T)* node);
 *
 * Same as DEQUE_PUSH_NODE, but inserts at the end of the deque.
 */
#define DEQUE_APPEND_NODE(deque, node) do                   \
{                                                           \
    if (DEQUE_FIRST((deque)) == (NODE_NEXT((node)) = NULL)) \
        DEQUE_FIRST((deque)) = (node);                      \
    else                                                    \
        NODE_NEXT(DEQUE_LAST((deque))) = (node);            \
    DEQUE_LAST((deque)) = (node);                           \
    DEQUE_SIZE((deque))++;                                  \
} while (0)

/*
 * void DEQUE_APPEND(DEQUE_TYPE(T)* deque, typename T, T data);
 *
 * Same as DEQUE_PUSH, but inserts at the end of the deque.
 */
#define DEQUE_APPEND(deque, T, value) do        \
{                                               \
    NODE_TYPE(T)* _new_node = NODE_ALLOC(T);    \
    NODE_DATA(_new_node) = (value);             \
    DEQUE_APPEND_NODE((deque), _new_node);      \
} while (0)

/*
 * void DEQUE_POP_NODE(DEQUE_TYPE(T)* deque);
 *
 * Removes the first element of the deque. Note that it does not free its
 * resources, if you need to do that, use DEQUE_POP instead.
 */
#define DEQUE_POP_NODE(deque) do                                            \
{                                                                           \
    assert((DEQUE_FIRST((deque)) != NULL));                                 \
    if ((DEQUE_FIRST((deque)) = NODE_NEXT(DEQUE_FIRST((deque)))) == NULL)   \
        DEQUE_LAST((deque)) = NULL;                                         \
    DEQUE_SIZE((deque))--;                                                  \
} while (0)

/*
 * void DEQUE_POP(DEQUE_TYPE(T)* deque, typename T);
 *
 * Removes the first element from the deque, deallocating its resources using
 * NODE_FREE.
 */
#define DEQUE_POP(deque, T) do                          \
{                                                       \
    NODE_TYPE(T)* _prev_first = DEQUE_FIRST((deque));   \
    DEQUE_POP_NODE((deque));                            \
    NODE_FREE(_prev_first);                             \
} while (0)


/*
 * void DEQUE_CLEAR(DEQUE_TYPE(T)* deque);
 *
 * Removes all data in the deque. Note that it does not attempt to free the
 * memory used by the nodes, if you need to do that, use DEQUE_FREE instead.
 *
 * Note: alias of DEQUE_INIT, but it could change in the future.
 */
#define DEQUE_CLEAR DEQUE_INIT

/*
 * void DEQUE_FREE(DEQUE_TYPE(T)* deque, typename T);
 *
 * Frees all the nodes in the deque and clears it. This operation has an O(n)
 * complexity.
 */
#define DEQUE_FREE(deque, T) do                                 \
{                                                               \
    NODE_TYPE(T)* _prev_first;                                  \
    while ((_prev_first = DEQUE_FIRST((deque))) != NULL)        \
    {                                                           \
        DEQUE_FIRST((deque)) = NODE_NEXT(DEQUE_FIRST((deque))); \
        NODE_FREE(_prev_first);                                 \
    }                                                           \
    DEQUE_LAST((deque)) = NULL;                                 \
    DEQUE_SIZE((deque)) = 0;                                    \
} while (0)

#endif /* _DEQUE_H_ */
