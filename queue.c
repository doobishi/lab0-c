#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "list.h"
#include "queue.h"

/* Create an empty queue */
struct list_head *q_new()
{
    struct list_head *head = malloc(sizeof(struct list_head));
    if (!head)
        return NULL;

    head->next = head;
    head->prev = head;

    return head;
}

/* Free all storage used by queue */
void q_free(struct list_head *head)
{
    if (!head)
        return;

    struct list_head *curr, *safe;
    list_for_each_safe(curr, safe, head) {
        element_t *entry = list_entry(curr, element_t, list);
        free(entry->value);
        free(entry);
    }

    free(head);
}

element_t *_alloc_newnode(char *s)
{
    element_t *new_node = malloc(sizeof(element_t));
    if (!new_node)
        return NULL;

    /* 1. Allocate and copy string s to new_node->value */
    new_node->value = strdup(s);
    if (!new_node->value) {
        free(new_node);  // Clean up to avoid memory leak
        return NULL;
    }

    return new_node;
}

/* Insert an element at head of queue */
bool q_insert_head(struct list_head *head, char *s)
{
    if (!head)
        return false;

    element_t *new_node = _alloc_newnode(s);
    if (!new_node)
        return false;

    /* 2. Use macro list_add to link the node */
    list_add(&new_node->list, head);

    return true;
}

/* Insert an element at tail of queue */
bool q_insert_tail(struct list_head *head, char *s)
{
    if (!head)
        return false;

    element_t *new_node = _alloc_newnode(s);
    if (!new_node)
        return false;

    list_add(&new_node->list, head->prev);
    return true;
}

/* Remove an element from head of queue */
element_t *q_remove_head(struct list_head *head, char *sp, size_t bufsize)
{
    if (!head || list_empty(head))
        return NULL;

    element_t *target = list_entry(head->next, element_t, list);

    if (sp && target->value) {
        snprintf(sp, bufsize, "%s", target->value);
    }

    list_del(&target->list);

    return target;
}

/* Remove an element from tail of queue */
element_t *q_remove_tail(struct list_head *head, char *sp, size_t bufsize)
{
    if (!head || list_empty(head))
        return NULL;

    element_t *target = list_entry(head->prev, element_t, list);

    if (sp && target->value) {
        snprintf(sp, bufsize, "%s", target->value);
    }

    list_del(&target->list);

    return target;
}

/* Return number of elements in queue */
int q_size(struct list_head *head)
{
    if (!head)
        return 0;

    int count = 0;
    struct list_head *node;

    list_for_each(node, head) {
        count++;
    }

    return count;
}

/* Delete the middle node in queue */
bool q_delete_mid(struct list_head *head)
{
    if (!head || list_empty(head))
        return false;

    struct list_head *slow = head->next;
    struct list_head *fast = head->next;

    // 2. Move fast 2 steps and slow 1 step
    // In a circular list, we stop when fast or fast->next hits 'head'
    while (fast->next != head && fast->next->next != head) {
        slow = slow->next;
        fast = fast->next->next;
    }

    // 3. Delete the 'slow' node
    list_del(slow);

    // 4. Free the memory (assuming the node is wrapped in a struct)
    element_t *entry = list_entry(slow, element_t, list);
    free(entry->value);
    free(entry);

    return true;
}

/* Delete all nodes that have duplicate string */
bool q_delete_dup(struct list_head *head)
{
    if (!head || list_empty(head) || !head->next->next)
        return false;
    struct list_head *std = head->next;
    struct list_head *nxt_of_std = std->next;
    while (std != head && nxt_of_std != head) {
        element_t *ele_std = list_entry(std, element_t, list);
        const element_t *ele_nxt_std = list_entry(nxt_of_std, element_t, list);
        if (strcmp(ele_std->value, ele_nxt_std->value) == 0) {
            while (nxt_of_std != head &&
                   !strcmp(ele_std->value, ele_nxt_std->value)) {
                struct list_head *dup = nxt_of_std;
                nxt_of_std = nxt_of_std->next;
                ele_nxt_std = list_entry(nxt_of_std, element_t, list);

                list_del(dup);
                element_t *ele_dup = list_entry(dup, element_t, list);
                free(ele_dup->value);
                free(ele_dup);
            }
            list_del(std);
            free(ele_std->value);
            free(ele_std);
            std = nxt_of_std;
            nxt_of_std = std->next;
        } else {
            std = nxt_of_std;
            nxt_of_std = std->next;
        }
    }

    return true;
}

/* Swap every two adjacent nodes */
void q_swap(struct list_head *head)
{
    if (!head || list_empty(head) || list_is_singular(head))
        return;

    for (struct list_head *node = head->next;
         node != head && node->next != head; node = node->next) {
        list_move(node, node->next);
    }
}

/* Reverse elements in queue */
void q_reverse(struct list_head *head)
{
    struct list_head *node, *safe;
    for (node = (head)->next, safe = node->next; node != (head) && safe != head;
         node = safe, safe = node->next) {
        list_move(safe, head);
        safe = node;
    }
}

/* Reverse the nodes of the list k at a time */
void q_reverseK(struct list_head *head, int k)
{
    int len = 0;
    struct list_head *group_start = NULL;
    list_for_each(group_start, head) {
        len++;
    }

    if (!head || list_empty(head) || k <= 1)
        return;
    int iter_count = len / k;
    if (iter_count == 0)
        return;

    group_start = head->next;
    while (iter_count > 0) {
        struct list_head *next = group_start->next;
        int group_iter = k - 1;
        while (group_iter > 0) {
            struct list_head *move_to_front = next;
            list_del(move_to_front);
            list_add(move_to_front, group_start->prev);
            next = group_start->next;
            group_start = move_to_front;
            group_iter--;
        }

        group_start = next->next;
        iter_count--;
    }
}

/* Sort elements of queue in ascending/descending order */
void q_sort(struct list_head *head, bool descend) {}

/* Remove every node which has a node with a strictly less value anywhere to
 * the right side of it */
int q_ascend(struct list_head *head)
{
    if (!head || list_empty(head) || list_is_singular(head)) {
        return 0;
    }
    struct list_head *curr = head->next;
    struct list_head *next = curr->next;
    const element_t *curr_entry = list_entry(curr, element_t, list);
    const char *current_max_val = curr_entry->value;

    while (next != head) {
        curr_entry = list_entry(curr, element_t, list);
        if (strcmp(curr_entry->value, current_max_val) < 0) {
            struct list_head *to_delete = curr;
            curr = next;
            list_del(to_delete);
            element_t *to_delete_entry = list_entry(to_delete, element_t, list);
            free(to_delete_entry->value);
            free(to_delete_entry);
        } else {
            curr = next;
        }
        next = curr->next;
    }
    return 1;
}

/* Remove every node which has a node with a strictly greater value anywhere to
 * the right side of it */
int q_descend(struct list_head *head)
{
    // https://leetcode.com/problems/remove-nodes-from-linked-list/
    return 0;
}

/* Merge all the queues into one sorted queue, which is in ascending/descending
 * order */
int q_merge(struct list_head *head, bool descend)
{
    // https://leetcode.com/problems/merge-k-sorted-lists/
    return 0;
}
