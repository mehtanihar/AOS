// #ifndef MY_CLIENT
// #define MY_CLIENT


typedef struct _thread_info{
    int client_id;
    int files_pending;
    char *input_buffer;
    char *output_buffer;
    int input_size;
    int *output_size;
    
    char ***input_buffer_arr;
    char ***output_buffer_arr;
    char ***output_mem_addr;
    char ***processed_memory;
    int **input_size_arr;
    int **output_size_arr;

    int is_sync;
    
} thread_info;

void *client_init(void *arg);

void *synchronised_compress(char *buffer, int input_size, char* opbuffer, int* output_size, int client_id);
void *asynchronised_compress(char*** buffer, int** input_size, char*** opbuffer, int** output_size, char*** pointer_array, int client_id, char*** processed_memory);
void *qos_synchronised_compress(char*** buffer, int** input_size, char*** opbuffer, int** output_size, int client_id, int files_pending);
