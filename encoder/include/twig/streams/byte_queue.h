#pragma once
#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>

typedef struct {
    char *buffer;
    size_t cap;
    size_t size;
    char *rd, *wr;
    bool op_ok;
} queue_t;


#define QUEUE_EMPTY(q) ((q)->size == 0)
#define QUEUE_FULL(q) ((q)->size == (q)->cap)
#define QUEUE_RESET(q) ((q)->size = 0, (q)->rd = (q)->buffer, (q)->wr = (q)->buffer, (q)->op_ok = true)
#define QUEUE_AVAIL(q) ((q)->cap - (q)->size)

/**
 * Initialize a queue.
 * @param self the queue
 * @param buffer the location of the underlying buffer
 * @param size the size of the underlying buffer, in bytes
 * @return pointer to the initialized queue.
 */
static inline queue_t *queue_init(queue_t *self, char *buf, size_t size);


/**
 * Get the first element of the queue
 * @param self the queue
 * @return first element in the queue
 */
static inline char queue_pop(queue_t *self);

/**
 * Add a new element of a given size to the queue.
 * @param self the queue
 * @param val pointer to the value to push
 */
static inline void queue_push(queue_t *self, char val);

/**
 * View the last element without removal.
 * @param self the queue
 * @return value on top of queue.
 */
static inline char queue_peek(queue_t *self);


static inline queue_t *queue_init(queue_t *self, char *buf, size_t size) {
    self->buffer = buf;
    self->cap = size;
    self->size = 0;
    self->rd = self->buffer;
    self->wr = self->buffer;
    self->op_ok = true;
    return self;
}


static inline char queue_pop(queue_t *self) {
    char val = 0;
    if(self->size > 0){
        self->op_ok = true;
        self->size--;
        val = *self->rd;
        if((uintptr_t)(self->rd - self->buffer) == (uintptr_t)(self->cap - 1)) {
            self->rd = self->buffer;
        }
        else {
            self->rd++;
        }
    }
    else {
        self->op_ok = false;
    }
    return val;
}


static inline void queue_push(queue_t *self, char val) {
    if(self->size < self->cap) {
        *self->wr = val;
        self->size++;
        if(self->wr == self->buffer + self->cap - 1) {
            self->wr = self->buffer;
        }
        else {
            self->wr++;
        }
        self->op_ok = true;
    }
    else {
        self->op_ok = false;
    }
}

static inline char queue_peek(queue_t *self) {
    char val = 0;
    if(self->size > 0) {
        self->op_ok = true;
        val = *self->rd;
    }
    else {
        self->op_ok = false;
    }
    return val;
}
