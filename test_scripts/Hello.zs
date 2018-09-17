class Hello(){

    var helloMessage = "hello world!";

    sayHello();

    function setHelloMessage(message){
        helloMessage = message;
    }

    function getHelloMessage(){
        return helloMessage;
    }

    function sayHello(){
        print(helloMessage);
    }

    function closureTest(){
        var x = 100;
        return ()=>{
            print("variable in my parent scope is :"+x);
        };
    }

    function fncRefTest(fncRef){
        fncRef("hi");
    }

}