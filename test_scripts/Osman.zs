import "../test_scripts/BinaryTree" as BinaryTree

class Osman(){

   var root = null;

   print("put <number> to insert a node");
   print("get <number> to find a node");
   print("q to quit");

   while(true){

       var line = readln();

        if(line.equals("q")){
            exit(0);
        } else if (line.startsWith("put")){
            var num = number(line.substring(3));
            if(root == null){
                root = new BinaryTree(num);
            } else {
                root.add(num);
            }
        } else {
            var num = number(line.substring(3));
            if(root!=null){
                root.find(num);
            } else {
                throw "tried to find before adding any node";
            }
        }

   }

}