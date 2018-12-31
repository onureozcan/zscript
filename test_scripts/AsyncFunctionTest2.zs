class AsyncFunctionTest2(){

    tshared var limit = 100000;

    // when more than one thread tries to call the same function state it used to crash the vm.
    // this test tests if this bug still exists or not
    function longRunningFunction(){
        var i = 0;
        while(i<limit){
            i++;
            // just to trigger gc
            var a = {};
            print(i);
        }
    }

    async function callLongRunningFunctionAsync(){
        longRunningFunction();
    }

    callLongRunningFunctionAsync();
    longRunningFunction();
}