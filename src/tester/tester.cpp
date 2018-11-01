//
// Created by onur on 25.10.2018.
//

#include <cstdio>
#include <cstdlib>
#include <cassert>
#include <cstring>

// test if a given script behaves as expected without corrupting memory
void test(const char *script) {
    printf("%s..............",script);
    const char *template_str = "valgrind --quiet --memcheck:leak-check=no --error-exitcode=1 ./zero %s >> /dev/null";
    size_t len = strlen(template_str) + strlen(script) + 1;
    char *buff = (char *) malloc(len);
    snprintf(buff, len, template_str, script);
    assert(system(buff) == 0);
    printf("OK\n");
}

int main() {
    test("../test_scripts/OperatorsTest.zs");
    test("../test_scripts/primetest.zs");
    test("../test_scripts/ClassInstanceTest.zs");
    test("../test_scripts/JsonTest.zs");
    test("../test_scripts/ThrowTest.zs");
    test("../test_scripts/ScopesTest.zs");
    test("../test_scripts/AsyncFunctionTest.zs");
    test("../test_scripts/RecursiveFibonacci.zs");
    test("../test_scripts/CombinatorFunctionTest.zs");
    return 0;
}