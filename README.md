## zeroscript

a toy programming language for hobby purposes. do not take it seriously.
it will later be used to provide userspace to zeroscriptOs project.

**roadmap:**

- [X] json

- [X] foreach loop 

- [X] lambda expressions 

- [X] object creation from class 

- [X] static objects and functions 

- [X] import classes from different folders

- [X] throw errors

- [X] try/catch

- [X] access modifiers

- [ ] class inheritance

- [X] garbage collector

- [X] asynchronous functions

- [ ] packaging of compiled bytecode into .zar files

- [ ] standard library implementation

- [ ] semantic analyzer

- [ ] optimizer 

- [ ] intelliJ plugin

**basic syntax**:

It resembles javascript a lot and yet has some different sides

- class and member declaration:

```
class Hello(/*constructor arguments*/){

    //member variable
    var helloMessage = "hello world!";

    // function call
    sayHello();

    // set method for helloMessage
    function setHelloMessage(message){

        // notice that this refers to the class itself
        this.helloMessage = message;

    }

    function getHelloMessage(){
        return helloMessage;
    }

}
```

- static functions and variables:

```
class Math(){

    static var someStaticVariable = 100;

    static function floor(n){
        return n - (n % 1);
    }

}
```

- lambda expressions and function references:

```
     helloObject.fncRefTest(function(x){
            print(x);
     });

     helloObject.fncRefTest((x)=>{
                 print(x);
     });

     helloObject.fncRefTest(x=>print(x));


```

- object and array creation

```
 // instance of a class
 var helloObject = new Hello();

 // json
 var obj = Object();
 var obj2 = {};

 //array
 var arr = [];

```

- import class files from path

```
import "../test_scripts/Math" as Math
import "../test_scripts/Hello" as Hello

class ImportTest(){

    static var a = Math.floor(3.7);
    print(Math.round(2.34));
    var hello = new Hello();

}

```

- define private variables

```
// this cannot get accessed from outside this class
class PrivateVriablesTest(){

    private var privateVariable;

    private function privateFnc(){
        print("i cant be called outside this class unless accessed by an accessor :(");
    }

    function getPrivateFnc(){
        return this.privateFnc;
    }

}
```

- asynchronous functions

```
    // async functions return immediatelly and run on a separate thread
    async function asyncFnc(){
        new Primetest();
    }

    // you can pass arguments
    asyncCount(()=>{
        print("finished async 3");
    });

     // can capture arguments in an async fnc
    async function asyncCount(arg){
        for(var i = 0;i<1000;i++){
            print(i);
        }
        arg();
    }
```

**run as script**:

```
./zrun <path to source file> [class path]
```
