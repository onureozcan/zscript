//
// Created by onur on 12.10.2018.
//
arraylist_t *event_queue = NULL;

void call_next_event();

void wait_for_event() {
    while (event_queue) {
        if (event_queue->size) {
            call_next_event();
        }
    };
}

void enqueue_event(int_t *function) {
    arraylist_push(event_queue, function);
}

void call_next_event() {
    z_object_t *function_ref = *(z_object_t **) arraylist_get(event_queue, 0);
    z_interpreter_state_t *other_state = function_ref->function_ref_object.responsible_interpreter_state;
    z_object_t *called_fnc = context_new();
    called_fnc->context_object.parent_context = function_ref->function_ref_object.parent_context;
    called_fnc->context_object.return_context = NULL;
    called_fnc->context_object.return_address = NULL;
    called_fnc->context_object.requested_return_register_index = NULL;
    other_state->instruction_pointer = (function_ref->function_ref_object.start_address);
    other_state->current_context = called_fnc;
    other_state = z_interpreter_run(other_state);
    if (other_state->return_code) {
        error_and_exit(other_state->exception_details);
    }
    if (event_queue)
        arraylist_remove_index(event_queue, 0);
}