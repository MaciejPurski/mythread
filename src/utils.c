#include "utils.h"

void thread_push(thread_t **head, thread_t *n_thread)
{
    if (*head == NULL) {
        *head = n_thread;
    } else if ((*head)->priority > n_thread->priority) {
        n_thread->next = *head;
        *head = n_thread;
    } else {
        thread_t *tmp = *head;
        while (tmp->next && tmp->next->priority <= n_thread->priority) {
            tmp = tmp->next;
        }

        n_thread->next = tmp->next;
        tmp->next = n_thread;
    }
}

thread_t *thread_pop(thread_t **head)
{
    if (*head == NULL)
        return NULL;

    thread_t *old = *head;

    *head = (*head)->next;
    old->next = NULL;

    return old;
}