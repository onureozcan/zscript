//
// Created by onur on 11.05.2018.
//

void err_out_of_memory() {

}

void error_and_exit(char *message) {
    //TODO:print stack trace here
    fprintf(stderr, "%s\n", message);
    exit(1);
}
