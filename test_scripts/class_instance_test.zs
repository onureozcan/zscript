class ClassInstanceTest()
{
    print("************ STATIC VARIABLES TEST **********************");
    print("static varibale in StaticVariables.staticVariable is :"+StaticVariables.staticVariable);
    print("make it 100");
    StaticVariables.staticVariable = 100;
    print("static varibale in StaticVariables.staticVariable is :"+StaticVariables.staticVariable);
    print("************ STATIC METHOD TEST **********************");
    StaticVariables.osman();
    print("floor:" + Math.floor(5.25));
    print("round:" + Math.round(5.65));
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
    helloObject.fncRefTest(x => print(x));
    var a = "test";
    helloObject.fncRefTest(x => print(a));
    print("************ CATCH EXCEPTIONS INSIDE ANOTHER CLASSS TEST **********************");
    var throwTest = new ThrowTest();

    try {
        throwTest.throwTest2Inner();
    } catch(e){
        print("this time I caught it: "+e);
    }
    try {
        // must throw an exception since there is no property defined on selami
        helloObject.selami = 10;
    } catch(e){
        print("and this too "+e);
    }

    print("************ PRIVATE VARIABLES TEST **********************");

    var objectWithPrivateVariables = new PrivateVariablesTest();
    try {
        print(objectWithPrivateVariables.privateVariable);
    } catch(e) {
        print(e);
    }
}