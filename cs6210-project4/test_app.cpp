#include "gtstore.hpp"
#include "client.cpp"

void single_set_get(int client_id) {
	cout << "Testing single set-get for GTStore by client " << client_id << ".\n";

    GTStoreClient client;
	client.init(client_id);
	
	string key = to_string(client_id);
	vector<string> value;
	value.push_back("phone");
	value.push_back("phone_case");
	
	client.put(key, value);
	client.get(key);

	client.finalize();
}


void custom_single_set_get(int client_id, string value1, string value2) {
	cout << "Testing single set-get for GTStore by client " << client_id << ".\n";

    GTStoreClient client;
	client.init(client_id);
	
	string key = to_string(client_id);
	vector<string> value;
	value.push_back(value1);
	value.push_back(value2);
	
	client.put(key, value);
	client.get(key);

	client.finalize();
}



void test_erase(int client_id) {
	cout << "Testing single set-get for GTStore by client " << client_id << ".\n";

    GTStoreClient client;
	client.init(client_id);
	
	string key = to_string(client_id);
	vector<string> value;
	value.push_back("TV");
	value.push_back("laptop");

	value.push_back("phone");
	
	client.put(key, value);
	client.get(key);
	client.erase(key,"TV");
	client.get(key);
	client.finalize();
}


void multi_client_single_set_get() {
	cout << "Testing single set-get for GTStore by multiple client. \n";
	int num_clients = 5;
	int client_ids[] =  {0,1,2,3,4};
	string values[] = {"phone", "phone case", "laptop", "laptop cover","apples","oranges","trimmer","shaving cream","headphone","speaker"};
	for(int i=0;i<num_clients;i++) // loop will run n times (n=5) 
    { 
        if(fork() == 0) 
        { 
			GTStoreClient client;//= new GTStoreStorage();
			client.init(client_ids[i]);
			string key = to_string(client_ids[i]);
			vector<string> value;
			value.push_back(values[i*2]);

			value.push_back(values[i*2 + 1]);
			client.put(key,value);
			client.get(key);
			client.finalize();
			
			
            // printf("[son] pid %d from [parent] pid %d\n",getpid(),getppid()); 
            exit(0); 
        } 
    } 

  
}


int main(int argc, char **argv) {
	string test = string(argv[1]);
	// int client_id = 0;
	
	string test1 = "single_set_get";
	string test2 = "custom_single_set_get";
	string test3 = "multi_client_single_set_get";
	string test4 = "test_erase";
	
	if (string(argv[1]) ==  test1) {
		int client_id = atoi(argv[2]);
		single_set_get(client_id);
	}
	else if (string(argv[1]) ==  test2) {
		int client_id = atoi(argv[2]);
		string value1 = argv[3];
		string value2 = argv[4];
		custom_single_set_get(client_id, value1, value2);
	}
	else if (string(argv[1]) ==  test3) {
		multi_client_single_set_get();
	}
	else if(string(argv[1]) ==  test4){
		int client_id = atoi(argv[2]);
		test_erase(client_id);
	}

}
