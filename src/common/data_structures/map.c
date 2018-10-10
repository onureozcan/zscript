//
// Created by onur on 05.06.2018.
//
// TODO : dynamically increase and decrease size
#define MAP_BAG_SIZE 10
//2^0
#define MAP_FLAG_ENUMERABLE 1
//2^1
#define MAP_FLAG_PRIVATE 2

typedef struct map_t {
    uint_t size;
    uint_t size_of_an_item;
    uint_t is_immutable;
    arraylist_t *keys[MAP_BAG_SIZE];
    arraylist_t *values[MAP_BAG_SIZE];
    arraylist_t *flags[MAP_BAG_SIZE];
} map_t;

void map_insert_flags(map_t *self, char *key, any_ptr_t value, int_t is_enumerable);

any_ptr_t map_get_flags(map_t *self, char *key, int_t* flags) ;

Z_INLINE static uint_t get_hash(char *value) {
    uint_t hash = 0;
    while (*value)
        hash = (hash * 10) + *value++ - '0';
    return hash % MAP_BAG_SIZE;
}

map_t *map_new(uint_t siz_of_an_item) {
    map_t *self = (map_t *) z_alloc_or_die(sizeof(map_t));
    self->size = 0;
    self->size_of_an_item = siz_of_an_item;
    self->is_immutable = 0;
    for (int i = 0; i < MAP_BAG_SIZE; i++) {
        self->keys[i] = NULL;
        self->values[i] = NULL;
        self->flags[i] = NULL;
    }
    return self;
}

void map_insert(map_t *self, char *key, any_ptr_t value) {
    map_insert_flags(self, key, value, 0|MAP_FLAG_ENUMERABLE);
}

void map_insert_non_enumerable(map_t *self, char *key, any_ptr_t value) {
    map_insert_flags(self, key, value, 0);
}

void map_insert_flags(map_t *self, char *key, any_ptr_t value, int_t flag) {
    if (self->is_immutable) return;
    uint_t hash = get_hash(key);
    arraylist_t *vbag = self->values[hash];
    arraylist_t *kbag = self->keys[hash];
    arraylist_t *flags = self->flags[hash];
    if (!vbag) {
        vbag = arraylist_new(self->size_of_an_item);
        kbag = arraylist_new(sizeof(char *));
        flags = arraylist_new(sizeof(int_t));
        self->values[hash] = vbag;
        self->keys[hash] = kbag;
        self->flags[hash] = flags;
    }
    for (uint_t i = 0; i < kbag->size; i++) {
        if (strcmp(*(char **) arraylist_get(kbag, i), key) == 0) {
            arraylist_set(vbag, value, i);
            arraylist_set(flags, &flag, i);
            return;
        }
    }
    self->size += flag & MAP_FLAG_ENUMERABLE;
    arraylist_push(kbag, &key);
    arraylist_push(vbag, value);
    arraylist_push(flags, &flag);
}

Z_INLINE any_ptr_t map_get(map_t *self, char *key) {
    return map_get_flags(self,key,NULL);
}

Z_INLINE any_ptr_t map_get_flags(map_t *self, char *key, int_t* flags) {
        uint_t hash = get_hash(key);
        arraylist_t *kbag = self->keys[hash];
        arraylist_t *vbag = self->values[hash];
        if (kbag) {
            for (uint_t i = 0; i < kbag->size; i++) {
                if (strcmp(*(char **) arraylist_get(kbag, i), key) == 0) {
                    if(flags){
                        *flags = *((int *) arraylist_get(self->flags[hash], i));
                    }
                    return arraylist_get(vbag, i);
                }
            }
        }
        return NULL;
}

arraylist_t *map_key_list(map_t *self) {
    arraylist_t *keys = arraylist_new(sizeof(char *));
    for (int i = 0; i < MAP_BAG_SIZE; i++) {
        arraylist_t *kbag = self->keys[i];
        arraylist_t *fbag = self->flags[i];
        if (kbag)
            for (int_t j = 0; j < kbag->size; j++) {
                int_t flag = *((int *) arraylist_get(fbag, j));
                if (flag & MAP_FLAG_ENUMERABLE)
                    arraylist_push(keys, &(*(char **) arraylist_get(kbag, j)));
            }
    }
    return keys;
}

void map_free(map_t* self){
    for (int_t i = 0; i < MAP_BAG_SIZE; i++) {
        arraylist_t *kbag = self->keys[i];
        arraylist_t *fbag = self->flags[i];
        arraylist_t *vbag = self->values[i];
        if (kbag){
            arraylist_free(kbag);
            arraylist_free(vbag);
            arraylist_free(fbag);
        }
    }
    z_free(self);
}