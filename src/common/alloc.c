//
// Created by onur on 11.05.2018.
//
int_t heap_limit = 1 * (1024 * 1024);
int_t used_heap = 0;

Z_INLINE any_ptr_t z_decorate_ptr(any_ptr_t ptr, uint_t size) {
    *((uint_t *) ptr) = size;
    return ptr + sizeof(int_t);
}

Z_INLINE any_ptr_t z_undecorate_ptr(any_ptr_t ptr) {
    ptr -= sizeof(uint_t);
    used_heap -= *((uint_t *) ptr);
    return ptr;
}

void z_free(any_ptr_t ptr);

any_ptr_t z_alloc_or_die(size_t size);

any_ptr_t z_realloc_or_die(any_ptr_t old_ptr, size_t size) {
    any_ptr_t ptr = z_alloc_or_die(size);
    size_t old_size = *((uint_t *) (old_ptr - sizeof(uint_t))) - sizeof(uint_t);
    if (ptr) {
        memcpy(ptr, old_ptr, old_size);
        z_free(old_ptr);
        return ptr;
    } else {
        err_out_of_memory();
    }
}

int_t gc();

Z_INLINE void z_free(any_ptr_t ptr) {
    Z_FREE(z_undecorate_ptr(ptr));
}

Z_INLINE any_ptr_t z_alloc_or_die(size_t size) {
    size += sizeof(uint_t);
    used_heap += size;
    any_ptr_t ptr = Z_MALLOC(size);
    if (ptr == NULL) {
        err_out_of_memory();
    }
    return z_decorate_ptr(ptr, size);
}

Z_INLINE any_ptr_t z_alloc_or_gc(size_t size) {
    size += sizeof(uint_t);
    used_heap += size;
TRY_ALLOCATE:
    if (used_heap > heap_limit) {
        gc();
        if (used_heap > heap_limit) {
            err_out_of_memory();
        } else {
            goto TRY_ALLOCATE;
        }
    }
    any_ptr_t ptr = Z_MALLOC(size);
    if (ptr == NULL) {
        err_out_of_memory();
    }
    ptr = z_decorate_ptr(ptr, size);
    return ptr;
}