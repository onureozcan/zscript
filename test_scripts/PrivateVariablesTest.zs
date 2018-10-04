class PrivateVriablesTest(){

    private var privateVariable;

    private function privateFnc(){
        print("i cant be called outside this class unless accessed by an accessor :(");
    }

    function getPrivateFnc(){
        return this.privateFnc;
    }

}