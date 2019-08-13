#include <sys/shm.h>

key_t shm_memory_key;
int shm_mem_size;
int req_producer_islocked, res_consumer_islocked, current_response_thread, pointer_array_islocked, processed_file_counter_islocked;
int is_sync;
int num_files;
int processed_file_counter;
char *req_prod, *req_cons, *res_prod, *res_cons;

int client_message_queue;

int curr_client_qos;
int is_qos;
int num_clients;