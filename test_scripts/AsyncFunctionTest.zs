class AsyncFucntionTest(){

    asyncFnc();

    for(var i = 0;i<100;i++){
        print("main:"+i);
    }

    async function asyncFnc(){
        for(var i = 0;i<100;i++){
            print("async:"+i);
        }
    }

}