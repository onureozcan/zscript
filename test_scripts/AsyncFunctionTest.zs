import "../test_scripts/Math" as Math
import "../test_scripts/primetest" as Primetest

class AsyncFucntionTest(){

    asyncFnc();
    synchronousFnc();
    asyncFnc2();
    asynFncWithThrow();

    // you can pass callback arguments
    asyncFnc3(()=>{
        print("finished async 3");
    });

    function synchronousFnc(){
        //these primetests both require gc and yet they should not corrupt each other's data
        new Primetest();
    }

    async function asyncFnc(){
        new Primetest();
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

    // can pass arguments to an async fnc
    async function asyncFnc3(arg){
        for(var i = 0;i<1000;i++){
            print(i);
            //this will trigger gc also
            var a = {};
        }
        arg();
    }
}