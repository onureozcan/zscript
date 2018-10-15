class PrivateVariablesTest(){

    private var privateVariable;

    private function privateFnc(){
        print("i cant be called outside this class unless accessed by an accessor :(");
    }

    function accessPrivateVariableFromTheSameClass(){
        var instanceOfTheSameClass = new PrivateVariablesTest();
        instanceOfTheSameClass.privateFnc();
    }

    function getPrivateFnc(){
        return this.privateFnc;
    }

}