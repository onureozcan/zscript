class Operators() {

    var strLength = "length";
    print("length of `test` is :" + "test".length());
    print("length of `test` is :" + "test"["length"]());
    print("length of `test` is :" + "test"[strLength]());

    print("length of `selami` is "+ "selami".length());

    var testObj = new Object();
    testObj.osman = "osman";
    testObj[2] = "VELI";
    print("an arbitrary property (osman) that is defined on an object is :" + testObj.osman);
    print("an arbitrary numeric property (2) that is defined on an object is :" + testObj[2]);
    print("length of tesObj.osman is "+ testObj.osman.length());

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

}