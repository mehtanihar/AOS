#include "gtfs.hpp"
#include <sys/mman.h>
#include <sys/types.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/stat.h>
#include <assert.h>
#include <utility>
#include <fstream>
#include <stdlib.h>
#include <string.h>
#include <map>
#include <stdio.h>
#include <string>
#include <sstream>

#define VERBOSE_PRINT(verbose, str...) do { \
	if (verbose) cout << "VERBOSE: "<< __FILE__ << ":" << __LINE__ << ":" << __func__ << "():" << str; \
	} while(0)

int do_verbose;

gtfs_t* gtfs_init(string directory, int verbose_flag) {
	do_verbose = verbose_flag;
	gtfs_t *gtfs = NULL;
	VERBOSE_PRINT(do_verbose, "Initializing GTFileSystem inside directory " << directory << "\n");
	//TODO: Any additional initializations and checks
	gtfs = (gtfs_t*)calloc(1,sizeof(gtfs_t));
	gtfs->dirname = directory;
	struct stat dir_info;
	if (stat(directory.c_str(), &dir_info) == 0){
		cout << " Directory instance already initialized!" << endl;
	}	
	else{
		mkdir(directory.c_str(), 0777);
		printf("Directory does not exist. Created directory! \n");
	}

	(gtfs->file_map) = new map<string,void*>(); 
	VERBOSE_PRINT(do_verbose, "Success.\n"); //On success returns non NULL.
	return gtfs;
}

int gtfs_clean(gtfs_t *gtfs) {
	int ret = -1;
	if (gtfs) {

		VERBOSE_PRINT(do_verbose, "Cleaning up GTFileSystem inside directory " << gtfs->dirname << "\n");
		
		for(map<string,void*>::iterator it = (*gtfs->file_map).begin(); it != (*gtfs->file_map).end(); ++it) {
			string backup_filename = it->first + ".bak";
			cout << backup_filename << "\n";
			ifstream ifs(backup_filename.c_str());
			

			if(ifs.good()){
				int size;
				struct stat s;
				int fd = open((it->first).c_str(), O_CREAT|O_RDWR, S_IRWXU);
				int status = fstat (fd, & s);
				size = s.st_size;
				
				void * addr;
				if((addr = mmap(NULL, size, PROT_READ | PROT_WRITE | PROT_EXEC, MAP_SHARED, fd, 0)) == MAP_FAILED){
					cout <<"error: file not in virtual memory";
					//error
				}
				
				while(ifs.peek() != EOF){
					
					string filename;
					int length, offset;
					ifs >> filename;
					ifs >> length;
					ifs >> offset;
					char data[length];
					
					string line = "";
					strcpy(data,line.c_str());
					getline(ifs,line);
					while(line != " @@@###$$$ "){
						getline(ifs,line);
						
						strcat(data,(line + "\n").c_str());
						
					}
					memcpy((char*)addr + offset, data,length);


				}
				

				remove(backup_filename.c_str());
			}
			else{
				cout << "No backup file+++++" <<endl;
			}
			ifs.close();

			

		}


	} else {
		VERBOSE_PRINT(do_verbose, "GTFileSystem does not exist\n");
		return ret;
	}
	//TODO: Any additional initializations and checks
	
	VERBOSE_PRINT(do_verbose, "Success.\n"); //On success returns 0.
	return ret;
}

file_t* gtfs_open_file(gtfs_t* gtfs, string filename, int file_length) {
	file_t *fl = (file_t*) calloc(1, sizeof(file_t));
	if (gtfs) {
		VERBOSE_PRINT(do_verbose, "Opening file " << filename << " inside directory " << gtfs->dirname << "\n");
		
		if(filename.size() > MAX_FILENAME_LEN){
			cout << "The filename size exceeds maximum file length " << endl;
			return NULL;
		}
		if((*gtfs->file_map).size() >= MAX_NUM_FILES_PER_DIR){
			cout << "Number of files exceeds the maximum number of files per directory" << endl;
			return NULL;
		}

		int size;
		struct stat s;
		// cout<<filename<<" "<<file_length<<endl;
		int fd = open(filename.c_str(), O_CREAT|O_RDWR, S_IRWXU);
		int status = fstat (fd, & s);
		size = s.st_size;

		cout << size << " " << file_length << endl;
		if(size > file_length + 1){
			cout << "Actual file length is larger than length specified. Aborting operation!" << endl;
			return NULL;
		}

		if(size < file_length){
			lseek(fd,file_length,SEEK_SET);
			write(fd,"",1);
		}
		
		void * addr;
		if((addr = mmap(NULL, file_length, PROT_READ | PROT_WRITE | PROT_EXEC, MAP_PRIVATE | MAP_POPULATE, fd, 0)) == MAP_FAILED){
			cout <<"error: file cannot be assigned to virtual memory";
		}
		
		(*(gtfs->file_map)).insert(make_pair(filename,addr));
		fl->filename = filename;
		fl->file_length = file_length;
		fl->addr = addr; 
		

	} else {
		VERBOSE_PRINT(do_verbose, "GTFileSystem does not exist\n");
		return NULL;
	}
	//TODO: Any additional initializations and checks
	
	VERBOSE_PRINT(do_verbose, "Success.\n"); //On success returns non NULL.
	return fl;
}

int gtfs_close_file(gtfs_t* gtfs, file_t* fl) {
	int ret = -1;
	if (gtfs and fl) {
		VERBOSE_PRINT(do_verbose, "Closing file " << fl->filename << " inside directory " << gtfs->dirname << "\n");
		string backup_filename = fl->filename + ".bak";
		// cout << backup_filename << "\n";
		// if(backup_filename == "test2.txt.bak"){
		// 	backup_filename = "test1.txt.bak";
		// }

		ifstream ifs(backup_filename.c_str());
		
		if(ifs.good()){
			int size;
			struct stat s;
			int fd = open((fl->filename).c_str(), O_CREAT|O_RDWR, S_IRWXU);
			int status = fstat (fd, & s);
			size = s.st_size;
			
			
			void * addr;
			if((addr = mmap(NULL, size, PROT_READ | PROT_WRITE | PROT_EXEC, MAP_SHARED, fd, 0)) == MAP_FAILED){
				cout <<"error: file not in virtual memory" << endl;
				//error
			}

			// cout<<"before while"<<(char*)addr<<endl;
			
			while(ifs.peek() != EOF){
				
				string filename;
				int length, offset;
				ifs >> filename;
				ifs >> length;
				ifs >> offset;
				char data[length];// = (char*)malloc(length * sizeof(char));
				// cout << filename << " " << length << " " <<  offset <<endl;
				string line = "";
				strcpy(data,line.c_str());
				// cout << "Printing lines" << endl;
				getline(ifs,line);
				while(line != " @@@###$$$ "){
					
					getline(ifs,line);
					// cout<<"Line:"<<line<<endl;
					strcat(data,(line + "\n").c_str());		
				
				}
				
				memcpy((char*)addr + offset, data,length);
			
			}
			
			munmap(addr,size);

		}
		else{
			cout<<"no backup file" <<endl;
		}
		
	

		(*(gtfs->file_map)).erase(fl->filename);
		munmap(fl->addr,fl->file_length);


		
	} else {
		VERBOSE_PRINT(do_verbose, "GTFileSystem or file does not exist\n");
		return ret;
	}
	//TODO: Any additional initializations and checks
	VERBOSE_PRINT(do_verbose, "Success.\n"); //On success returns 0.
	return ret;
}

int gtfs_remove_file(gtfs_t* gtfs, file_t* fl) {
	int ret = -1;
	if (gtfs and fl) {
	    VERBOSE_PRINT(do_verbose, "Removing file " << fl->filename << " inside directory " << gtfs->dirname << "\n");
		cout << "Removing file" << endl;
		remove((fl->filename).c_str());
    	remove((fl->filename+".bak").c_str());
		(*(gtfs->file_map)).erase(fl->filename);
		munmap(fl->addr,fl->file_length);
	
	} else {
	    VERBOSE_PRINT(do_verbose, "GTFileSystem or file does not exist\n");
	    return ret;
	}
	//TODO: Any additional initializations and checks
	
	VERBOSE_PRINT(do_verbose, "Success.\n"); //On success returns 0.
	return ret;	
	
}

char* gtfs_read_file(gtfs_t* gtfs, file_t* fl, int offset, int length) {
	char* ret_data = NULL;
	if (gtfs and fl) {
		VERBOSE_PRINT(do_verbose, "Reading " << length << " bytes starting from offset " << offset << " inside file " << fl->filename << "\n");
		
		if((*(gtfs->file_map)).find(fl->filename) != (*(gtfs->file_map)).end()){

			ret_data = (char*)calloc(1,length * sizeof(char));	
			void *addr = fl->addr;
			memcpy ( ret_data, ((char*)addr) + offset, length );
			//return ret_data;

		}
		else{

			cout<<"File not opened yet! Aborting read operation" << endl;
			return NULL;
			//error: read before open
		}

	} else {
		VERBOSE_PRINT(do_verbose, "GTFileSystem or file does not exist\n");
		return NULL;
	}
	//TODO: Any additional initializations and checks
	
	VERBOSE_PRINT(do_verbose, "Success.\n"); //On success returns pointer to data read.
	return ret_data;	
}

write_t* gtfs_write_file(gtfs_t* gtfs, file_t* fl, int offset, int length, const char* data) {
	write_t *write_id = (write_t*) calloc(1, sizeof(write_t));
	if (gtfs and fl) {
		VERBOSE_PRINT(do_verbose, "Writting " << length << " bytes starting from offset " << offset << " inside file " << fl->filename << "\n");
			
		write_id->original_data =  (char*)calloc(1,length * sizeof(char));
		write_id->addr = fl->addr;

		if(fl->addr == NULL){
			cout << "No file exists in virtual memory" << endl;
			return NULL;
		}
		memcpy(write_id->original_data,((char*)fl->addr) + offset,length);
		
		memcpy(((char*)(fl->addr)+offset),data,length);
		
		write_id->offset = offset;
		
		write_id->filename.push_back('a');
		
		write_id->filename = fl->filename;
		write_id->length = length;
		write_id->offset = offset;
		write_id->data =  (char*)calloc(1,length * sizeof(char));
		strcpy(write_id->data,data);
		
	
	} else {
		VERBOSE_PRINT(do_verbose, "GTFileSystem or file does not exist\n");
		return NULL;
	}
	//TODO: Any additional initializations and checks
	// cout<<fl->addr<<" "<<(char*)fl->addr<<" after write"<<endl;
		
	VERBOSE_PRINT(do_verbose, "Success.\n"); //On success returns non NULL.
	return write_id;	
}

int gtfs_sync_write_file(write_t* write_id) {
	int ret = -1;
	if (write_id) {
		VERBOSE_PRINT(do_verbose, "Persisting write of " << write_id->length << " bytes starting from offset " << write_id->offset << " inside file " << write_id->filename << "\n");
		
		string filename = write_id->filename;
		string backup_filename = filename + ".bak";
		ofstream ofs;
		cout << "Data: " << write_id->data << endl;
		cout << "Persisting write of " << write_id->length << " bytes starting from offset " << write_id->offset << " inside file " << write_id->filename << "\n";
		
		int fd = open(backup_filename.c_str(), O_CREAT|O_RDWR| O_APPEND, S_IRWXU);
		write(fd,filename.c_str(),filename.size());
		write(fd,"\n",1);
		
		std::stringstream ss1;
		ss1 << write_id->length;
		write(fd,(ss1.str()).c_str(),(ss1.str()).size());
		write(fd,"\n",1);
		
		
		std::stringstream ss2;
		ss2 << write_id->offset;
		write(fd,(ss2.str()).c_str(),(ss2.str()).size());
		write(fd,"\n",1);

		write(fd,write_id->data, write_id->length);
		write(fd,"\n",1);
		write(fd," @@@###$$$ ",11);
		write(fd,"\n",1);
		
		close(fd);
		cout << " Old data: " << ((char*)(write_id->addr)) << endl;
		memcpy(((char*)(write_id->addr)+write_id->offset),write_id->data,write_id->length);

		cout << " New data: " << ((char*)(write_id->addr)) << endl;
		

	} else {
		VERBOSE_PRINT(do_verbose, "Write operation does not exist\n");
		return ret;
	}
	//TODO: Any additional initializations and checks
	
	VERBOSE_PRINT(do_verbose, "Success.\n"); //On success returns number of bytes written.
	return ret;	
}

int gtfs_abort_write_file(write_t* write_id) {
	int ret = -1;
	if (write_id) {

		memcpy(write_id->data,write_id->original_data,write_id->length);
		memcpy(((char*)write_id->addr) + write_id->offset,write_id->original_data,write_id->length);
		
		
		VERBOSE_PRINT(do_verbose, "Aborting write of " << write_id->length << " bytes starting from offset " << write_id->offset << " inside file " << write_id->filename << "\n");
	} else {
		VERBOSE_PRINT(do_verbose, "Write operation does not exist\n");
		return ret;
	}
	//TODO: Any additional initializations and checks
	
	VERBOSE_PRINT(do_verbose, "Success.\n"); //On success returns 0.
	return ret;	
}

