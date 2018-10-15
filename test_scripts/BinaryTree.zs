class BinaryTree(val){

    private var data = val;
    private var left;
    private var right;

    function getData(){
        return this.data;
    }

    function getLeft(){
        return this.left;
    }

    function getRight(){
        return this.right;
    }

    function add(data){
        return addRecursive(this,data);
    }

    private function addRecursive(current,data){
        if(current == null){
            print("created new");
            return new BinaryTree(data);
        }
        if (data < current.data) {
            print("added to left");
            current.left = addRecursive(current.left, data);
        } else if (data > current.data) {
            print("added to right");
            current.right = addRecursive(current.right, data);
        } else {
            // value already exists
            return current;
        }
        return current;
    }
}