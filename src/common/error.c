//
// Created by onur on 11.05.2018.
//


int error_and_exit_fmt(const char *format, ...)
{
    va_list args;
    va_start(args, format);
    vfprintf(stderr ,format, args);
    va_end(args);
    exit(1);
}

void error_and_exit(char *message) ;

void err_out_of_memory() {
    error_and_exit("cannot allocate memory");
}

void error_and_exit(char *message) {
    //TODO:print stack trace here
    fprintf(stderr, "%s\n", message);
    exit(1);
}
