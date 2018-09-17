class ClassInstanceTest()
{
    print("************ STATIC VARIABLES TEST **********************");
    print("static varibale in StaticVariables.staticVariable is :"+StaticVariables.staticVariable);
    print("make it 100");
    StaticVariables.staticVariable = 100;
    print("static varibale in StaticVariables.staticVariable is :"+StaticVariables.staticVariable);
    print("************ STATIC METHOD TEST **********************");
    StaticVariables.osman();
    print("************ CLASS INSTANCE TEST **********************");
    print("creating an instance of Hello class...");
    var helloObject = new Hello();
    var message = helloObject.helloMessage;
    print("hello object's message is " + message);
    print("now set it to osman...");
    helloObject.helloMessage = "osman is awesome!";
    print("hello object's message is " + helloObject.helloMessage);
    print("now call sayHello on helloObject...");
    helloObject.sayHello();
    print("now set it via setter method");
    helloObject.setHelloMessage("osman again!");
    helloObject.sayHello();
    print("invoke closure on helloObject");
    helloObject.closureTest()();
    print("fncRef test");
    helloObject.fncRefTest(function(x){
        print(x);
    });
}