class ScopesTest(){

    var a = 10;
    function test1() {
        //read from the parent scope
        assert(a == 10);
    }
    function closureTest(){
        var x = "x";
        return ()=>{
            assert(a == 10);
            // make sure that closure parent scopes don't disappear
            assert(x.equals("x"));
        };
    }

    test1();
    closureTest()();
}