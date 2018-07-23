#include <iostream>
#include <antlr4-runtime.h>
#include <fstream>
#include <stack>
#include <cstring>

using namespace std;
using namespace antlr4;
using namespace tree;

#include "common/common.h"
#include "compiler/Compiler.cpp"
#include "interpreter/interpreter_dd.c"

int main(int argc, const char *argv[]) {
    std::ifstream stream;
    bool runMode = false;
    const char *compile_flag = NULL;
    const char *filename = NULL;
    const char *oFilename = NULL;

    //compile to file
    if (argc == 4) {
        compile_flag = argv[1];
        filename = argv[2];
        oFilename = argv[3];
        stream.open(filename);

    } else {
        //run as a script
        filename = argv[1];
        stream.open(filename);
        runMode = true;
    }

    ANTLRInputStream input(stream);
    zeroscriptLexer lexer(&input);
    CommonTokenStream tokens(&lexer);
    zeroscriptParser parser(&tokens);
    AstGenerator *astGenerator = new AstGenerator(parser.classDeclaration());
    ClassDeclaration* cls = astGenerator->getRootClass();
    Compiler *compiler = new Compiler(cls);
    long len = 0;
    char *bytes = compiler->toBytes(&len);

    if (runMode) {
        clock_t begin = clock();
        z_interpreter_run(bytes, len);
        clock_t end = clock();
        double time_spent = (double)(end - begin) / CLOCKS_PER_SEC;
        printf("time spent: %lf\n", time_spent);
    } else {
        FILE *ofile = fopen(oFilename, "wb");
        fwrite(bytes, static_cast<size_t>(len), 1, ofile);
        fclose(ofile);
    }
    return 0;
}