//
// Created by onur on 11.05.2018.
//

void error_and_exit(char *message) ;

void err_out_of_memory() {
    error_and_exit("cannot allocate memory");
}

void error_and_exit(char *message) {
    //TODO:print stack trace here
    fprintf(stderr, "%s\n", message);
    exit(1);
}
