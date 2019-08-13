#include <sys/shm.h>
#include <sys/msg.h>
#include <stdio.h>
#include <stdlib.h>
#include "include.h"

typedef struct _thread_info{
    int client_id;
    int files_pending;
    char *input_buffer;
    char *output_buffer;
    int input_size;
    int *output_size;

    char ***input_buffer_arr;
    char ***output_buffer_arr;
    char ***processed_memory;
    char ***output_mem_addr;
    int **input_size_arr;
    int **output_size_arr;

    int is_sync;
    
} thread_info;

int debug = 1;

void qos_synchronised_compress(char*** buffer, int** input_size, char*** opbuffer, int** output_size, int client_id, int files_pending){
    

    while(1){
        printf("Started synchronised compress with qos, client id: %d  files_pending %d\n", client_id, files_pending);
        

        if(files_pending > 0){

            while(curr_client_qos != client_id){
                int T=100;
                while(T--);
            }
            req_producer_islocked = 1;

            printf("request producer locked \n");

            

            char * start = req_prod;
        
            
            char word[] = "client";
            printf("Client id: %d Client writing started: %s \n", client_id, word);
            for(int i = 0 ; i < 6; i++){
                *start = word[i];
                start++;

            }

            if(debug == 1){
                printf("Client id : %d client written\n", client_id);
            }


            int j, n_bits = 4;
            int temp_client_id = client_id;
            for(j = 0; j < n_bits && temp_client_id > 0; j++){
                *(start + n_bits - j -1 ) = ('0' + temp_client_id%10);
                temp_client_id = (int) (temp_client_id/10); 
            }
            for(int i = n_bits - j - 1; i >= 0 ; i--){
                *(start + i) = '0';
            }

            start = start + n_bits;

            //Save input buffer size in shared memory

            char word_[] = "memory";
            for(int i = 0 ; i < 6; i++){
                *start = word_[i];
                start++;
            }

            n_bits = 8;
            j=0;
            int temp_input_size = **input_size;
            for(j = 0; j < n_bits && temp_input_size > 0; j++){
                *(start + n_bits - j -1 ) = ('0' + temp_input_size%10);
                temp_input_size = (int) (temp_input_size/10); 
            }

            for(int i = n_bits - j - 1; i >= 0 ; i--){
                *(start + i) = '0';
            }
            start = start + n_bits;


            if(debug == 1){
                printf("Client id : %d memory written\n", client_id);
            }

            // Save the buffer in the shared memory
            for(int i = 0; i < **input_size; i++){
                *start = *((**buffer) + i);
                start++;
            }

            if(debug == 1){
                printf("Client id : %d Write complete\n", client_id);
            }

            //Update the request producer
            req_prod = start;

            (*buffer)++;
            (*input_size)++;
            printf("Input size is now %d \n", **input_size);
            

            if(debug == 1){
                printf("Client id : %d Lock free\n", client_id);
            }

            //Free the lock
            curr_client_qos = (curr_client_qos + 1) % num_clients;
            files_pending--;
            req_producer_islocked = 0;
        }
        else{
            while(curr_client_qos != client_id){
                int T=100;
                while(T--);
            }
            curr_client_qos = (curr_client_qos + 1) % num_clients;
            return;

        }


        
        while(current_response_thread != client_id){        
            int T=100;
            while(T--);
        }     
        res_consumer_islocked = 1;
        char* start = res_cons;
        
        if(debug == 1){
            printf("Response consumer in client %s \n", start);
        }
        // Move forward 6 steps | corresponding to client
        char word2[] = "server";
        for(int i=0; i<6; i++){
            if(*start == word2[i]){
                start++;
            }
            else{
                printf("*** %c, %c \n",*start, word2[i]);
                printf("Got error reading from shared memory while reading server name\n");
            }
        }

        int clnt_id = 0;
        int n_bits = 4;
        for(int i=0; i<n_bits;i++){
            clnt_id = clnt_id * 10 + (int)(*start - '0');
            start++;
        }

        char word1[] = "memory";
        for(int i=0; i<6; i++){

            if(*start == word1[i]){
                start++;
            }
            else{
                printf("Got error reading from shared memory while reading memory name \n");
            }
        }


        char blk_size[8];
        n_bits = 8;
        for(int i=0; i<n_bits;i++){
            blk_size[i] = *start;
            start++;
        }
        int mem_blk_size = atoi(blk_size);

        printf("@Mem size: %d \n", mem_blk_size);

        (*opbuffer)[0] = (char *)malloc(mem_blk_size * sizeof(char));
        // printf("Out before: %d \n",output_size[0]);
        // output_size[0] = 5;
        // printf("Out after: %d",output_size[0]);
        
        // output_size++;
        **output_size = mem_blk_size;
        printf("Assigned output size\n");
                
        for(int i=0; i<mem_blk_size; i++){
            (*opbuffer)[0][i] = *start;
            start++;
        }

        (*opbuffer)++;
        (*output_size)++;
        
        if(num_files > 0){
            num_files--;
            printf("Client id: %d Sent output buffers to app: %s \n", client_id, (*opbuffer)[-1]);

        }

        
        res_cons = start;

        processed_file_counter_islocked = 0;
        pointer_array_islocked = 0;
        res_consumer_islocked = 0;
                
    

    }
}



void synchronised_compress(char *buffer, int input_size, char* opbuffer, int * output_size, int client_id){
    

    while(1){
        printf("Started synchronised compress, client id: %d  lock: %d\n", client_id, req_producer_islocked);

        char * start = req_prod;
            
        while(req_producer_islocked == 1){

            //printf("sleeping client_id : %d \n",client_id);
    
            int T=10000;
            while(T--);
        }

        // if(req_producer_islocked == 0){
        // printf("~~~~~~req_producer: %d \n", req_producer_islocked);
        req_producer_islocked = 1;
        printf("request producer locked \n");
        
        //Lock is available. Start writing from req_producer
        
        
        //Save client ID in shared memory
        
        
        char word[] = "client";
        if(debug == 1){
            printf("Client %d writing started: %s \n", client_id, word);
        }
        for(int i = 0 ; i < 6; i++){
            *start = word[i];
            start++;

        }

        if(debug == 1){
            printf("Client id : %d client written\n", client_id);
        }


        int j, n_bits = 4;
        int temp_client_id = client_id;
        for(j = 0; j < n_bits && temp_client_id > 0; j++){
            *(start + n_bits - j -1 ) = ('0' + temp_client_id%10);
            temp_client_id = (int) (temp_client_id/10); 
        }
        for(int i = n_bits - j - 1; i >= 0 ; i--){
            *(start + i) = '0';
        }

        start = start + n_bits;

        //Save input buffer size in shared memory

        char word_[] = "memory";
        for(int i = 0 ; i < 6; i++){
            *start = word_[i];
            start++;
        }

        n_bits = 8;
        j=0;
        int temp_input_size = input_size;
        for(j = 0; j < n_bits && temp_input_size > 0; j++){
            *(start + n_bits - j -1 ) = ('0' + temp_input_size%10);
            temp_input_size = (int) (temp_input_size/10); 
        }

        for(int i = n_bits - j - 1; i >= 0 ; i--){
            *(start + i) = '0';
        }
        start = start + n_bits;


        if(debug == 1){
            printf("Client id : %d memory written\n", client_id);
        }

        printf("Input size: %d", input_size);

        // Save the buffer in the shared memory
        for(int i = 0; i < input_size; i++ ){
            *start = buffer[i];
            start++;
        }

        if(debug == 1){
            printf("Client id : %d Write complete\n", client_id);
        }

        //Update the request producer
        req_prod = start;
        if(debug == 1){
            printf("Client id : %d Lock free\n", client_id);
        }

        //Free the lock
        req_producer_islocked = 0;

        
        // } //#Removed if
        while(current_response_thread != client_id || res_consumer_islocked == 1){        
            int T=100;
            while(T--);
        }     
        res_consumer_islocked = 1;
        start = res_cons;

        
        if(debug == 1){
            printf("Response consumer in client %s \n", start);
        }
        // Move forward 6 steps | corresponding to client
        

        char word3[] = "server";
        for(int i=0; i<6; i++){
            if(*start == word3[i]){
                start++;
            }
            else{
                printf("*** %c, %c \n",*start, word3[i]);
                printf("Got error reading from shared memory while reading server name\n");
            }
        }

        int clnt_id = 0;
        n_bits = 4;
        for(int i=0; i<n_bits;i++){
            clnt_id = clnt_id * 10 + (int)(*start - '0');
            start++;
        }
        printf("Client id at the end: %d \n", clnt_id);
        // int clnt_id = atoi(_clnt_id);

        char word1[] = "memory";
        for(int i=0; i<6; i++){

            printf("#Memory characters: %c %c \n",*start, word1[i]);
            if(*start == word1[i]){
                start++;
            }
            else{
                printf("Got error reading from shared memory while reading memory name \n");
            }
        }

        char blk_size[8];
        n_bits = 8;
        for(int i=0; i<n_bits;i++){
            blk_size[i] = *start;
            start++;
        }
        int mem_blk_size = atoi(blk_size);

        printf("Mem size: %d %c\n", mem_blk_size, *start);
        
        // opbuffer = (char*)malloc(mem_blk_size * sizeof(char));
        // opbuffer = start;
        printf("%d\n", mem_blk_size);
        output_size = (int*)malloc(sizeof(int));
        *output_size = mem_blk_size;
        for(int i=0; i<mem_blk_size; i++){
            printf("%c \n",*start);
            opbuffer[i] = *start;
            start++;
        }
        printf("************Output in client: %s \n", opbuffer);

        res_cons = start;
        printf("Client id: %d  exited. Current response thread: %d \n", client_id, current_response_thread);
        res_consumer_islocked = 0;
        
        return;
    

    }
}

void asynchronised_compress(char*** buffer, int** input_size, char*** opbuffer, int** output_size, char*** pointer_array, int client_id, char*** processed_memory){

   
    
    printf("Started asynchronised compress for Client id: %d Client queue: %d \n", client_id, client_message_queue);
    char *response = (char*) malloc(5000*sizeof(char));
    while(1){

        if(num_files == 0){
            printf("Client id %d exited \n", client_id);
            return;
        }
        
        if (msgrcv (client_message_queue, &response, sizeof(response), 0, IPC_NOWAIT) != -1){
            // printf("YYYYYYYYYYYYYYYYYYYYYYYYYYYYYY \n");
            printf("Client id: %d Message received from : \n", client_id);

            while(pointer_array_islocked == 1){
                int T=100;
                while(T--);
            }

            pointer_array_islocked = 1;
            
            **pointer_array = response;
            
            (*pointer_array)++;
            processed_file_counter--;

            pointer_array_islocked = 0;
        }

        
        if(req_producer_islocked == 0 && **input_size != -1){
            req_producer_islocked = 1;
            printf("Input size in buffer: %d \n", **input_size);
            printf("Client id: %d request producer locked \n", client_id);

            char * start = req_prod;
            
            //Lock is available. Start writing from req_producer
            //Save client ID in shared memory
            
            char word[] = "client";
            printf("Client id: %d Client writing started: %s \n", client_id, word);
            for(int i = 0 ; i < 6; i++){
                *start = word[i];
                start++;

            }

            if(debug == 1){
                printf("Client id : %d client written\n", client_id);
            }


            int j, n_bits = 4;
            int temp_client_id = client_id;
            for(j = 0; j < n_bits && temp_client_id > 0; j++){
                *(start + n_bits - j -1 ) = ('0' + temp_client_id%10);
                temp_client_id = (int) (temp_client_id/10); 
            }
            for(int i = n_bits - j - 1; i >= 0 ; i--){
                *(start + i) = '0';
            }

            start = start + n_bits;

            //Save input buffer size in shared memory

            char word_[] = "memory";
            for(int i = 0 ; i < 6; i++){
                *start = word_[i];
                start++;
            }

            n_bits = 8;
            j=0;
            int temp_input_size = **input_size;
            for(j = 0; j < n_bits && temp_input_size > 0; j++){
                *(start + n_bits - j -1 ) = ('0' + temp_input_size%10);
                temp_input_size = (int) (temp_input_size/10); 
            }

            for(int i = n_bits - j - 1; i >= 0 ; i--){
                *(start + i) = '0';
            }
            start = start + n_bits;


            if(debug == 1){
                printf("Client id : %d memory written\n", client_id);
            }

            

            // Save the buffer in the shared memory
            for(int i = 0; i < **input_size; i++){
                *start = *((**buffer) + i);
                start++;
            }

            if(debug == 1){
                printf("Client id : %d Write complete\n", client_id);
            }

            //Update the request producer
            req_prod = start;

            (*buffer)++;
            (*input_size)++;
            printf("Input size is now %d \n", **input_size);
            

            if(debug == 1){
                printf("Client id : %d Lock free\n", client_id);
            }

            //Free the lock
            req_producer_islocked = 0;
            
        
        }
    
        if(processed_file_counter_islocked == 0 && pointer_array_islocked == 0 && processed_file_counter != 0 && num_files > 0){          
            if(processed_file_counter!= 0){
                processed_file_counter_islocked = 1;
                pointer_array_islocked = 1;

                printf("Processed files: %d client: %d \n", processed_file_counter, client_id);
                if(num_files == 0){
                    res_consumer_islocked = 0;
                    processed_file_counter_islocked = 0;
                    pointer_array_islocked = 0;
                    return;
                }

                
                char* start = *((*pointer_array) + processed_file_counter);
                printf("Client id: %d Start: %s \n",client_id, start);
                // Move forward 6 steps | corresponding to client
                char word[] = "server";
                for(int i=0; i<6; i++){
                    if(*start == word[i]){
                        start++;
                    }
                    else{
                        printf("*** %c, %c \n",*start, word[i]);
                        printf("Got error reading from shared memory while reading server name\n");
                    }
                }

                int clnt_id = 0;
                int n_bits = 4;
                for(int i=0; i<n_bits;i++){
                    clnt_id = clnt_id * 10 + (int)(*start - '0');
                    start++;
                }

                char word1[] = "memory";
                for(int i=0; i<6; i++){

                    if(*start == word1[i]){
                        start++;
                    }
                    else{
                        printf("Got error reading from shared memory while reading memory name \n");
                    }
                }

                char blk_size[8];
                n_bits = 8;
                for(int i=0; i<n_bits;i++){
                    blk_size[i] = *start;
                    start++;
                }
                int mem_blk_size = atoi(blk_size);

                printf("@Mem size: %d \n", mem_blk_size);

                // opbuffer = &start[0];
                printf("start right now: %s \n", start);
                while(res_consumer_islocked == 1){
                    int T=100;
                    while(T--);
                }
                res_consumer_islocked = 1;

                (*opbuffer)[0] = (char *)malloc(mem_blk_size * sizeof(char));
                // printf("Out before: %d \n",output_size[0]);
                // output_size[0] = 5;
                // printf("Out after: %d",output_size[0]);
                
                // output_size++;
                **output_size = mem_blk_size;
                printf("Assigned output size\n");
                
                for(int i=0; i<mem_blk_size; i++){
                    (*opbuffer)[0][i] = *start;
                    start++;
                }

                (*opbuffer)++;
                (*output_size)++;
                processed_file_counter++;
                
                if(num_files > 0){
                    num_files--;
                    printf("Client id: %d Sent output buffers to app: %s \n", client_id, (*opbuffer)[-1]);

                }

                
                processed_file_counter_islocked = 0;
                pointer_array_islocked = 0;
                res_consumer_islocked = 0;
                

                if(num_files == 0){
                    printf("Client id %d exited \n", client_id);
                    return;
                }
                
                }
                
                
                


                
            
        }
    }

}


void *client_init(void *u_arg){
    
    #define ptr ((thread_info *)u_arg)
    if(num_files != 0){
        printf("client initialised %d  is_sync %d \n", ptr->client_id, ptr->is_sync);

        if(ptr->is_sync){
            if(is_qos == 0){
                synchronised_compress(ptr->input_buffer, ptr->input_size, ptr->output_buffer, ptr->output_size, ptr->client_id);
            }
            else{
                qos_synchronised_compress(ptr->input_buffer_arr, ptr->input_size_arr, ptr->output_buffer_arr, ptr->output_size_arr, ptr->client_id, ptr->files_pending);
            }    
            
        }

        else{

            asynchronised_compress(ptr->input_buffer_arr, ptr->input_size_arr, ptr->output_buffer_arr, ptr->output_size_arr, ptr->output_mem_addr, ptr->client_id, ptr->processed_memory);
        }
    }
}
