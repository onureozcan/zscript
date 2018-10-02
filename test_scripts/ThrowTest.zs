class ThrowTest(){

    throwTest0();

    throwTest1();

    throwTest2();

    throwTest3();

    function throwTest0(){
        print("this tests runtime errors");
        try {
            10/0;
        }catch(e){
            print(e);
        }
    }

    function throwTest1(){
        print("this tests catching thrown errors");
        try {
            throw "what happened?";
        } catch(e) {
            print("test 1 is passed. message:"+e);
        }
        print("and something to print after...");
    }

    function throwTest2(){
        print("this tests catching thrown errors inside another function");
        try {
            throwTest2Inner();
        } catch(e){
            print("test 2 is passed. exception message:"+e);
        }
        print("and something to print after...");
    }

    function throwTest2Inner(){
           throw "something happened :(";
    }

    function throwTest3(){
        print("this tests finally block");
        try {
            return 0;
        } catch (e){
            print("something happened");
        } finally {
            print("finally");
        }
    }

}