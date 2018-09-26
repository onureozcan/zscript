class Hello(){

    var helloMessage = "hello world!";

    sayHello();

    function setHelloMessage(message){
        this.helloMessage = message;
    }

    function getHelloMessage(){
        return this.helloMessage;
    }

    function sayHello(){
        print(this.helloMessage);
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