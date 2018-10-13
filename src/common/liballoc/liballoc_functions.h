
int liballoc_lock()
{
    return 0;
}

int liballoc_unlock()
{
    return 0;
}

void* liballoc_alloc( int pages )
{
    return malloc((size_t) (pages * l_pageSize));
}

int liballoc_free( void* ptr, int pages )
{
    free(ptr);
    return 0;
}