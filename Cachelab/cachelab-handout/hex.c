#include "stdio.h"
#include "stdlib.h"
int main(int argc,char* argv[]){
    printf("%ld\n",strtol(argv[1],NULL,16));
    return 0;
}

