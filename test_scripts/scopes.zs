class Scopes(){

    var a = 10;
    function test1() {
        print("value of `a` from the parent scope is " + a);
    }
    function closureTest(){
        var x = "x";
        return ()=>{
            print("x from the closureTest is :"+x);
            print("a from parent scope is :"+a);
        };
    }

    function setA(b){
        a = b;
    }

    test1();
    closureTest()();
}