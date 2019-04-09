#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stddef.h>
#include <ctype.h>
//By Ryan Corbin and  Michael Deatley

//prototypes
void get_maxs();
int request_resources(int customer_num, int request[]);
void release_resources(int customer_num, int release[]);
void display();


//Resource Structures
#define NUMBER_OF_CUSTOMERS 5
#define NUMBER_OF_RESOURCES 4
int available[NUMBER_OF_RESOURCES];
int maximum[NUMBER_OF_CUSTOMERS][NUMBER_OF_RESOURCES];
int allocation[NUMBER_OF_CUSTOMERS][NUMBER_OF_RESOURCES];
int need[NUMBER_OF_CUSTOMERS][NUMBER_OF_RESOURCES];


//Main
int main(int argc, char *argv[]) {
    for(int i = 0; i < NUMBER_OF_RESOURCES; i++){
        if(i+1 < argc){
            available[i] = atoi(strsep(&argv[i+1],""));
        }else{
            available[i] = 0;
        }
    }

    get_maxs();

    //user inputs available
    printf("inputs:\nRQ:request resources\nRL:release resources\n*:Display\nQ:Quit\n");

    //get input loop
    while(1){
        char input[20];
        fgets(input,20,stdin);//get user input
        strtok(input, "\n");//remove the \n at the end

        int CN = 0, req[NUMBER_OF_RESOURCES] = {0};//CN = Customer num; Req = arg2 for RQ and RL; both default to 0

        //get op from input
        char *op, *tok = strtok(input," ");//separate tokens by space
        if(tok != NULL){
            op = strsep(&tok,"");
        }

        //strlwr(op)
        for(int i = 0; i < strlen(op); i++){
            op[i] = tolower(op[i]);
        }

        //get customer number from input
        tok = strtok(NULL," ");
        if(tok != NULL){
            CN = atoi(strsep(&tok,""));
        }

        //get req from last 4 inputs
        int i;
        for( i = 0; i < NUMBER_OF_RESOURCES; i++){
            tok = strtok(NULL," ");
            if(tok != NULL){
                req[i] = atoi(strsep(&tok,""));
            }
        }

        //commands read by strcmp
        if(!strcmp(op,"*")){
            display();
        }else if(!strcmp(op,"rq")){
            if(!request_resources(CN,req)){printf("Request Was Successful\n");}else{printf("Request Was Unsuccessful\n");}
        }else if(!strcmp(op,"rl")){
            release_resources(CN,req);
            printf("Resources were Released\n");
        }
        //need an exit strategy
        else if(!(strcmp(op,"q"))){
            break;
        }
    }
    return (EXIT_SUCCESS);
}


//gets maximums from file; is now a subfunction because it was cluttering up the variables in the debugger
void get_maxs(){
    //read from txt file, to get maximums
    FILE *in;
    in = fopen("input.txt","r");
    char job[100];
    int i = 0;
    while (fgets(job,100,in) != NULL && i < NUMBER_OF_CUSTOMERS) {
        char *temp;
        temp = strdup(job);
        int j;
        for(j = 0; j < NUMBER_OF_RESOURCES; j++){
            maximum[i][j] = atoi(strsep(&temp,","));//
            allocation[i][j] = 0;//resources start at 0, and are allocated by user
            need[i][j] = maximum[i][j];//need = maximum - 0(allocation)
        }
        free(temp);
        i++;
    }
    fclose(in);//done with input file
}


//Test if currently in a safe state; 1 if safe, 0 if not
int test_safe(){
    int fin[NUMBER_OF_CUSTOMERS] = {0};  //track what can finish
    int Tavailable[NUMBER_OF_RESOURCES]; //temporary available to avoid changes while testing
    int i;
    for(i= 0; i < NUMBER_OF_RESOURCES; i++){
        Tavailable[i] = available[i];
    }
    i = 0;
    while(i < NUMBER_OF_CUSTOMERS){
        i++;
        int locked = 1;//uhhh... true
        int j;
        for(j = 0; j < NUMBER_OF_CUSTOMERS; j++){
            if(fin[j] == 0){
                int check = 0;
                int k;
                for(k = 0; k < NUMBER_OF_RESOURCES; k++){
                    if(Tavailable[k] >= need[j][k]){
                        check++;
                    }
                }
                if(check == NUMBER_OF_RESOURCES){
                        int m;
                    for (m = 0 ; m < NUMBER_OF_RESOURCES ; m++){ //skipping l because it looks the same as a 1 in my IDE
                        Tavailable[m] = allocation[j][m] + Tavailable[m];
                    }
                    fin[j] = 1;
                    locked = 0;//not locked
                }
            }
            if(locked==0){
                break;
            }
        }
        if(locked == 1){
            return 0;
        }
    }
    return 1;
}


//request_resources: return 0 if succeed else return -1
int request_resources(int customer_num, int request[]){
    if(customer_num < 0 || customer_num >= NUMBER_OF_CUSTOMERS){
        return -1;
    }
    int i;
    for(i = 0; i < NUMBER_OF_RESOURCES; i++){
        if(request[i] > available[i]){
            return -1;
        }
        if(request[i] > need[customer_num][i]){
            return -1;
        }
    }
    for(i = 0; i < NUMBER_OF_RESOURCES; i++){
            available[i] = available[i] - request[i];
            allocation[customer_num][i] = allocation[customer_num][i] + request[i];
            need[customer_num][i] = need[customer_num][i] - request[i];
    }
    if (test_safe()){
        return 0;
    }else{

        for(i = 0; i < NUMBER_OF_RESOURCES; i++){
            allocation[customer_num][i] = allocation[customer_num][i] - request[i];
            need[customer_num][i] = need[customer_num][i] + request[i];
            available[i] = available[i] + request[i];
        }
        return -1;
    }
}


//Releases Resources; Can request more than max but will only release max
void release_resources(int customer_num, int release[]){
    int i;
    for(i = 0; i < NUMBER_OF_RESOURCES; i++){
        if(release[i] > allocation[customer_num][i]){
            release[i] = allocation[customer_num][i];
        }
        allocation[customer_num][i] = allocation[customer_num][i] - release[i];
        need[customer_num][i] = need[customer_num][i] + release[i];
        available[i] = available[i] + release[i];
    }
}

//Displays Current Resource Status
void display(){
    printf("\navailable :");
    int i;
    for(i = 0; i < NUMBER_OF_RESOURCES; i++){
            printf(" R%d:%d",i, available[i]);
    }printf(";\n");
    printf("maximum:\n");
    for(i = 0; i < NUMBER_OF_CUSTOMERS; i++){
        printf("Customer %d:",i);
        int j;
        for(j = 0; j < NUMBER_OF_RESOURCES; j++){
            printf(" R%d:%d",j, maximum[i][j]);
        }
        printf(";\n");
    }
    printf("allocation:\n");
    for(i = 0; i < NUMBER_OF_CUSTOMERS; i++){
        printf("Customer %d:",i);
        int j;
        for(j = 0; j < NUMBER_OF_RESOURCES; j++){
            printf(" R%d:%d",j, allocation[i][j]);
        }
        printf(";\n");
    }
    printf("need:\n");
    for(i = 0; i < NUMBER_OF_CUSTOMERS; i++){
        printf("Customer %d:",i);
        int j;
        for(j = 0; j < NUMBER_OF_RESOURCES; j++){
            printf(" R%d:%d",j, need[i][j]);
        }
        printf(";\n");
    }
}
