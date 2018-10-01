class ThrowTest(){


    throwTest1();

    throwTest2();

    function throwTest2(){
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


    function throwTest1(){
       try {
               throw "what happened?";
           } catch(e) {
               print("test 1 is passed. message:"+e);
           }
        print("and something to print after...");
    }

}