//
// Created by onur on 18.07.2018.
//

#ifndef ZEROSCRIPT_STRING_H
#define ZEROSCRIPT_STRING_H

char* str_to_string(void* _self){
    return ((z_object_t*) _self)->string_object.value;
}

void str_add(void* _self, z_reg_t* r1, z_reg_t* r2){
    z_object_t* self = (z_object_t*) _self;
    char* result = NULL;
    if(r1->type == TYPE_NUMBER){
        result = strconcat((const char *) self->string_object.value, num_to_str(r1->number_val));
    } else if(r1->type == TYPE_STR){
        z_object_t* other = (z_object_t*) r1->val;
        result = strconcat((const char *) self->string_object.value, other->string_object.value);
    } else {
        error_and_exit("not implemented yet.");
    }
    z_object_t* ret_str = string_new(result);
    r2->type = TYPE_STR;
    r2->val = (int_t)(ret_str);
}


#endif //ZEROSCRIPT_STRING_H
