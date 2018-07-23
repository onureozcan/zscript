//
// Created by onur on 05.06.2018.
//

#define MAP_BAG_SIZE 100

typedef struct map_t {
    uint_t size;
    uint_t size_of_an_item;
    arraylist_t *keys[MAP_BAG_SIZE];
    arraylist_t *values[MAP_BAG_SIZE];
} map_t;

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
    for (int i = 0; i < MAP_BAG_SIZE; i++) {
        self->keys[i] = NULL;
        self->values[i] = NULL;
    }
    return self;
}

void map_insert(map_t *self, char *key, any_ptr_t value) {
    uint_t hash = get_hash(key);
    arraylist_t *vbag = self->values[hash];
    arraylist_t *kbag = self->keys[hash];
    if (!vbag) {
        vbag = arraylist_new(self->size_of_an_item);
        kbag = arraylist_new(sizeof(char *));
        self->values[hash] = vbag;
        self->keys[hash] = kbag;
    }
    for (uint_t i = 0; i < kbag->size; i++) {
        if (strcmp(*(char **) arraylist_get(kbag, i), key) == 0) {
            arraylist_set(vbag,value,i);
            return;
        }
    }
    self->size++;
    arraylist_push(kbag, &key);
    arraylist_push(vbag, value);
}

Z_INLINE any_ptr_t map_get(map_t *self, char *key) {
    uint_t hash = get_hash(key);
    arraylist_t *kbag = self->keys[hash];
    arraylist_t *vbag = self->values[hash];
    if (kbag) {
        for (uint_t i = 0; i < kbag->size; i++) {
            if (strcmp(*(char **) arraylist_get(kbag, i), key) == 0) {
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
        if (kbag)
            for (int j = 0; j < kbag->size; j++) {
                arraylist_push(keys, arraylist_top(kbag));
            }
    }
    return keys;
}