class OperatorsTest(){

    print("basic math operations test:");

    assert(1 + 2 == 3);
    assert(1 + 0 == 1);
    assert(-1 + -2 == -3);
    assert(-1 + 0 == -1);
    assert(1.0 + 2 == 3);
    assert(1.5 + 2.5 == 4);
    assert(2.5 + -1.5 == 1);
    assert(1 - 2 == -1);
    assert(-1 - 2 == -3);

    assert(("a" + 1).equals("a1"));
    assert(("a" + "b").equals("ab"));

    assert(1 * 2 == 2);
    assert(1 * 0 == 0);
    assert(-1 * -2 == 2);
    assert(-1 * 0 == 0);
    assert(1.0 * 2 == 2);
    assert(1.5 * 2.5 == 3.75);
    assert(2.5 * -1.5 == -3.75);

    assert(1 / 2 == 0.5);
    assert(-1 / 2 == -0.5);
    assert(1.0 / 2 == 0.5);
    assert(1.5 / 2.5 == 0.6);
    assert(1.5 / -2.5 == -0.6);

    print("prefix and postfix operators test:");

    var a = 1;
    var b = ++a;
    assert(b == 2);
    assert(a == 2);
    b = a++;
    assert(b == 2);
    assert(a == 3);
    assert(++a == 4);
    assert(a++ == 4);
    assert(a == 5);

    a = 1;
    var b = --a;
    assert(b == 0);
    assert(a == 0);
    b = a--;
    assert(b == 0);
    assert(a == -1);
    assert(--a == -2);
    assert(a-- == -2);
    assert(a == -3);

    print("boolean operators test:");
    assert(0 and 1 == 0);
    assert(0 and 0 == 0);
    assert(1 and 0 == 0);
    assert(1 and 1 == 1);

    assert(0 or 1 == 1);
    assert(0 or 0 == 0);
    assert(1 or 0 == 1);
    assert(1 or 1 == 1);

    print("boolean operators short circuit test:");
    assert(0 and (()=>{throw "not good";})() == 0);
    assert(1 or (()=>{throw "not good";})() == 1);


}