#ifndef GTFS
#define GTFS

#include <cstdio>
#include <cstdlib>
#include <iostream>
#include <unistd.h>
#include <sys/wait.h>
#include <map>
#include <string>

using namespace std;

#define PASS "\033[32;1m PASS \033[0m\n"
#define FAIL "\033[31;1m FAIL \033[0m\n"

// GTFileSystem basic data structures 

#define MAX_FILENAME_LEN 255
#define MAX_NUM_FILES_PER_DIR 1024

extern int do_verbose;

typedef struct gtfs {
    string dirname;
    map <string, void*>* file_map; 
    
    // TODO: Add any additional fields if necessary
} gtfs_t;

typedef struct file {
    string filename;
	int file_length;
    void* addr;
    // TODO: Add any additional fields if necessary
} file_t;

typedef struct write {
    string filename;
	int offset;
	int length;
	char *data;
    char *original_data;
    void* addr;
    // TODO: Add any additional fields if necessary
} write_t;

// GTFileSystem basic API calls

gtfs_t* gtfs_init(string directory, int verbose_flag);
int gtfs_clean(gtfs_t *gtfs);

file_t* gtfs_open_file(gtfs_t* gtfs, string filename, int file_length);
int gtfs_close_file(gtfs_t* gtfs, file_t* fl);
int gtfs_remove_file(gtfs_t* gtfs, file_t* fl);

char* gtfs_read_file(gtfs_t* gtfs, file_t* fl, int offset, int length);
write_t* gtfs_write_file(gtfs_t* gtfs, file_t* fl, int offset, int length, const char* data);
int gtfs_sync_write_file(write_t* write_id);
int gtfs_abort_write_file(write_t* write_id);

// TODO: Add here any additional data structures or API calls


#endif
