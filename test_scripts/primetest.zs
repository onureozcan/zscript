class Primetest() {

    var testLimit = 100000;
    var inlineTest = false;

    if (inlineTest) {
        for (var i = 0; i < testLimit; i = i ++) {
            var _isPrime = i != 1;
            for (var j = 2;j < i; j++) {
                if (i % j == 0) {
                    _isPrime = false;
                    break;
                }
            }
            print(i + " isPrime:" + _isPrime);
        }
        print("inline prime test is done");
     } else {
        for(var i= 0;i<testLimit;i++){
            print(i + " isPrime :" + isPrime(i));
        }
        print("prime test with function calls is done");
        function isPrime(num) {
            var i = 2;
            for (;i<num;i++) {
                if (num % i == 0){
                    return false;
                }
            }
            return num != 1;
        }
    }
}
