#include <stdlib.h>
#include <stdio.h>
#include "common.h"
#include "node.h"
#include "thread_props.h"
#include "List_Extras.h"


// int delete_process(List *l, Process_Props * process) {
//     // Check if the head is the process in question
//     if ((l->head) == NULL) {
//         return EXIT_FAILURE;
//     }
//     Process_Props * node_data;
//     struct node *last_n = l->head;
//     struct node *n = l->head->next;
//     if(n == NULL){
//         deleteHead(l);
//         free(last_n->data);
//         free(last_n);
//         return EXIT_SUCCESS;
//     }
//     node_data = n->data;
//     Process_Props * current_data = last_n->data;
//     if(current_data->pid == process->pid){
//         deleteHead(l);
//         free(last_n->data);
//         free(last_n);
//         return 1;
//     }
//     while (n != NULL) {
//         node_data = n->data;
//         if (node_data->pid == process->pid) {
//             // TODO terminate process
//             if(n->next == NULL){
//                 last_n->next = n->next;
//                 l->tail = last_n;
//             }
//             last_n->next = n->next;
//             l->size--;
//             free(n->data);
//             free(n);
//             return 0;
//         }
//         last_n = n;
//         n = n->next;
        
//     }
//     // If the process is not found in the list then return error code -1
//     return -1;
// }

thread_prop * get_by_tid(const List *l, tid_t tid) {
    struct node *n = l->head;
    thread_prop * node_data;
    while (n != NULL) {
        node_data = n->data;
        if (node_data->tid == tid) {
            return node_data;
        }
        n = n->next;
    }
    // If the process cannot be found, return NULL
    return NULL;
}

func_prop * get_by_func(const List *l, void(*func)(void*)) {
    struct node *n = l->head;
    func_prop * node_data;
    while (n != NULL) {
        node_data = n->data;
        if (node_data->func_pointer == func) {
            return node_data;
        }
        n = n->next;
    }
    // If the process cannot be found, return NULL
    return NULL;
}

void addSJF(List *l, thread_prop * tp_insert ){
    struct node *ni= newNode(tp_insert); // Create a new node (item, NULL) to insert
    unsigned long avgTime = fpGetAverageTime(tp_insert->fp);
    if (l->size == 0) // Inserting in empty list
        l->head = l->tail = ni;
    else { // Inserting in a non-empty list
        struct node *n = l->head;
        thread_prop * node_data = n->data;

        if(fpGetAverageTime(node_data->fp) > avgTime){
            addHead(l, tp_insert);
            return;
        }

        while (n->next != NULL) {
            node_data = n->next->data;
            if  (fpGetAverageTime(node_data->fp) > avgTime){
                ni->next = n->next;
                n->next = ni;
                l->size++;
                return;
            }
            n = n->next;
        }
        if (n->next == NULL){ // insert at the end
            n->next = ni;
            l->tail = ni;

        }
    }
    l->size++; // We just inserted an it

}

