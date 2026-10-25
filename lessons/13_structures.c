#include <stdio.h>
#include <stdlib.h>

typedef struct Node {
    int value;
    struct Node *next;
} Node;

static int push_front(Node **head, int value)
{
    Node *node = malloc(sizeof *node);
    if (node == NULL) {
        return 0;
    }
    node->value = value;
    node->next = *head;
    *head = node;
    return 1;
}

static void clear(Node **head)
{
    while (*head != NULL) {
        Node *old = *head;
        *head = old->next; /* 释放前读取 next，释放后不再访问 old。 */
        free(old);
    }
}

int main(void)
{
    Node *head = NULL;
    for (int value = 1; value <= 3; ++value) {
        if (!push_front(&head, value)) {
            clear(&head);
            fputs("allocation failed\n", stderr);
            return 1;
        }
    }
    int sum = 0;
    for (const Node *node = head; node != NULL; node = node->next) {
        printf("%d ", node->value);
        sum += node->value;
    }
    printf("sum=%d\n", sum);
    clear(&head);
    printf("empty=%d\n", head == NULL);
    return 0;
}
