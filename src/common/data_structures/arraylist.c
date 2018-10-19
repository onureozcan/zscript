#include "../types.h"

#define ARRAY_LIST_INITIAL_CAPACITY 3

typedef struct arraylist_t {
    int_t size;
    int_t capacity;
    char *data;
    char *top;
    size_t size_of_item;
} arraylist_t;

Z_INLINE static void extend_capacity(arraylist_t *self);

arraylist_t *arraylist_new_capacity(size_t sizeof_item, int_t initial_capacity) {
    arraylist_t *self = (arraylist_t *) z_alloc_or_die(sizeof(arraylist_t));
    self->capacity = initial_capacity;
    self->size_of_item = sizeof_item;
    self->data = (char *) z_alloc_or_die(sizeof_item * initial_capacity);
    self->size = 0;
    self->top = &self->data[0];
    return self;
}

/**
 * initializes a new arraylist.
 * @param sizeof_item size of an item in the list.
 * @return arraylist ptr.
 */
arraylist_t *arraylist_new(size_t sizeof_item) {
    return arraylist_new_capacity(sizeof_item, ARRAY_LIST_INITIAL_CAPACITY);
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
    return self->top - self->size_of_item;
}

Z_INLINE any_ptr_t arraylist_pop(arraylist_t *self) {
    self->size--;
    self->top -= self->size_of_item;
    return self->top;
}

Z_INLINE any_ptr_t arraylist_get(arraylist_t *self, int_t index) {
    return (any_ptr_t) (self->data + index * self->size_of_item);
}

Z_INLINE static void extend_capacity(arraylist_t *self) {
    char* new_data = (char*) z_alloc_or_die(self->capacity * self->size_of_item * 2);
    memcpy(new_data, self->data, self->size * self->size_of_item);
    z_free(self->data);
    self->data = new_data;
    self->capacity = self->capacity * 2;
    self->top = &self->data[self->size * self->size_of_item];
}

void arraylist_remove_item(arraylist_t *self, any_ptr_t ptr) {
    for (int_t i = 0; i < self->size; i++) {
        any_ptr_t next = arraylist_get(self, i);
        int_t found = TRUE;
        for (int_t j = 0; j < self->size_of_item; j++) {
            if (((char *) ptr)[j] != ((char *) next)[j]) {
                found = FALSE;
                break;
            }
        }
        if (found) {
            self->size--;
            memcpy(next, arraylist_get(self, i + 1), (size_t) (self->size - i));
            break;
        }
    }
}

void arraylist_remove_index(arraylist_t *self, int_t i) {
    any_ptr_t next = arraylist_get(self, i);
    self->size--;
    memcpy(next, arraylist_get(self, i + 1), (size_t) (self->size - i));
}

void arraylist_free(arraylist_t *self) {
    z_free(self->data);
    z_free(self);
}
