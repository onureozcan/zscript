class Hello(){

    var hello_message = "hello world!";

    sayHello();

    function setHelloMessage(message){
        hello_message = message;
    }

    function sayHello(){
        print(hello_message);
    }

}