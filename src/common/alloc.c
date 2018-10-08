//
// Created by onur on 11.05.2018.
//
uint_t heap_limit = 5 * (1024 * 1024);
uint_t used_heap = 0;

Z_INLINE any_ptr_t z_decorate_ptr(any_ptr_t ptr, uint_t size) {
    *((uint_t *) ptr) = size;
    return ptr + sizeof(int_t);
}

Z_INLINE any_ptr_t z_undecorate_ptr(any_ptr_t ptr) {
    ptr -= sizeof(uint_t);
    used_heap -= *((uint_t *) ptr);
    return ptr;
}

any_ptr_t z_realloc_or_die(any_ptr_t old_ptr, size_t size) {
    any_ptr_t ptr = Z_REALLOC(old_ptr - sizeof(uint_t), size + sizeof(uint_t));
    if (ptr == NULL) {
        err_out_of_memory();
    }
    return z_decorate_ptr(ptr, size);
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
        printf("memory request failed, gc..\n");
        gc();
        if (used_heap > heap_limit) {
            err_out_of_memory();
        } else{
            printf("recover from gc\n");
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