//
// Created by onur on 11.05.2018.
//

any_ptr_t z_realloc_or_die(any_ptr_t old_ptr,size_t size) {
    any_ptr_t ptr = Z_REALLOC(old_ptr,size);
    if (ptr == NULL) {
        err_out_of_memory();
    }
    return ptr;
}

Z_INLINE any_ptr_t z_alloc_or_die(size_t size) {
    any_ptr_t ptr = Z_MALLOC(size);
    if (ptr == NULL) {
        err_out_of_memory();
    }
    return ptr;
}