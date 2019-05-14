class Operators() {

    var strLength = "length";
    print("length of `test` is :" + "test".length());
    print("length of `test` is :" + "test"["length"]());
    print("length of `test` is :" + "test"[strLength]());

    print("length of `selami` is "+ "selami".length());

    var testObj = new Object();
    testObj.osman = "osman value";
    testObj[2] = "VELI";
    print("an arbitrary property (osman) that is defined on an object is :" + testObj.osman);
    print("an arbitrary numeric property (2) that is defined on an object is :" + testObj[2]);
    print("length of tesObj.osman is "+ testObj.osman.length());

    print("size of testObj is "+ testObj.size());

    for(var i = 0; i < testObj.keys().size();i++){
        print("keys[" + i + "] is :" + testObj.keys()[i]+" and the value is :" + testObj[testObj.keys()[i]]);
    }

    testObj["asd"] = "a new value. this must invalidate key cache";

    print("a new value is added...");

    for(var i = 0; i < testObj.keys().size();i++){
        print("keys[" + i + "] is :" + testObj.keys()[i]);
    }

    print("traversing values in a foreach loop...");

    for(var key in testObj){
        print(key + " = " + testObj[key]);
    }

    print("prefix and postfix operators test:");

    var a = 1;

    print(a++); //1
    a = 1;
    print(-a++); //-1
    a = 1;
    print(++a); //2
    a = 1;
    print(-++a); //-2
    a = 1;
    print(---a); //-2 //longest match first: -- -a

    print("prefix and postfix operators for floating point types test:");
    a = 1.5;
    print(a++); //1
    a = 1.5;
    print(-a++); //-1
    a = 1.5;
    print(++a); //2
    a = 1.5;
    print(-++a); //-2
    a = 1.5;
    print(---a); //-2 //longest match first: -- -a

    print("1 and 0:"+ (1 and 0));
    print("1 and 1:"+ (1 and 1));

    print("1 or 0:"+ (1 or 0));
    print("1 or 1:"+ (1 or 1));

    for(var i = 1; i > 0 and i < 4;i++ ){
        print(i);
    }

    print("1 or 1 and 0:"+ (1 or 1 and 0));

}