class VarargFunctionTest(){

    // call with no arg
    vararg();
    // cal with 1 arg
    vararg("a");
    // call with 2 args
    vararg("a","b");
    // call with 3 args
    vararg("a","b","c");
    // call with 4 agrs
    vararg("a","b","c","d");

    function vararg(a,b,c){
        print(""+a+"|"+b+"|"+c);
    }

}