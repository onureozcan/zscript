#include "../types.h"

#define ARRAY_LIST_INITIAL_CAPACITY 3

typedef struct arraylist_t {
    int_t size;
    int_t capacity;
    char *data;
    char *top;
    size_t size_of_item;
} arraylist_t;

//private functions
Z_INLINE static void extend_capacity(arraylist_t *self);

//public functions
/**
 * initializes a new arraylist.
 * @param sizeof_item size of an item in the list.
 * @return arraylist ptr.
 */
arraylist_t *arraylist_new(size_t sizeof_item) {
    arraylist_t *self = (arraylist_t *) z_alloc_or_die(sizeof(arraylist_t));
    self->capacity = ARRAY_LIST_INITIAL_CAPACITY;
    self->size_of_item = sizeof_item;
    self->data = (char *) z_alloc_or_die(sizeof_item * ARRAY_LIST_INITIAL_CAPACITY);
    self->size = 0;
    self->top = &self->data[0];
    return self;
}

/**
 * add an item to the list.
 * @param self this ptr.
 * @param item ptr of item to be added.
 * @return size of the list.
 */
Z_INLINE int_t arraylist_push(arraylist_t *self, any_ptr_t item) {
    if (self->size >= self->capacity) {
        extend_capacity(self);
    }
    memcpy(self->top, (char *) item, self->size_of_item);
    self->top += self->size_of_item;
    self->size++;
    return self->size - 1;
}

/**
 * set the specific index of the array.
 * @param self this ptr.
 * @param item ptr of item to be added.
 * @param index the index.
 */
void arraylist_set(arraylist_t *self, any_ptr_t item, int_t index) {
    if (self->size >= self->capacity) {
        extend_capacity(self);
    }
    memcpy(self->data + (self->size_of_item * index), (char *) item, self->size_of_item);
}
Z_INLINE any_ptr_t arraylist_top(arraylist_t *self) {
    return self->top -self->size_of_item;
}

Z_INLINE any_ptr_t arraylist_pop(arraylist_t *self) {
    self->size--;
    self->top -= self->size_of_item;
    return self->top;
}

Z_INLINE any_ptr_t arraylist_get(arraylist_t *self, int_t index) {
    return (any_ptr_t) (self->data + index * self->size_of_item);
}
//private functions
Z_INLINE static void extend_capacity(arraylist_t *self) {
    self->capacity = self->capacity * 2;
    self->data = (char *) z_realloc_or_die(self->data, self->capacity * self->size_of_item);
    self->top = &self->data[self->size * self->size_of_item];
}

