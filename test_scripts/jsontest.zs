class JSONTest(){

    var testObj = {
        "a":"a1"
    };

    var testObj2 = {
        "nested":testObj
    };

    print("********keys of testObj*******");

    for(var key in testObj){
        print(key + ":" + testObj[key]);
    }

    print("********keys of testObj2.nested*******");

    for(var key in testObj2.nested){
        print(key + ":" + testObj2.nested[key]);
    }

    print("********keys of testObj2*******");

    for(var key in testObj2){
        print(key + ":" + testObj2[key]);
    }

    var arrayObj = [1,2,3,4];

    print("********keys of arrayObj*******");

    for(var key in arrayObj){
        print(key + ":" + arrayObj[key]);
    }

}