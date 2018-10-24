class Math(){

    static function floor(n){
        return n - (n % 1);
    }

    static function round(x){
        if (x < 0.0)
            return int(x - 0.5);
        else
            return int(x + 0.5);
    }

}