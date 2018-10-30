class Recursive()
{

    var results = [
        0,
        1,
        1,
        2,
        3,
        5,
        8,
        13,
        21,
        34,
        55,
        89,
        144,
        233,
        377,
        610,
        987,
        1597,
        2584,
        4181,
        6765,
        10946,
        17711,
        28657,
        46368,
        75025
    ];

    function fibonacci(num) {
        if (num == 0) return 0;
        if (num <= 1) return 1;
        return fibonacci(num - 1) + fibonacci(num - 2);
    }

    for (var i = 0; i < 26; i++) {
        var fib = fibonacci(i);
        assert(fib == results[i]);
        print("fib(" + i + "):" + fib);
    }

}