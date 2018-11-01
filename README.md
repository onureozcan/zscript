## zeroscript

It is a dynamically typed, object oriented scripting language written in c and c++.
The name does not mean that it includes no scripting part. Zero here states that it is in the 0th stage, has a long way to go.


**basic syntax**:

It resembles javascript a lot and yet has some different sides

- class and member declaration:

```
// classses are top level entities,
// meaning that everything must belong to a class.
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
    // writing `this` is not mandatory
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
 var obj2 = { "a":0, "c": "d" };

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

- declare private variables

```
// private variables cannot get accessed from outside this class
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


**ZeroscriptOs**

The reason behind writing this language is to use it in an another hobby operating system development project (ZeroscriptOS).
Zeroscript will work as a way to write user space applications for the os project without actually implementing a user space. The whole kernel will act as a Zeroscript interpreter.
Native bindings will displace system calls. Every process being implemented in a scripting language will provide isolation.

Although this model is questionable and performance of such an os would not be comparable to a "normal" one but as it is said before, it is a hobby project and every single line written is for pleasure.
After all no one cares if implementing things this way is the best or not for an os that no one will use :)

Zeroscript is initial step of writing ZeroscriptOS. However when it is done, Zeroscript will be ready to use not only under the operating system project that I have mentioned above, but also under the Desktop Linux environments and may be in Windows if pthread-related parts are converted to their Windows equivalents.

It is now too early to use it in real world, and there are tons of things to do. I have an untidy road map and implementing these steps one by one. Apart from having basics, the road map is as follows:

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

- [ ] jit compiler


**build from source**:

It is a C/C++ mixed project and the whole project is dependent to the ANTLR c++ runtime. I am pretty sure that there is a wiser way to use antlr with cmake but for now it requires header files and binaries to be in the default include path due to my poor build tool knowledge.

It is only tested on gcc 6 and 7. Also requires gcc's labels as values extension and uses posix threads so I am not sure about how it will behave under Windows.
Note that portability is not a concern for this project. At least for now.

**testing**:

Tests are intended to be more like "real world" problems rather than unit testing of each feature. There is no easy way to test features in an isolated way for me.
For example, I can not test correctness of math operations without relying on correctness of assert function.
For this reason, I write Zeroscript equivalents of well known algorithms, data structures and design patterns such as binary trees, recursive fibonacci etc and expect them to give correct results.

To run tests, build tester and run it in the project's root folder.

**run scripts**:

```
./zero <path to source file>
```

**compile scripts and generate .zar files**:

not yet.

**run zar files**:

not yet.

**contributions**:

any help will be appreciated. just make a pull request.
