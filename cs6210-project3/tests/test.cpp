#include "../gtfs.hpp"
#include <fstream>
#include <string.h>
#include <stdio.h>
// Assumes files are located within the current directory
string directory;
int verbose;

// **Test 1**: Testing that data written by one process is then successfully read by another process.
void writer() {
	cout << "In writer" << endl;

	cout << "Initializing" << endl;
	gtfs_t *gtfs = gtfs_init(directory, verbose);
	cout << "Initialized" << endl;

	string filename = "test1.txt";
	cout << "Opening" << endl;
	file_t *fl = gtfs_open_file(gtfs, filename, 100);
	cout << "Opened. File contains: " << (char*)fl->addr << endl;
	string str = "Hi, I'm the writer.\n";
	
	cout << "Writing data: " << str << endl;
	write_t *wrt = gtfs_write_file(gtfs, fl, 10, str.length(), str.c_str());
	cout << "Data written. File contains: " << (char*)fl->addr << endl;

	cout << "Syncing file" << endl;
	gtfs_sync_write_file(wrt);
	cout << "File synced. File contains: " << (char*)fl->addr << endl;
	
	gtfs_close_file(gtfs, fl);
}

void reader() {
	cout << "In reader" << endl;
	cout << "Initializing" << endl;
	gtfs_t *gtfs = gtfs_init(directory, verbose);
	cout << "Initialized" << endl;

	string filename = "test1.txt";
	cout << "Opening" << endl;
	file_t *fl = gtfs_open_file(gtfs, filename, 100);
	cout << "Opened. File contains: " << (char*)fl->addr << endl;
	
	string str = "Hi, I'm the writer.\n"; 
	cout << "Reading data"<<endl;
	char *data = gtfs_read_file(gtfs, fl, 10, str.length());
	cout << "Data read. File contains: " << data << endl;
	cout << "Expected data: " << str << endl;
	// cout<<data<<"-------"<<endl;

	if (data != NULL) {
		str.compare(string(data)) == 0 ? cout << PASS : cout << FAIL;
	} else {
		cout << FAIL;
	}

	gtfs_close_file(gtfs, fl);
	
}

void test_write_read() {
	int pid;
	pid = fork();
	if (pid < 0) {
		perror("fork");
		exit(-1);
	}
	if (pid == 0) {
		writer();
		exit(0);
	}
	waitpid(pid, NULL, 0);
	reader();
}

// **Test 2**: Testing that aborting a write returns the file to its original contents.

void test_abort_write() {
	cout << "In test abort write" << endl;
	cout << "Initializing" << endl;
	gtfs_t *gtfs = gtfs_init(directory, verbose);
	cout << "Initialized" << endl;
	cout << "Opening file" << endl;
	string filename = "test2.txt";
	file_t *fl = gtfs_open_file(gtfs, filename, 100);
	cout << "Opened. File contains: " << (char*)fl->addr << endl;
	
	cout << "Writing file" << endl;
	string str = "Testing string.\n"; 
	write_t *wrt1 = gtfs_write_file(gtfs, fl, 0, str.length(), str.c_str());
	cout << "Data written. File contains: " << (char*)fl->addr << endl;


	cout << "Syncing file" << endl;
	gtfs_sync_write_file(wrt1);
	cout << "File synced" << endl;
	
	cout << "Writing file" << endl;
	write_t *wrt2 = gtfs_write_file(gtfs, fl, 20, str.length(), str.c_str());
	cout << "Data written. File contains: " << (char*)fl->addr << endl;
	
	cout << "Aborting write" << endl;
	gtfs_abort_write_file(wrt2);
	cout << "Aborted. File contains: " << ((char*)fl->addr) + wrt2->offset << endl;
	
		
	
	char *data1 = gtfs_read_file(gtfs, fl, 0, str.length());
	cout << "Reading file. File contains: " << data1 << endl;

	cout << "Expected output:" << str << endl;
	if (data1 != NULL) { 
		// First write was synced so reading should be successfull
		if (str.compare(string(data1)) != 0) {
			cout << FAIL;
		}
		// Second write was aborted and there was no string written in that offset
		char *data2 = gtfs_read_file(gtfs, fl, 20, str.length());
		cout << "Reading file aborted. File contains: " << data2 << endl;

		if (data2 == NULL) {
			cout << FAIL;
		} else if (string(data2).compare("") == 0) {
			cout << PASS;
		}
	} else {
		cout << FAIL;
	}
	cout<<"Closing file"<<endl;	
	gtfs_close_file(gtfs, fl);
	cout<<"Closed file"<<endl;

}

// **Test 3**: Testing that the logs are truncated.

void test_truncate_log() {
	cout << "In test abort write" << endl;
	cout << "Initializing" << endl;
	
	gtfs_t *gtfs = gtfs_init(directory, verbose);
	string filename = "test3.txt";
	file_t *fl = gtfs_open_file(gtfs, filename, 100);
	cout<<"Test 3 file opened" <<endl;
	string str = "Testing string.\n"; 
	write_t *wrt1 = gtfs_write_file(gtfs, fl, 0, str.length(), str.c_str());
	cout << "Test 3  file opened:" << wrt1->filename <<endl;

	gtfs_sync_write_file(wrt1);
	
	write_t *wrt2 = gtfs_write_file(gtfs, fl, 20, str.length(), str.c_str());
	gtfs_sync_write_file(wrt2);
	
	cout << "Before GTFS cleanup\n";
	system("ls -l .");
	
	gtfs_clean(gtfs);
	
	cout << "After GTFS cleanup\n";
	system("ls -l .");
	
	cout << "If log is truncated: " << PASS ;//<< "If exactly same output:" << FAIL;
	
	gtfs_close_file(gtfs, fl);

}

void test_unsynced_write_read() {

	cout << "In test unsynced write read" << endl;
	cout << "Initializing" << endl;
	gtfs_t *gtfs = gtfs_init(directory, verbose);
	cout << "Initialized" << endl;

	cout << "Opening file" << endl;
	string filename = "test4.txt";
	file_t *fl = gtfs_open_file(gtfs, filename, 100);
	cout << "Opened. File contains: " << (char*)fl->addr << endl;

	cout << "Writing file" << endl;
	string str1 = "Existing value.\n"; 
	write_t *wrt1 = gtfs_write_file(gtfs, fl, 0, str1.length(), str1.c_str());
	cout << "Data written. File contains: " << (char*)fl->addr << endl;

	cout << "Syncing file" << endl;
	gtfs_sync_write_file(wrt1);
	cout << "File synced" << endl;
	

	char *data1 = gtfs_read_file(gtfs, fl, 0, str1.length());
	cout << "Reading file. File contains: " << data1 << endl;

	
	cout << "Writing file" << endl;
	string str2 = "Testing string.\n"; 
	write_t *wrt2 = gtfs_write_file(gtfs, fl, 0, str2.length(), str2.c_str());
	cout << "Data written. File contains: " << (char*)fl->addr << endl;
	
	char *data2 = gtfs_read_file(gtfs, fl, 0, str2.length());
	cout << "Reading file. File contains: " << data2 << endl;

	cout << "Expected output:" << str2 << endl;
	if (data2 != NULL) { 
		// First write was synced so reading should be successfull
		if (str1.compare(string(data1)) != 0) {
			cout << FAIL;
		}
		else{
			cout << PASS;
		}
		
	} else {
		cout << FAIL;
	}

	cout << "Aborting write" << endl;
	gtfs_abort_write_file(wrt2);
	cout << "Aborted. File contains: " << ((char*)fl->addr) + wrt2->offset << endl;
	

	char *data3 = gtfs_read_file(gtfs, fl, 0, str2.length());
	cout << "Reading file. File contains: " << data3 << endl;

	if (str1.compare(string(data3)) != 0) {
			cout << FAIL;
		}
		else{
			cout << PASS;
		}


	cout<<"Closing file"<<endl;	
	gtfs_close_file(gtfs, fl);
	cout<<"Closed file"<<endl;

}


void test_abort_sync() {
	cout << "In test abort sync" << endl;
	cout << "Initializing" << endl;
	gtfs_t *gtfs = gtfs_init(directory, verbose);
	cout << "Initialized" << endl;

	cout << "Opening file" << endl;
	string filename = "test5.txt";
	file_t *fl = gtfs_open_file(gtfs, filename, 100);
	cout << "Opened. File contains: " << (char*)fl->addr << endl;

	cout << "Write 1" << endl;
	string str01 = "Existing value.\n"; 
	write_t *wrt1 = gtfs_write_file(gtfs, fl, 0, str01.length(), str01.c_str());
	cout << "Data written. File contains: " << (char*)fl->addr << endl;

	char *data1 = gtfs_read_file(gtfs, fl, 0, str01.length());
	cout << "Reading file. File contains: " << data1 << endl;


	cout << "Write 2" << endl;
	string str2 = "Modified value.\n"; 
	write_t *wrt2 = gtfs_write_file(gtfs, fl, 0, str2.length(), str2.c_str());
	cout << "Data written. File contains: " << (char*)fl->addr << endl;

	cout << "Aborting write 1" << endl;
	gtfs_abort_write_file(wrt1);
	cout << "Aborted. File contains: " << ((char*)fl->addr) + wrt1->offset << endl;
			
	char *data2 = gtfs_read_file(gtfs, fl, 0, str2.length());
	cout << "Reading file. File contains: " << data2 << endl;

	cout << "Syncing write 2 " << endl;
	
	gtfs_sync_write_file(wrt2);
	
	cout << "Synced. File contains: " << ((char*)fl->addr) << endl;
			
	char *data3 = gtfs_read_file(gtfs, fl, 0, str2.length());
	cout << "@@@@@@@@@@@ File contains: " << ((char*)fl->addr) << endl;
	cout << "Reading file. File contains: " << data3 << endl;


	cout << "Expected output:" << str2 << endl;
	if (data3 != NULL) { 
		// First write was synced so reading should be successfull
		if (str2.compare(string(data3)) != 0) {
			cout << FAIL;
		}
		else{
			cout << PASS;
		}
		
	} else {
		cout << FAIL;
	}


	cout<<"Closing file"<<endl;	
	gtfs_close_file(gtfs, fl);
	cout<<"Closed file"<<endl;

}


void test_close_before_open(){
	gtfs_t *gtfs = gtfs_init(directory, verbose);

	file_t *fl = NULL;

	int res= gtfs_close_file(gtfs, fl);

	if (res==-1){
		cout << "File not open!" << endl;
		cout << PASS;
	}
	else{
		cout << FAIL;
	}

}

void test_remove_before_create(){
	gtfs_t *gtfs = gtfs_init(directory, verbose);

	file_t *fl = NULL;

	int res= gtfs_remove_file(gtfs, fl);

	if (res==-1){
		cout<<"Unable to remove the file which doesn't exist\n";
		cout << PASS;
	}
	else{
		cout << FAIL;
	}

}


void test_filename_too_long(){	
	gtfs_t *gtfs = gtfs_init(directory, verbose);
	string filename = "testtesttesttesttesttesttesttesttesttesttesttesttesttesttesttesttesttesttesttesttesttesttesttesttesttesttesttesttesttesttesttesttesttesttesttesttesttesttesttesttesttesttesttesttesttesttesttesttesttesttesttesttesttesttesttesttesttesttesttesttesttesttesttesttesttesttesttesttesttesttesttesttesttesttest.txt";
	file_t *fl = gtfs_open_file(gtfs, filename, 100);

	if (fl==NULL){
		cout << PASS;
	}
	else{
		cout << FAIL;
	}
}

void writer_crash() {
	cout << "In writer" << endl;

	cout << "Initializing" << endl;
	gtfs_t *gtfs = gtfs_init(directory, verbose);
	cout << "Initialized" << endl;

	string filename = "test9.txt";
	cout << "Opening" << endl;
	file_t *fl = gtfs_open_file(gtfs, filename, 100);
	cout << "Opened. File contains: " << (char*)fl->addr << endl;
	string str = "Hi, I'm the writer.\n";
	
	cout << "Writing data: " << str << endl;
	write_t *wrt = gtfs_write_file(gtfs, fl, 10, str.length(), str.c_str());
	cout << "Data written. File contains: " << (char*)fl->addr << endl;

	cout << "Syncing file" << endl;
	gtfs_sync_write_file(wrt);
	cout << "File synced. File contains: " << (char*)fl->addr << endl;
	
	abort(); // Aborting before closing the file and applying the log.
	gtfs_close_file(gtfs, fl);
}


void reader_crash() {
	cout << "In reader" << endl;
	cout << "Initializing" << endl;
	gtfs_t *gtfs = gtfs_init(directory, verbose);
	cout << "Initialized" << endl;

	string filename = "test9.txt";
	cout << "Opening" << endl;
	file_t *fl = gtfs_open_file(gtfs, filename, 100);
	cout << "Opened. File contains: " << (char*)fl->addr << endl;
	
	string str = "Hi, I'm the writer.\n";
	string str_null = ""; 
	cout << "Reading data"<<endl;
	char *data = gtfs_read_file(gtfs, fl, 10, str.length());
	cout << "Data read. File contains: " << data << endl;
	cout << "Expected data: " << str << endl;
	// cout<<data<<"-------"<<endl;

	if (data != NULL) {
		cout << "File expected to be empty as system crashed!" << endl;
		str_null.compare(string(data)) == 0 ? cout << PASS : cout << FAIL;
	} else {
		cout << FAIL;
	}
	gtfs_close_file(gtfs,fl);
	cout << "Cleaning logs" << endl;
	gtfs_clean(gtfs);


	cout << "Opening" << endl;
	file_t *fl_new = gtfs_open_file(gtfs, filename, 100);
	cout << "Opened. File contains: " << (char*)fl_new->addr << endl;

	cout << "Reading data"<<endl;
	char *data2 = gtfs_read_file(gtfs, fl_new, 10, str.length());
	cout << "Data read. File contains: " << data2 << endl;
	cout << "Expected data: " << str << endl;
	// cout<<data<<"-------"<<endl;

	if (data2 != NULL) {
		if(str.compare(string(data2)) == 0){
			cout << "File has been successfully recovered from system crash" << endl;
			cout << PASS;
		} else{
			cout << FAIL;
		}  
	} else {
		cout << FAIL;
	}


	gtfs_close_file(gtfs, fl_new);
	
}

void test_crash(){
	int pid;
	pid = fork();
	if (pid < 0) {
		perror("fork");
		exit(-1);
	}
	if (pid == 0) {
		writer_crash();
		exit(0);
	}
	waitpid(pid, NULL, 0);
	reader_crash();
	system("rm test9.txt");
	

}

// TODO: Implement any additional tests

int main(int argc, char **argv) {
    if (argc < 2)
      printf("Usage: ./test verbose_flag\n");
    else
      verbose = strtol(argv[1], NULL, 10);
	
	// Get current directory path
	char cwd[256];
	if (getcwd(cwd, sizeof(cwd)) != NULL) {
		directory = string(cwd);
	} else {
		cout << "[cwd] Something went wrong.\n";
	}
	

	// Call existing tests
	cout << "================== Test 1 ==================\n";
	cout << "Testing that data written by one process is then successfully read by another process.\n";
	test_write_read();
	
	cout << "================== Test 2 ==================\n";
	cout << "Testing that aborting a write returns the file to its original contents.\n";
	test_abort_write();

	cout << "================== Test 3 ==================\n";
	cout << "Testing that the logs are truncated.\n";	
	test_truncate_log();



	// // TODO: Call any additional tests


	cout << "================== Test 4 ==================\n";
	cout << "Testing read after the unsynced writes\n";	
	test_unsynced_write_read();

	cout << "================== Test 5 ==================\n";
	cout << "Testing sync of write 2 after abort of write 1\n";	
	test_abort_sync();

	cout << "================== Test 6 ==================\n";
	cout << "Testing close the file without opening\n";	
	test_close_before_open();

	cout << "================== Test 7 ==================\n";
	cout << "Testing remove the file without creating it\n";	
	test_remove_before_create();

	cout << "================== Test 8 ==================\n";
	cout << "Testing filename is too long\n";	
	test_filename_too_long();

	cout << "================== Test 9 ==================\n";
	cout << "Testing system crash recovery\n";	
	test_crash();
	
}
