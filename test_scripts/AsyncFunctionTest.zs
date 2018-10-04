import "../test_scripts/Math" as Math

class AsyncFucntionTest(){

    var a = 10;
    asyncFnc();
    synchronousFnc();
    asyncFnc2();
    asynFncWithThrow();
    asyncFnc3();

    function synchronousFnc(){
        for(var i = 0;i<10;i++){
            print(i);
        }
    }

    async function asyncFnc(){
        for(var i = 0;i<10;i++){
            print(i);
        }
    }

    async function asynFncWithThrow(){
        try {
            throw "why are you calling me?";
        }catch(e){
            print(e);
        }
    }
    // access other classes inside an async function
    async function asyncFnc2(){
        print("rounded:"+Math.round(11.2));
    }

    // cannot pass arguments to an async fnc
    // they have their own stack created
    // but they can access variables in parent scopes
    async function asyncFnc3(){
        print("arg:"+a);
    }
}