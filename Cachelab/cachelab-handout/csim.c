#include "cachelab.h"
#include "stdlib.h"
#include "stdio.h"
#include "getopt.h"
#include "string.h"

static int E=1;
static int b=4;
static int s=4;
char * infile=NULL;
FILE * f;
int hitNumber=0;
int missNumber=0;
int eviction=0;
typedef struct block {
    char valid; 
    long tag;
    int rank;
}Line;
int getSet(long address, int s,int b)
{
    // Make a mask of s bit all 1s
    int mask=1;
    int i=0;
    for(i=1;i<s;i++){
    	mask|=1<<i;
    }
    // Now shift address right to get rid of block bits
    address=address>>b;
    int S=address & mask; 
    return S;
}
int getTag(long address, int s, int b){
    int Tag=address >>(s+b);
    return Tag;
}
void handleOneAccess(Line * cache,long address){
    int tagN=getTag(address,s,b);
    int setN=getSet(address,s,b);
    Line * curSet=(Line*) cache+setN*E;
    // Scan the set to check for lowest rank line and also if there's a hit
    //printf("current block is %p",curSet); 
    int i=0;
     int lowestRank=0x7FFFFFFF;
    int highestRank=0;
    Line * curLine=NULL;
    Line * LRU=NULL;
    char hit=0x0;

    for(i=0;i<E;i++){
        curLine=(curSet +i);
	if(curLine->tag==tagN&&curLine->valid){
           hit=1; 
	    break;
	}
//	printf("rank is %d, lowestrank is %d",curLine->rank,lowestRank);
	if(curLine->rank<lowestRank){
 	    lowestRank=curLine->rank;
	    LRU=curLine;	
	}
	if(curLine->rank>highestRank){
 	    highestRank=curLine->rank;	
	}
    }
 //   printf("lowest rank is %d\n",lowestRank);
  //  printf("highest rank is %d\n",highestRank);
    if(!hit){
	// Not found, then update the least recent one
        LRU->rank=highestRank+1;
	LRU->tag=tagN;
	printf(" miss");	
	missNumber++;
	if(LRU->valid){
	    printf(" eviction");
	    eviction++;
	}
	LRU->valid=1;
    }else{
        printf(" hit"); 
	hitNumber++;
    }
}
void handleLine(char* line,Line * cache){
    if(line[0]=='I'){
        return; 
    }
    char * token;
    char * type;
    char * addtxt;
    char * size;
    type=strtok(line," ");
    token=strtok(NULL," ");
    addtxt=strtok(token,","); 
    size=strtok(NULL,","); 
    size=strtok(size,"\n");
    unsigned long result=0;
    result=strtol(addtxt,NULL,16);
    if(type[0]=='L'||type[0]=='S'){
	printf("L %s,%s",addtxt,size);    
	handleOneAccess(cache,result);		
	printf("\n");
    }else if(type[0]=='M'){
	printf("M %s,%s",addtxt,size);
	handleOneAccess(cache,result);
	handleOneAccess(cache,result);
	printf("\n");	
    }
//    if(strcmp(type,"L")==0){
//        printf("L %s,%s",addtxt,size);    
//	printf("cache address start at %p",cache);
//	handleOneAccess(cache,result);		
//	printf("the address is %lu \n",result);	
//    }else if(strcmp(type,"M")==0){
//        printf("M %s,%s",addtxt,size);
//	printf("cache address start at %p",cache);
//	handleOneAccess(cache,result);
//	handleOneAccess(cache,result); 
//    } 
}
int main(int argc, char *argv[])
{
    char c;
    while ((c=getopt(argc,argv,"hvs:E:b:t:"))!=-1){
        switch(c){
	case 's':
	    s=atoi(optarg);
	    break;
	case 'E':
	    E=atoi(optarg);
	    break;
	case 'b':
	    b=atoi(optarg);
	    break;
	case 't':
	    infile=optarg;
	    break;
	case 'h':
	default:
	    exit(1);
	}
    }   
    if(!(f=fopen(infile,"r"))){
	printf(" Error: Couldn't open %s\n",infile);
    }
    int S=1<<s;  
    Line * cache= (Line *)calloc(S*E,sizeof(Line));
    char input[255];
    while(fgets(input,255,f)){
   	handleLine(input,cache); 
    } 
    free(cache);
    printSummary(hitNumber, missNumber,eviction);
   // printf("hit:%d misses:%d evictions:%d",hitNumber,missNumber,eviction);
    return 0;
}
