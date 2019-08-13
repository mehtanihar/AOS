#include <stdio.h>
#include <stdlib.h>
#include <sys/msg.h>
#include <sys/stat.h>
#include <sys/shm.h>
#include <pthread.h>
#include <string.h>

#include "include.h"
#include "client.h"
#include "server.h"
#include <math.h>
// #define shm_size 2048

typedef struct _thread_info thread_info;

int main(int argc, char *argv[])
{
    

    if(argc == 1){
        printf("No file path provided");
        return -1;
    }


    
    int num_segments, segment_size, n_threads;

    curr_client_qos = 0;
    int debug = 1;
    is_sync = 1;
    is_qos = 1;
    int n_files = 0;
    n_threads = 5;
    int shm_size = 2048;
    char* dir_path = (char *) malloc(100 * sizeof(char));
    strcpy(dir_path , argv[1]);

    for(int i = 2; i < argc; i++){
        if(strcmp(argv[i],"num_segments") == 0){
            i++;
            num_segments = atoi(argv[i]);
        }
        else if(strcmp(argv[i],"segment_size") == 0){
            i++;
            segment_size = atoi(argv[i]);
        }
        else if(strcmp(argv[i],"num_threads") == 0){
            i++;
            n_threads = atoi(argv[i]);
        }

        else if(strcmp(argv[i],"is_sync") == 0){
            i++;
            is_sync = atoi(argv[i]);
        }
        else if(strcmp(argv[i],"is_qos") == 0){
            i++;
            is_qos = atoi(argv[i]);
        }
        else{
            n_files++;
        }

    }

    num_clients = n_threads;
    if(n_files == 0){
        printf("No file path provided");
        return -1;
    }
    
    if(debug == 1){

        printf("Directory path: %s \n", dir_path);
        printf("Number of segments %d \n", num_segments);
        printf("Size of segments %d \n", segment_size);
        printf("Number of threads: %d \n", n_threads);
        printf("Sync: %d \n", is_sync);
        printf("Num files: %d \n", n_files);
        printf("\n");
    }
    
    

    num_files = n_files;
    shm_size = num_segments * segment_size;

    if(debug == 1){
        printf("Initialize shared memory\n");
    }
    

    int shm_key = 11;
    shm_mem_size = shm_size;
    shm_memory_key = ftok("shared_memory", shm_key);
    int shmid = shmget(shm_memory_key, shm_size, 0666 | IPC_CREAT);

    if (shmid < 0) {
        printf("Unable to allocate shared memory\n");
        exit(1);
    }
    char *my_sh_mem = (char *)shmat(shmid, (void *)0, 0);
    current_response_thread = -1;

    shmdt(my_sh_mem);
    shmctl(shmid,IPC_RMID,NULL); 
    shmid = shmget(shm_memory_key, shm_size, 0666 | IPC_CREAT);
    my_sh_mem = (char *)shmat(shmid, (void *)0, 0);
    
    //Give info about shared memory
    req_prod = my_sh_mem;
    req_cons = my_sh_mem;
    res_prod = my_sh_mem + (int)(shm_size / 2);
    res_cons = my_sh_mem + (int)(shm_size / 2);

    //Initialize producer and consumer locks
    req_producer_islocked = 0;
    //req_consumer_islocked = 0;

    //res_producer_islocked = 0;
    res_consumer_islocked = 0;
    
    
    if ((client_message_queue = msgget(IPC_PRIVATE, 0660)) == -1){
        perror ("msgget: myqid");
        exit (1);
    }

    if(debug == 1){
        printf("Creating server \n");
    }

    
    pthread_t server_thread;
    pthread_create(&server_thread, NULL, server_init, NULL);
    // server_init();
    
    if(debug == 1){
        printf("Server created! \n");
    }
    // Define pthread array
    int N;
    if(is_sync == 1)
        N = n_files;
    else
        N = n_threads;

    if(is_qos==1){
        N = n_threads;
    }
    
    pthread_t threads[N];
        

    thread_info arg[N];
    // thread_info arg_threads[N]; //being used for async
    
    thread_info *uarg;
    struct stat st;

    printf("Total files to read %d \n", n_files);

    int start_arg = 2; //change if user is giving mem size


    if(is_sync == 1){
        
        if(is_qos==0){
            for (int i = start_arg; i < n_files + start_arg; i++){
                char * file_path = malloc(100 * sizeof(char));
                strcpy(file_path, dir_path);
                strcat(file_path, "/");
                strcat(file_path, argv[i]);

                FILE *fp = fopen(file_path, "rb");
                stat(file_path, &st);
                int size = st.st_size;
                printf(" The size of file is %d \n", size);
                char *buff = (char *)malloc((size) * sizeof(char));
                size_t ret_code = fread(buff, sizeof(char), size, fp);
                // printf("Read file %s %ld \n", buff, sizeof(buff));
                fclose(fp);

                /**File has been read. We will pass the buff
                 * to client. Now, its client responsibilty
                 * to get the servicing done by dom0. 
                 **/

                uarg = &arg[i - start_arg];

                uarg->input_buffer = buff;
                uarg->input_size = size;
                uarg->client_id = i - start_arg;
                uarg->is_sync = is_sync;
                uarg->output_buffer = (char*)malloc((size) * sizeof(char));

                printf("Creating thread id: %d \n", arg[i - start_arg].client_id);
                pthread_create(&threads[i - start_arg], NULL, client_init, uarg);
                if(debug == 1){
                    printf("Created thread id: %d \n", arg[i - start_arg].client_id);
                
                }
            }
        }
        else{
            int files_per_thread = ceil(1.0*n_files/N) ;

            char **buff_arr = (char **)malloc((n_files+1) * sizeof(char*));
            int *size_arr = (int *)malloc((n_files+1) * sizeof(int));
            char ** output_buffer_arr = (char **)malloc((n_files+1) * sizeof(char*));
            char ** write_arr = &output_buffer_arr[0];
            int *output_size_arr = (int *)malloc((n_files+1) * sizeof(int));
            

            char temp = '0';
            buff_arr[n_files] =  (char *)malloc(sizeof(char));
            buff_arr[n_files]  = &temp;
            size_arr[n_files] = -1;
            printf("Start reading files next \n");

            for (int i = start_arg; i < n_files + start_arg; i++){
                char * file_path = (char *) malloc(100 * sizeof(char));
                strcpy(file_path, dir_path);
                strcat(file_path, "/");
                strcat(file_path, argv[i]);

                FILE *fp = fopen(file_path, "rb");
                stat(file_path, &st);
                int size = st.st_size;
                size_arr[i-start_arg] = size;
                printf(" The size of file is %d \n", size);
                buff_arr[i-start_arg] = (char *)malloc((size) * sizeof(char));
                size_t ret_code = fread(buff_arr[i-start_arg], sizeof(char), size, fp);
                fclose(fp);            
            }
        

            for(int i=0; i < n_threads; i++){
                uarg = &arg[i];

                uarg->input_buffer_arr = &(buff_arr);
                uarg->input_size_arr = &(size_arr);
                uarg->client_id = i;
                uarg->is_sync = is_sync;
                uarg->output_buffer_arr = &(output_buffer_arr);
                uarg->output_size_arr = &(output_size_arr);
                uarg->files_pending = files_per_thread;

                if(i >= (n_files % n_threads ))
                    uarg->files_pending = files_per_thread - 1;     
                if(n_threads==1 && i==0)
                    uarg->files_pending = 1;
                printf("files per thread%d\n", uarg->files_pending);
                printf("Creating sync with qos thread id: %d \n", arg[i].client_id);
                pthread_create(&threads[i], NULL, client_init, uarg);
                if(debug == 1){
                    printf("Created thread id: %d \n", arg[i].client_id);
                }
            }

            for(int i =0; i < n_threads; i++){
                pthread_join(threads[i], NULL);
            }


            for (int i = start_arg; i < n_files + start_arg; i++){

                
                
                char* output_file = (char *) malloc(100 * sizeof(char));
                
                printf("%s \n", argv[i]);
                strcpy(output_file, dir_path);
                strcat(output_file, "/");
                strcat(output_file, argv[i]);
                strcat(output_file, ".compressed");
                printf("%s \n", output_file);

                FILE *fp = fopen(output_file, "w");
                if (fp == NULL)
                {
                    printf("Error opening file!\n");
                    exit(1);
                }
                printf("id: %d \n",i-start_arg);
                

                printf("++%s\n", *(write_arr));


                fprintf(fp, "%s", *write_arr);
                write_arr++;

                fclose(fp);
                printf("closed file \n");


            }

        

        }

        if(is_qos == 0){

            for (int i = start_arg; i < n_files + start_arg; i++){

                pthread_join(threads[i - start_arg], NULL);
                
                num_files--;
                
                char * output_file = (char*) malloc(100 * sizeof(char));
                strcpy(output_file, dir_path);
                strcat(output_file, "/");
                strcat(output_file, argv[i]);
                strcat(output_file, ".compressed");
                printf("%s \n", output_file);
        
                FILE *fp = fopen(output_file, "w");
                if (fp == NULL)
                {
                    printf("Error opening file!\n");
                    exit(1);
                }
                // arg[i - start_arg].output_buffer = "test\n";
                printf("Hi %s\n", arg[i - start_arg].output_buffer);
                fprintf(fp, "%s", arg[i - start_arg].output_buffer);
                printf("Hi\n");
                // size_t my_sz = *arg[i - start_arg].output_size;
                // char *temp_buff = (char *)malloc((arg[i - start_arg].input_size) * sizeof(char));
                // printf("++%s\n",arg[i - start_arg].output_buffer);
                // char k = snappy_uncompress(arg[i - start_arg].output_buffer, arg[i - start_arg].output_size, temp_buff, arg[i - start_arg].input_size);
                // printf("%d \n", k);
                fclose(fp);

            }
        }


    }
    else{

        char **buff_arr = (char **)malloc((n_files+1) * sizeof(char*));
        char **pointer_array = (char **)malloc((n_files+1) * sizeof(char*));
        int *size_arr = (int *)malloc((n_files+1) * sizeof(int));
        char ** output_buffer_arr = (char **)malloc((n_files+1) * sizeof(char*));
        char ** write_arr = &output_buffer_arr[0];
        int *output_size_arr = (int *)malloc((n_files+1) * sizeof(int));
        

        // buff_arr = NULL;
        // mem_out = NULL;
        //char temp_char = '0';
        char temp = '0';
        buff_arr[n_files] =  (char *)malloc(sizeof(char));
        buff_arr[n_files]  = &temp;
        // mem_out[n_files] = &temp;
        size_arr[n_files] = -1;
        printf("Start reading files next \n");

        for (int i = start_arg; i < n_files + start_arg; i++){
            char * file_path = (char *) malloc(100 * sizeof(char));
            strcpy(file_path, dir_path);
            strcat(file_path, "/");
            strcat(file_path, argv[i]);

            FILE *fp = fopen(file_path, "rb");
            stat(file_path, &st);
            int size = st.st_size;
            size_arr[i-start_arg] = size;
            printf(" The size of file is %d \n", size);
            buff_arr[i-start_arg] = (char *)malloc((size) * sizeof(char));
            size_t ret_code = fread(buff_arr[i-start_arg], sizeof(char), size, fp);
            fclose(fp);            
        }
      

        for(int i=0; i < n_threads; i++){
            uarg = &arg[i];

            uarg->input_buffer_arr = &(buff_arr);
            uarg->input_size_arr = &(size_arr);
            uarg->client_id = i;
            uarg->output_mem_addr = &(pointer_array);
            uarg->is_sync = is_sync;
            uarg->processed_memory = &pointer_array;
            uarg->output_buffer_arr = &(output_buffer_arr);
            uarg->output_size_arr = &(output_size_arr);

            printf("Creating async thread id: %d \n", arg[i].client_id);
            pthread_create(&threads[i], NULL, client_init, uarg);
            if(debug == 1){
                printf("Created thread id: %d \n", arg[i].client_id);
            }
        }

        for(int i =0; i < n_threads; i++){
            pthread_join(threads[i], NULL);
        }

        for (int i = start_arg; i < n_files + start_arg; i++){

            
            
            char* output_file = (char *) malloc(100 * sizeof(char));
            
            printf("%s \n", argv[i]);
            strcpy(output_file, dir_path);
            strcat(output_file, "/");
            strcat(output_file, argv[i]);
            strcat(output_file, ".compressed");
            printf("%s \n", output_file);

            FILE *fp = fopen(output_file, "w");
            if (fp == NULL)
            {
                printf("Error opening file!\n");
                exit(1);
            }
            printf("id: %d \n",i-start_arg);
            

            printf("++%s\n", *(write_arr));


            fprintf(fp, "%s", *write_arr);
            write_arr++;

            fclose(fp);
            printf("closed file \n");


        }


    }  

    printf("yo===\n");
    //close server
    pthread_join(server_thread, NULL);
    printf("App exit()\n");

    shmdt(my_sh_mem);
    shmctl(shmid,IPC_RMID,NULL); 
    printf("Detached and destroyed shared memory\n");

    return 0;
}
