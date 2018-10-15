class BinaryTree(val){
    var data = val;
    var left;
    var right;

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
        print("add "+ data);
        return addRecursive(this,data);
    }

    function find(data){
        print("search "+ data);
        return findRecursive(this,data);
    }

    private function findRecursive(current,data){
        if (current == null){
            print("not found:"+data);
            return null;
        }
        if (data < current.data) {
            print("search left");
            findRecursive(current.left, data);
        } else if (data > current.data) {
            print("search right");
            findRecursive(current.right, data);
        } else {
            print("found:"+data);
            return current;
        }
    }

    private function addRecursive(current,data){
        if(current == null){
            return new BinaryTree(data);
        }
        if (data < current.data) {
            print("left");
            current.left = addRecursive(current.left, data);
        } else if (data > current.data) {
            print("right");
            current.right = addRecursive(current.right, data);
        } else {
            // value already exists
            return current;
        }

        return current;
    }
}