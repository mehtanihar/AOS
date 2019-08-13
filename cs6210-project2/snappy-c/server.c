#include <stdio.h>
#include <stdlib.h>
#include "snappy.h"
#include "include.h"
#include <sys/msg.h>
#include <string.h>

void *server_init(){

    int shmid = shmget(shm_memory_key, shm_mem_size, 0666);
    char *my_shared_mem = (char*) shmat(shmid, (void*)0, 0); 

    if (shmid < 0) {
        printf("Unable to access shared memory in the server\n");
        exit(1);
    }
    int debug = 1;

    struct snappy_env env;
    snappy_init_env_sg(&env, true);

    while(1){
        

        
        if(debug == 1){
            printf("\nIn server\n");
        }
           
        while(req_cons == req_prod && num_files!=0){
            int T=100;
            while(T--);
        }

        if (num_files==0){
            printf("Server exited \n");
            break;
        }

            

        if(debug == 1){
            printf("Server can start processing requests\n");
        }

        // Move forward 6 steps | corresponding to client
        char *word = "client";
        char * start = req_cons;
        
        for(int i=0; i<6; i++){

            if(*start == word[i]){
                
                start++;
            }
            else{
                printf("Read character: %c Expected: %c \n", *start, word[i]);
                printf("Got error reading from shared memory while reading client name \n");
                exit(1);
            }
        }

        if(debug == 1){
            printf("Server read client ID!\n");
        }

        
        // char _clnt_id[4];
        int n_bits = 4;
        int clnt_id = 0;
        for(int i=0; i<n_bits;i++){
            clnt_id = clnt_id * 10 + (int)(*start - '0');
            start++;
        }

        // int clnt_id = atoi(_clnt_id);

        // printf("Writing client id, %s\n", _clnt_id);

        word = "memory";
        for(int i=0; i<6; i++){
            if(*start == word[i]){
                start++;
            }
            else{
                printf("Got error reading from shared memory while reading memory name  \n");
                exit(1);
            }
        }

        char blk_size[8];
        n_bits = 8;
        for(int i=0; i<n_bits;i++){
            blk_size[i] = *start;
            start++;
        }
        int mem_blk_size = atoi(blk_size);

        if(debug == 1){
            printf("Server read memory!\n");
        }

        char *inputbuffer = (char*) malloc(mem_blk_size*sizeof(char));
        for(int i=0; i<mem_blk_size; i++){
            inputbuffer[i] = *start;
            start++;
        }
        req_cons = start;


        if(debug == 1){
            printf("Server read input buffer!\n");
        }

        
        // char outbuffer[mem_blk_size];
        

        size_t _out_size;// = snappy_max_compressed_length(mem_blk_size);
        char *outbuffer = (char*) malloc(_out_size *sizeof(char));
        if(debug == 1){
            printf("Server will start compressing!\n");
        }
        printf("%s\n",inputbuffer);
        
        int err = snappy_compress(&env, inputbuffer, mem_blk_size, outbuffer, &_out_size);
        if(err!=0){
            printf("Error in compression \n");
            exit(0);
        }
        
        if(debug == 1){
            printf("Snappy done compression!\n");
        }
        int out_size = (int)_out_size;


        start = res_prod;


        word = "server";
        for(int i = 0 ; i < 6; i++){
            *start = word[i];
            start++;
        }
        if(debug == 1){
            printf("Writing word server\n");
        }
        int j;
        int temp_clnt_id = clnt_id;
        n_bits =4;
        for(j = 0; j < n_bits && temp_clnt_id > 0; j++){
            *(start + n_bits - j -1 ) = ('0' + temp_clnt_id%10);
            temp_clnt_id = (int) (temp_clnt_id/10); 
        }
        if(debug == 1){
            printf("Writing client id, %d\n", clnt_id);
        }
        for(int i = n_bits - j - 1; i >= 0 ; i--){
            *(start + i) = '0';
        }
        start = start + n_bits;

        //Save output buffer size in shared memory

        word = "memory";
        for(int i = 0 ; i < 6; i++){
            *start = word[i];
            start++;
        }

        if(debug == 1){
            printf("Writing word memory, %d\n", clnt_id);
        }


        n_bits = 8;

        j=0;
        int temp_out_size = out_size;
        if(debug == 1){
            printf("Out size: %d \n", out_size);
        }


        for(j = 0; j < n_bits && temp_out_size > 0; j++){
            // printf()
            *(start + n_bits - j -1 ) = ('0' + temp_out_size%10);
            temp_out_size = (int) (temp_out_size/10); 
        }


        for(int i = n_bits - j - 1; i >= 0 ; i--){
            *(start + i) = '0';
        }


        

        start = start + n_bits;


        if(debug == 1){
            printf("Writing outbuffer,\n %s\n", outbuffer);
        }


        if(is_sync == 0){
            

            strncpy(start, outbuffer, out_size);

            start = start + out_size;
             
            if (msgsnd (client_message_queue, &res_prod, sizeof(char *), 0) == -1)
            {
                perror ("client: msgsnd");
                exit (1);
            }
            printf("Message sent \n");

            res_prod = start;



        }
        else{
            
            // Save the buffer in the shared memory
            for(int i = 0; i < out_size; i++ ){
                *start = outbuffer[i];
                start++;
            }
            //Update the response producer
            res_prod = start;
            
            while(res_consumer_islocked == 1){        
                int T=100;
                while(T--);
            }
             
            res_consumer_islocked = 1;
            start = res_cons;
            res_consumer_islocked = 0;

            if(debug == 1){
                printf("Reading server name in server: %s \n", start);
            }
             char *word = "server";
            for(int i=0; i<6; i++){
                if(*start == word[i]){
                    start++;
                }
                else{
                    printf("Got error reading from shared memory while reading server name \n");
                    exit(1);
                }
            }
            if(debug == 1){
                printf("Server name read, \n");
            } 

                        
            int n_bits = 4;
            clnt_id = 0;
            for(int i=0; i<n_bits;i++){
                clnt_id = 10*clnt_id + (int) (*start - '0');
                start++;
            }
            if(debug == 1){
                printf("Response set, wake client %d \n", clnt_id);
            }

            current_response_thread = clnt_id;
            
            
            
                    

        
        }
        
    }        

}