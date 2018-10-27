class JsonTest(){

    var obj = { "a":0,"b":{"c":0,"d":1}, "e":[1,2,3] };

    assert(obj.a == 0);
    assert(obj["a"] == 0);
    assert(obj.b.c == 0);
    assert(obj["b"]["d"] == 1);
    assert(obj.e[0] == 1);
    assert(obj["e"][1] == 2);

    var keys = obj.keys();
    assert(keys.size() == 3);

    var obj2 = {"098766543212343435":"asd"};
    assert(obj2.keys()[0].equals("098766543212343435"));

    var count = 0;
    for (var key in obj) {
        count++;
    }
    assert(count == 3);

}