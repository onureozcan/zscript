import "../test_scripts/Math" as Math

class AsyncFucntionTest(){

    asyncFnc();
    synchronousFnc();
    asyncFnc2();
    asynFncWithThrow();

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

    async function asyncFnc2(){
        print("rounded:"+Math.round(11.2));
    }
}