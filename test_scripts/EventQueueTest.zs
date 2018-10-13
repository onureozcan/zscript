import "../test_scripts/primetest" as Primetest

class EventQueueTest(){

    enqueue(function(){
        new Primetest();
        exit();
    });

}