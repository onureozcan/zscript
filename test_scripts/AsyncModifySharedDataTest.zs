class AsyncModifySharedDataTest(){

    synchronized var osman = 0;
    synchronized var opCount = 0;
    synchronized var limit = 10000;

    // any of these cross thread accesses should not corrupt each other's data
    modifyOsmanAsync(1);
    modifyOsmanAsync("asd");
    modifyOsmanAsync(()=>{});
    modifyOsman({});

    async modifyOsmanAsync(val){
        while(opCount < limit) {
            osman = val;
            print("osman:"+osman);
            opCount ++;
            print("op count :"+opCount);
        }
        print("finished");
    }

    modifyOsman(val){
        while(opCount < limit) {
            osman = val;
            print("osman:"+osman);
            opCount ++;
            print("op count :"+opCount);
        }
        print("finished");
    }
}