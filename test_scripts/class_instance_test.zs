class ClassInstanceTest()
{
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
}