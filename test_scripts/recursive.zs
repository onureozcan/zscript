class Recursive(){

    function fibonacci(num) {
        if (num <= 1) return 1;
        return fibonacci(num - 1) + fibonacci(num - 2);
    }

    for(var i = 0; i < 26;i++){
        print("fib("+i+"):"+fibonacci(i));
    }

}