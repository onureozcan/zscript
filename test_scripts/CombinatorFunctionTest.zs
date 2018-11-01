class CombinatorFunctionTest(){

    assert(inSequence(
        (a)=> "arg was"+ a,
        ()=> "f"
    )().equals("arg wasf"));

    assert(flip(
            (a,b)=>(a+b)
    )("hello","world").equals("worldhello"));

    function inSequence(f,g){
        return ()=> f(g());
    }

    function flip(f){
        return (a,b)=>f(b,a);
    }

}