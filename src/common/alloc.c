//
// Created by onur on 11.05.2018.
//
int_t heap_limit = 1 * (1024 * 1024);
int_t used_heap = 0;
int_t total_thread_count = 1;
int_t gc_busy = 0;

pthread_mutex_t used_heap_lock = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t gc_list_lock = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t gc_busy_lock = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t thread_list_lock = PTHREAD_MUTEX_INITIALIZER;

pthread_cond_t gc_busy_cond = PTHREAD_COND_INITIALIZER;

pthread_barrier_t gc_safe_barrier;

#define USED_HEAP_LOCK pthread_mutex_lock(&used_heap_lock);
#define USED_HEAP_UNLOCK pthread_mutex_unlock(&used_heap_lock);

#define GC_LIST_LOCK pthread_mutex_lock(&gc_list_lock);
#define GC_LIST_UNLOCK pthread_mutex_unlock(&gc_list_lock);

#define GC_BUSY_LOCK pthread_mutex_lock(&gc_busy_lock);
#define GC_BUSY_UNLOCK pthread_mutex_unlock(&gc_busy_lock);

#define THREAD_LIST_LOCK pthread_mutex_lock(&thread_list_lock);
#define THREAD_LIST_UNLOCK pthread_mutex_unlock(&thread_list_lock);

void schedule_gc();

Z_INLINE any_ptr_t z_decorate_ptr(any_ptr_t ptr, uint_t size) {
    *((uint_t *) ptr) = size;
    return ptr + sizeof(int_t);
}

Z_INLINE any_ptr_t z_undecorate_ptr(any_ptr_t ptr) {
    ptr -= sizeof(uint_t);
    USED_HEAP_LOCK
    used_heap -= *((uint_t *) ptr);
    USED_HEAP_UNLOCK
    return ptr;
}

void z_free(any_ptr_t ptr);

any_ptr_t z_alloc_or_die(size_t size);

int_t gc();

Z_INLINE void z_free(any_ptr_t ptr) {
    Z_FREE(z_undecorate_ptr(ptr));
}

any_ptr_t z_alloc_or_die(size_t size) {
    size += sizeof(uint_t);
    USED_HEAP_LOCK
    used_heap += size;
    USED_HEAP_UNLOCK
    any_ptr_t ptr = Z_MALLOC(size);
    if (ptr == NULL) {
        err_out_of_memory();
    }
    return z_decorate_ptr(ptr, size);
}

any_ptr_t z_alloc_or_gc(size_t size) {
    size += sizeof(uint_t);
    USED_HEAP_LOCK
    used_heap += size;
    USED_HEAP_UNLOCK
    if(used_heap > heap_limit){
        schedule_gc();
    }
    any_ptr_t ptr = Z_MALLOC(size);
    if (ptr == NULL) {
        err_out_of_memory();
    }
    ptr = z_decorate_ptr(ptr, size);
    return ptr;
}
/*
 * //herkes nasilsa buraya gelecek
 * gc(){inux des
 *      1- giris bariyeri (diger herkesin gc noktasina ulasmasini bekle)
 *      2- main isen gc yap degilsen gc'nin bitmeisni bekle
 * }
 * */
void schedule_gc() {
    GC_BUSY_LOCK
    gc_busy = 1;
    pthread_cond_broadcast(&gc_busy_cond);
    GC_BUSY_UNLOCK
//
//    printf("awaiting gc barrier 1. thread count: %d\n",total_thread_count);
    int ret = pthread_barrier_wait(&gc_safe_barrier);
//    puts("reached");
    if (ret == PTHREAD_BARRIER_SERIAL_THREAD) {
        gc();
    }
//    puts("awaiting gc barrier 2");
    pthread_barrier_wait(&gc_safe_barrier);
//    puts("reached");

    GC_BUSY_LOCK
    gc_busy = 0;
    pthread_cond_broadcast(&gc_busy_cond);
    GC_BUSY_UNLOCK
}

void init_gc_barrier(int_t count){
    pthread_barrier_destroy(&gc_safe_barrier);
    pthread_barrier_init(&gc_safe_barrier, NULL, (unsigned int) count);
}