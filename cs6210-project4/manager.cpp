#include "gtstore.hpp"
#include "storage.cpp"

#define WIN32_LEAN_AND_MEAN  /* required by xmlrpc-c/server_abyss.hpp */

#ifdef _WIN32
#  include <windows.h>
#else
#  include <unistd.h>
#endif

#ifdef _WIN32
  #define SLEEP(seconds) SleepEx(seconds * 1000);
#else
  #define SLEEP(seconds) sleep(seconds);
#endif


void GTStoreManager::init() {
	// cout << "Inside GTStoreManager::init()\n";
	srand((unsigned)time(0)); 
	max_ring_size = 360;
	replication_factor = 3;
	min_dist = 5;
	wait_flag = false;
	prev_rpc_node_name = "";
	next_rpc_node_name = "";
	socket_number = 8000;
}

void GTStoreManager::init(int size) {
	// cout << "Inside GTStoreManager::init(int size)\n";
	srand((unsigned)time(0)); 
	max_ring_size = size;
	replication_factor = 3;
	min_dist = 5;
	wait_flag = false;
	prev_rpc_node_name = "";
	next_rpc_node_name = "";
	socket_number = 8000;
	
	
}

void GTStoreManager::set_wait_flag(bool val){
	wait_flag = val;
}



class managerGetMethod : public xmlrpc_c::method {
public:
	GTStoreManager* my_manager ;//= new GTStoreStorage();
    managerGetMethod(GTStoreManager* manager) {
        this->_signature = "i:s";
        this->_help = "This method adds two integers together";
		my_manager = manager;
    }
    void execute(xmlrpc_c::paramList const& paramList,
            xmlrpc_c::value *   const  retvalP) {

		string const key(paramList.getString(0));
		vector<string> ret_data = my_manager->get_data(key);
		vector<xmlrpc_c::value> ret_data_array;

		for(int i = 0; i < ret_data.size();i++){
			ret_data_array.push_back(xmlrpc_c::value_string(ret_data[i]));
		}

		*retvalP = xmlrpc_c::value_array(ret_data_array);
    }
};


class managerInsertMethod : public xmlrpc_c::method {
public:
	GTStoreManager* my_manager ;//= new GTStoreStorage();
    managerInsertMethod(GTStoreManager* manager) {
        this->_signature = "i:ssss";
        this->_help = "This method adds two integers together";
		my_manager = manager;
    }
    void execute(xmlrpc_c::paramList const& paramList,
            xmlrpc_c::value *   const  retvalP) {

		string const node_name(paramList.getString(0));
		string const key(paramList.getString(1));
		string const data(paramList.getString(2));
		string const master_node(paramList.getString(3));
		int ret = my_manager->insert_data(node_name, key, data, master_node);
		*retvalP = xmlrpc_c::value_int(ret);
              
    }
};


class managerRemovePutMethod : public xmlrpc_c::method {
public:
	GTStoreManager* my_manager ;//= new GTStoreStorage();
    managerRemovePutMethod(GTStoreManager* manager) {
        this->_signature = "i:ss";
        this->_help = "This method adds two integers together";
		my_manager = manager;
    }
    void execute(xmlrpc_c::paramList const& paramList,
            xmlrpc_c::value *   const  retvalP) {
		
		
		my_manager->remove_node((*my_manager).ring[0]);
		string const key(paramList.getString(0));
		string const data(paramList.getString(1));
		int ret = my_manager->put_data(key, data);
		*retvalP = xmlrpc_c::value_string(key);
              
    }
};


class managerPutMethod : public xmlrpc_c::method {
public:
	GTStoreManager* my_manager ;//= new GTStoreStorage();
    managerPutMethod(GTStoreManager* manager) {
        this->_signature = "i:ss";
        this->_help = "This method adds two integers together";
		my_manager = manager;
    }
    void execute(xmlrpc_c::paramList const& paramList,
            xmlrpc_c::value *   const  retvalP) {

		string const key(paramList.getString(0));
		string const data(paramList.getString(1));
		int ret = my_manager->put_data(key, data);
		*retvalP = xmlrpc_c::value_string(key);
              
    }
};


class managerEraseMethod : public xmlrpc_c::method {
public:
	GTStoreManager* my_manager ;//= new GTStoreStorage();
    managerEraseMethod(GTStoreManager* manager) {
        this->_signature = "i:ss";
        this->_help = "This method adds two integers together";
		my_manager = manager;
    }
    void execute(xmlrpc_c::paramList const& paramList,
            xmlrpc_c::value *   const  retvalP) {

		string const key(paramList.getString(0));
		string const data(paramList.getString(1));
		int ret = my_manager->erase_data(key, data);
		*retvalP = xmlrpc_c::value_string(key);
              
    }
};



class managerAddMethod : public xmlrpc_c::method {
public:
	GTStoreManager* my_manager ;//= new GTStoreStorage();
    managerAddMethod(GTStoreManager* manager) {
        this->_signature = "i:s";
        this->_help = "This method adds two integers together";
		my_manager = manager;
    }
    void execute(xmlrpc_c::paramList const& paramList,
            xmlrpc_c::value *   const  retvalP) {

		string const node_name(paramList.getString(0));
		int ret = my_manager->add_node(node_name);
		*retvalP = xmlrpc_c::value_int(ret);
              
    }
};


class managerRemoveMethod : public xmlrpc_c::method {
public:
	GTStoreManager* my_manager ;//= new GTStoreStorage();
    managerRemoveMethod(GTStoreManager* manager) {
        this->_signature = "i:s";
        this->_help = "This method adds two integers together";
		my_manager = manager;
    }
    void execute(xmlrpc_c::paramList const& paramList,
            xmlrpc_c::value *   const  retvalP) {

		string const node_name(paramList.getString(0));
		int ret = my_manager->remove_node(node_name);
		*retvalP = xmlrpc_c::value_int(ret);
              
    }
};


int GTStoreManager::remove_node(string node_name) {
	/**
	 * Return 0 if operation successful
	 * Else return -1
	 */
	cout << "Trying to remove node" << node_name << endl;
	cout << "ring reverse begin :" << ring_reverse.begin()->first << " => " << ring_reverse.begin()->second << endl;


	if(node_name == ring[0] && ring.size()>1){
		// cout << " Cannot delete node 0" << endl;
		// return -1;
				// copy the data locally
		
		// map< string, pair<string, string> >::iterator temp_it;

		map<int, string>::iterator it, next;
		it = ring.begin();
		next = std::next(it);
		int offset = next->first;
		
		int curr_pos = 0;
		// cout << "Trying to delete node 0" << endl;
		// cout << "Position of node " << node_name << ":" <<ring_reverse[node_name] << endl;

		vector<xmlrpc_c::value> all_data;
		vector<xmlrpc_c::value>::iterator temp_it;

		try {
			string uri_path;
			uri_path = "http://localhost:"+to_string(process_map[node_name])+"/";
			string const serverUrl(uri_path + node_name);
			string const methodName(node_name+".get_all");
			
			xmlrpc_c::clientSimple myClient;
			xmlrpc_c::value result;
			

			myClient.call(serverUrl, methodName, "i", &result, 1);
	

			xmlrpc_c::value_array data_array(result);
			// cout << "Received" << endl;
				// Assume the method returned an integer; throws error if not

			// cout << "Result of RPC (sum of 5 and 7): " << sum << endl;

			all_data = data_array.vectorValueValue();


		} catch (exception const& e) {
			cerr << "Client threw error: " << e.what() << endl;
		} catch (...) {
			cerr << "Client threw unexpected error." << endl;
		}
		
		try {


			xmlrpc_c::value ret_key, result;
			string uri_path;
			uri_path = "http://localhost:"+to_string(process_map[node_name])+"/";
			string const serverUrl(uri_path + node_name);
			cout << "Trying  " << serverUrl << endl;
			string const methodName(node_name+".remove");
			
			xmlrpc_c::clientSimple myClient;
							
			myClient.call(serverUrl, methodName, "s", &result, node_name.c_str());
			
			// ret_key = result;
			// cout << "Success in send" << endl;

			while(it!=ring.end()){	
				it++;
				ring.erase(curr_pos);
				ring_reverse.erase(node_name);

				if(it== ring.end()){
					break;
				}
				string curr_node_name = it->second;
				ring_reverse[it->second] -= offset;
				ring[it->first - offset] = it->second;
				curr_pos = it->first;

			}

			for(int i = 0; i < all_data.size(); i+=3)	{
				xmlrpc_c::value_string const d1(all_data[i]);
				string const key(static_cast<string>(d1));
				xmlrpc_c::value_string const d2(all_data[i+1]);
				string const data(static_cast<string>(d2));

				int ret = put_data(key,data);
				if(ret == -1){
					cout<<"Error in putting data\n";
					return -1;
				}		
			}
			// cout << "Size of ring" << ring.size() << endl;
			cout << "case begin node: Sucessfully removed node " << node_name << endl;
			return 0;
			
		} catch (exception const& e) {
			cerr << "Client threw error: " << e.what() << endl;
		} catch (...) {
			cerr << "Client threw unexpected error." << endl;
		}
		
	

	}

	if(ring.size()==0){
		cout<<"Empty ring\n";
		return -1;
	}
	
	if(ring.size()==1){
		int temp_idx = ring_reverse[node_name];
		ring.erase(temp_idx);
		ring_reverse.erase(node_name);

		try {


			xmlrpc_c::value ret_key, result;
			string uri_path;
			uri_path = "http://localhost:"+to_string(process_map[node_name])+"/";
			string const serverUrl(uri_path + node_name);
			cout << "Trying  " << serverUrl << endl;
			string const methodName(node_name+".remove");
			
			xmlrpc_c::clientSimple myClient;
							
			myClient.call(serverUrl, methodName, "s", &result, node_name.c_str());
			
			// ret_key = result;
			// cout << "Success in send" << endl;
		
		
			
		} catch (exception const& e) {
			cerr << "Client threw error: " << e.what() << endl;
		} catch (...) {
			cerr << "Client threw unexpected error." << endl;
		}


		cout << "case ring size 1: Sucessfully removed node " << node_name << endl;
		return 0;
	}

	if ( ring_reverse.find(node_name) == ring_reverse.end() ) {
  		cout<<"Node with the name "<<node_name<<" not found\n";
		return -1;
	} 
	else {	
		
		int temp_idx = ring_reverse[node_name];
		ring.erase(temp_idx);
		ring_reverse.erase(node_name);
		
		vector<xmlrpc_c::value> all_data;
		vector<xmlrpc_c::value>::iterator temp_it;
		// copy the data locally
		try {
			string uri_path;
			uri_path = "http://localhost:"+to_string(process_map[node_name])+"/";
			string const serverUrl(uri_path + node_name);
			string const methodName(node_name+".get_all");
			
			xmlrpc_c::clientSimple myClient;
			xmlrpc_c::value result;
			

			myClient.call(serverUrl, methodName, "i", &result, 1);
	

			xmlrpc_c::value_array data_array(result);
			// cout << "Received" << endl;
				// Assume the method returned an integer; throws error if not

			// cout << "Result of RPC (sum of 5 and 7): " << sum << endl;

			all_data = data_array.vectorValueValue();


		} catch (exception const& e) {
			cerr << "Client threw error: " << e.what() << endl;
		} catch (...) {
			cerr << "Client threw unexpected error." << endl;
		}
		

		try {


			xmlrpc_c::value ret_key, result;
			string uri_path;
			uri_path = "http://localhost:"+to_string(process_map[node_name])+"/";

			string const serverUrl(uri_path + node_name);
			cout << "Trying  " << serverUrl << endl;
			string const methodName(node_name+".remove");
			
			xmlrpc_c::clientSimple myClient;
							
			myClient.call(serverUrl, methodName, "s", &result, node_name.c_str());
			
			// ret_key = result;
			// cout << "Success in send" << endl;
		
		
		
		cout << "case other: Sucessfully removed node " << node_name << endl;

		} catch (exception const& e) {
			cerr << "Client threw error: " << e.what() << endl;
		} catch (...) {
			cerr << "Client threw unexpected error." << endl;
		}


		for(int i = 0; i < all_data.size(); i+=3)	{
			xmlrpc_c::value_string const d1(all_data[i]);
			string const key(static_cast<string>(d1));
			xmlrpc_c::value_string const d2(all_data[i+1]);
			string const data(static_cast<string>(d2));

			int ret = put_data(key,data);
			if(ret == -1){
				cout<<"Error in putting data\n";
				return -1;
			}		
		}
	}
	return 0;
}


int GTStoreManager::add_node(string node_name) {
	/**
	 * Return 0 if operation successful
	 * Else return -1
	 */
	
	cout << "Trying to add node " + node_name << endl;
	
	int temp_replication_factor = replication_factor;

	// If node exists, return -1	
	if(ring_reverse.find(node_name) != ring_reverse.end()){
		cout << "Node already exists!" << endl;
		return -1;
	}
	

	if(ring.size() == 0){		
		// Place the first node at 0th location
		ring[0] = node_name;
		ring_reverse[node_name] = 0;

		
		process_map[node_name] = socket_number;
		socket_number++;
		int pid;
		pid = fork();
		if (pid < 0) {
			perror("fork");
			exit(-1);
		}
		if(pid == 0){
			int child_pid = getpid();

			
			GTStoreStorage storage;//= new GTStoreStorage();
			storage.init(node_name, socket_number-1);
			
			exit(0);
			// return 0;
			// kill(child_pid, SIGINT);

		}

		cout << "Successfully added node "+ node_name << endl;
		return 0;
	}
	int node_location;
	

	if(ring.size() == 1){
		if(temp_replication_factor > 1){
			node_location = (rand()%(max_ring_size -2*min_dist))+min_dist;
			ring[node_location] = node_name;
			ring_reverse[node_name] = node_location;
			
			string name_loc = ring[0];

			process_map[node_name] = socket_number;
			socket_number++;		

			int pid;
			pid = fork();
			if (pid < 0) {
				perror("fork");
				exit(-1);
			}
			if(pid == 0){
				int child_pid = getpid();
				
				GTStoreStorage storage;
				storage.init(node_name,socket_number - 1);
				
				exit(0);
			}
			xmlrpc_c::value ret_key;
			
			try {
				// if(debug == 0){
				string uri_path;

				uri_path = "http://localhost:" + to_string(process_map[name_loc]) + "/";
				

				string const serverUrl(uri_path + name_loc);
				
				
				string const methodName(name_loc+".get_all");
				
				xmlrpc_c::clientSimple myClient;
				xmlrpc_c::value result;
				vector<xmlrpc_c::value> all_data;

				myClient.call(serverUrl, methodName, "i", &result, 1);
       

				xmlrpc_c::value_array data_array(result);
				// cout << "Received" << endl;
					// Assume the method returned an integer; throws error if not

				// cout << "Result of RPC (sum of 5 and 7): " << sum << endl;

				all_data = data_array.vectorValueValue();
				// cout << "Success in receive" << endl;


				try {

					for(int i =0; i < all_data.size();i+=3){
						// cout << "iteration: " << i << endl;
						xmlrpc_c::value_string const d1(all_data[i]);
						string const key(static_cast<string>(d1));
						xmlrpc_c::value_string const d2(all_data[i+1]);
						string const data(static_cast<string>(d2));
						xmlrpc_c::value_string const d3(all_data[i+2]);
						string const master_node(static_cast<string>(d3));
						

						xmlrpc_c::value ret_key, result;
						string uri_path;
						uri_path = "http://localhost:"+to_string(process_map[node_name])+"/";

						string const serverUrl(uri_path + node_name);
						// cout << "Trying  " << serverUrl << endl;
						string const methodName(node_name+".put");
						
						xmlrpc_c::clientSimple myClient;
										
						myClient.call(serverUrl, methodName, "sss", &result, key.c_str(), data.c_str(), master_node.c_str());
						
						// ret_key = result;
						// cout << "Success in send" << endl;
					}
					
					
				} catch (exception const& e) {
					cerr << "Client threw error: " << e.what() << endl;
				} catch (...) {
					cerr << "Client threw unexpected error." << endl;
				}



			} catch (exception const& e) {
				cerr << "Client threw error: " << e.what() << endl;
			} catch (...) {
				cerr << "Client threw unexpected error." << endl;
			}

			cout << "Successfully added node "+ node_name << endl;

			return 0;
		}
		// write the else case | when replication factor is 1
	}

	int tries = 0;	
	std::map<int, string>::iterator itlow, ithigh, prev, next, my_iterator;

	while(true){
		tries += 1;
		node_location = (rand()%max_ring_size)+1; 
		bool found = true;
		itlow = ring.lower_bound(node_location);
		
		if(itlow->first == node_location){
			continue;
		}

		if(itlow->first > node_location){
			if(abs(node_location - itlow->first) < min_dist){
				found = false;
			}
		}

		if(itlow->first != ring.begin()->first){
			prev = std::prev(itlow);
			if(abs(node_location - prev->first) < min_dist){
				found = false;
			}	
		}

		if(abs(node_location - max_ring_size) < min_dist){
			found = false;
		}
		
		if(found == true){
			break;
		}
		if(tries > max_ring_size){
			cout<<"Exceeded number of tries to insert the node\n";
			return -1;
		}
	}

	ithigh = ring.end();
	ithigh--;
	// cout << ithigh->first << " ##" << endl;

	if(node_location > ithigh->first){
		prev = ithigh;	
		next = ring.begin();
		// cout << "###############" << endl;		
	}
	else{
		prev = std::prev(itlow);
		next = itlow;

		// cout << "!!!!!!!!!!!!!!!" << endl;
	}

	ring[node_location] = node_name;
	ring_reverse[node_name] = node_location;
	process_map[node_name] = socket_number;
	socket_number++;
	int pid;
	pid = fork();
	if (pid < 0) {
		perror("fork");
		exit(-1);
	}
	if(pid == 0){
		int child_pid = getpid();

		GTStoreStorage storage;//= new GTStoreStorage();
		storage.init(node_name, socket_number-1);
		
		exit(0);
	}
	
	//find common in prev and next and copy it to the new node
	std::map<string, pair<string, string> >::iterator it;
	std::map<int, vector<string> > temp_map;
	std::map<int, vector<string> >::iterator temp_it;
	vector<string> uncommon_keys;

	xmlrpc_c::value ret_key_prev, ret_key_next;
	string prev_node_name = ring[prev->first];

	string next_node_name = ring[next->first];

	
	vector<xmlrpc_c::value> all_data_next;

	try {

		string uri_path;
		uri_path = "http://localhost:"+to_string(process_map[next_node_name])+"/";

		string const serverUrl(uri_path + next_node_name);
		string const methodName(next_node_name+".get_all");
		
		xmlrpc_c::clientSimple myClient;
		xmlrpc_c::value result;
		

		myClient.call(serverUrl, methodName, "i", &result, 1);


		xmlrpc_c::value_array data_array(result);
		// cout << "Received" << endl;

		all_data_next = data_array.vectorValueValue();
		

	} catch (exception const& e) {
		cerr << "Client threw error: " << e.what() << endl;
	} catch (...) {
		cerr << "Client threw unexpected error." << endl;
	}

	vector<xmlrpc_c::value> all_data_prev;

	try {
		string uri_path;
		uri_path = "http://localhost:"+to_string(process_map[prev_node_name])+"/";

		string const serverUrl(uri_path + prev_node_name);
		string const methodName(prev_node_name+".get_all");
		
		xmlrpc_c::clientSimple myClient;
		xmlrpc_c::value result;
		

		myClient.call(serverUrl, methodName, "i", &result, 1);


		xmlrpc_c::value_array data_array(result);
		// cout << "Received" << endl;

		all_data_prev = data_array.vectorValueValue();
		

	} catch (exception const& e) {
		cerr << "Client threw error: " << e.what() << endl;
	} catch (...) {
		cerr << "Client threw unexpected error." << endl;
	}


	// Insert only if common in next_node
	for(int i=0; i< all_data_next.size(); i+=3) {
		
		bool found = false;
		xmlrpc_c::value_string const d1(all_data_next[i]);
		string const str_next_key(static_cast<string>(d1));
		xmlrpc_c::value_string const d2(all_data_next[i+1]);
		string const str_next_data(static_cast<string>(d2));
		xmlrpc_c::value_string const d3(all_data_next[i+2]);
		string const str_next_master(static_cast<string>(d3));

		for(int j = 0; j<all_data_prev.size();j+=3){

			xmlrpc_c::value_string const e1(all_data_next[j]);
			string const str_prev_key(static_cast<string>(e1));
			xmlrpc_c::value_string const e2(all_data_next[j+1]);
			string const str_prev_data(static_cast<string>(e2));
			xmlrpc_c::value_string const e3(all_data_next[j+2]);
			string const str_prev_master(static_cast<string>(e3));
			
			if(str_prev_key == str_next_key){
				//common				

				try {
					xmlrpc_c::value ret_key, result;
									string uri_path;
					uri_path = "http://localhost:"+to_string(process_map[node_name])+"/";

					string const serverUrl(uri_path+node_name);
					string const methodName(node_name+".put");
					
					xmlrpc_c::clientSimple myClient;
									
					myClient.call(serverUrl, methodName, "sss", &result, str_next_key.c_str(), str_next_data.c_str(), str_next_master.c_str());
					
					// ret_key = result;
					
				} catch (exception const& e) {
					cerr << "Client threw error: " << e.what() << endl;
				} catch (...) {
					cerr << "Client threw unexpected error." << endl;
				}

				
				int ret = put_data(str_next_key, str_next_data);
				if(ret == -1){
					return -1;
				}

				// cout << "key of hash:" << str_next_key << endl;
				std::size_t position = std::hash<string>{}(str_next_key);
				position = position % max_ring_size;
				
				if(position < 0){
					position += max_ring_size;
				}
				
				position = find_node(position);

				if(temp_map.find(position)==temp_map.end()){
					vector<string> my_temp_vec;
					my_temp_vec.push_back(str_next_key);
					temp_map[position] = my_temp_vec;
				}
				else{
					temp_map[position].push_back(str_next_key);
				}
				// cout << "Inserting in temp_map " << position << " -> " << str_next_key << endl;
				found = true;
				break;
			}
		}// end of j loop
	


		if (found == false) {
			// uncommon case 1
			// cout << "key of hash:" << str_next_key << endl;
			std::size_t position = std::hash<string>{}(str_next_key);
			position = position % max_ring_size;
			
			if(position < 0){
				position += max_ring_size;
			}

			

			if(node_location > position){

				try {
					xmlrpc_c::value ret_key, result;
					string uri_path;
					uri_path = "http://localhost:"+to_string(process_map[node_name])+"/";

					string const serverUrl(uri_path + node_name);
					string const methodName(node_name+".put");
					
					xmlrpc_c::clientSimple myClient;
									
					myClient.call(serverUrl, methodName, "sss", &result, str_next_key.c_str(), str_next_data.c_str(), node_name.c_str());
					
					// ret_key = result;
					
				} catch (exception const& e) {
					cerr << "Client threw error: " << e.what() << endl;
				} catch (...) {
					cerr << "Client threw unexpected error." << endl;
				}


				
				int ret = put_data(str_next_key, str_next_data);
				if(ret == -1){
					return -1;
				}

				position = find_node(position);
				
				if(temp_map.find(position)==temp_map.end()){
					vector<string> my_temp_vec;
					my_temp_vec.push_back(str_next_key);
					temp_map[position] = my_temp_vec;
				}				
				else{
					temp_map[position].push_back(str_next_key);
				}
				// cout << "Inserting in temp_map " << position << " -> " << str_next_key << endl;
				// cout<<temp_map[position].size()<<"+++\n";
				uncommon_keys.push_back(str_next_key);
			}
		} // end of found==false
	} // end of i loop
		 
	if(temp_map.size() > 0){
		cout << "Printing temp map:" << endl;
		for(temp_it = temp_map.begin(); temp_it != temp_map.end(); temp_it++){
			cout<<"key: "<<temp_it->first << endl;
			int zz = (temp_it->second).size();
			for (int i = 0; i<zz;i++){
				cout<<"The data is: "<< (temp_it->second)[i] <<endl;	
			}
		}
		
		temp_it = temp_map.begin();
		vector<string> curr_node_names;

		int first_pos = 0;
		int i =0;
		for(std::map<int, string>::iterator ring_iterator = ring.begin();ring_iterator != ring.end(); ring_iterator++){
			curr_node_names.push_back(ring_iterator->second);
			if(ring_iterator->second == ring[temp_it->first]){
				first_pos = i;
			}
			i++;
		}
		int curr_pos = first_pos;
		
		// cout<< temp_it->first<<"#++\n";
		
		string first_node_name =  curr_node_names[first_pos];
		string curr_node_name = first_node_name;

		for(int i=0; i<ring.size();i++){
			// cout<<curr_node_names[i]<<" ";
		}
		// cout << endl;
		
		while(temp_it != temp_map.end() && temp_replication_factor < ring.size()){
			curr_node_name = curr_node_names[curr_pos];
			// cout <<"++++++" << curr_node_name << endl;

			if(curr_node_name == ring[temp_it->first]){				
				// cout << (curr_pos + temp_replication_factor) % ring.size() << endl;
				string temp_name = curr_node_names[(curr_pos + temp_replication_factor) % ring.size()];


				
				vector<string> curr_vec = temp_it -> second;
				for(int i =0; i < curr_vec.size(); i++){
					// cout << "Erasing key " << curr_vec[i] << " in node " << temp_name << endl;

					try {
						xmlrpc_c::value ret_key, result;
						string uri_path;
						uri_path = "http://localhost:"+to_string(process_map[temp_name])+"/";

						string const serverUrl(uri_path + temp_name);
						string const methodName(temp_name+".erase");
						
						xmlrpc_c::clientSimple myClient;
										
						myClient.call(serverUrl, methodName, "s", &result, curr_vec[i].c_str());
						
						// ret_key = result;
						
					} catch (exception const& e) {
						cerr << "Client threw error: " << e.what() << endl;
					} catch (...) {
						cerr << "Client threw unexpected error." << endl;
					}

					
				}
				temp_it++;
				curr_pos = (curr_pos+1)%ring.size();
			}
			else{
				curr_pos = (curr_pos+1)%ring.size(); 
				continue;
			}
		}


	}
	
	cout << "Successfully added node "+ node_name << endl;
	// usleep(5000);
	return 0;
}


int GTStoreManager::insert_data(string node_name, string key, string data, string master_node) {
	
	try {
		xmlrpc_c::value ret_key, result;
		string uri_path;
		uri_path = "http://localhost:"+to_string(process_map[node_name])+"/";

		string const serverUrl(uri_path + node_name);
		string const methodName(node_name+".put");
		
		xmlrpc_c::clientSimple myClient;
						
		myClient.call(serverUrl, methodName, "sss", &result, key.c_str(), data.c_str(), master_node.c_str());
		// cout << "Successfully inserted data" << endl;
		return 0;
		
		// ret_key = result;
		
		
	} catch (exception const& e) {
		cerr << "Client threw error: " << e.what() << endl;
	} catch (...) {
		cerr << "Client threw unexpected error." << endl;
	}
	

	return -1;

}


int GTStoreManager::find_node(int key) {

	std::map<int, string>::iterator itlow, ithigh, prev, next;
	bool found = true;
	itlow = ring.lower_bound(key);

	if(ring.size() == 0){
		return -1;
	}

	ithigh = ring.end();
	ithigh--;


	if(key > ithigh->first){
		return ring.begin()->first;
	}

	return itlow->first;
}


int GTStoreManager::pop_data(string node_name, string key, string data) {
	
	cout << "Erasing data: " << data <<" from server: " << node_name << endl;

	try {
		xmlrpc_c::value ret_key, result;
		string uri_path;
		uri_path = "http://localhost:"+to_string(process_map[node_name])+"/";

		string const serverUrl(uri_path + node_name);
		string const methodName(node_name+".pop");
		
		xmlrpc_c::clientSimple myClient;
						
		myClient.call(serverUrl, methodName, "ss", &result, key.c_str(), data.c_str());
		// cout << "Successfully erased data" << endl;
		return 0;
		// ret_key = result;
				
	} catch (exception const& e) {
		cerr << "Client threw error: " << e.what() << endl;
	} catch (...) {
		cerr << "Client threw unexpected error." << endl;
	}
	
	return -1;
}



int GTStoreManager::erase_data(string key, string data) {
	
	// cout << "key of hash in put:" << key << endl;
	size_t position = std::hash<std::string>{}(key);
	position = position % max_ring_size;
	if(position < 0){
		position += max_ring_size;
	}

	int node_position = find_node(position);
	string node = ring[node_position];
	cout << "Inserting data: " << data <<" in server: " << node << " at position " << position << endl;

	std::map<int, string>::iterator it = ring.find(node_position);
	if(it == ring.end()){
		return -1;
	}
	string start_node = node;
	
	if(node != "None"){
		
		// Replication of data
		for(int i = 0; i < replication_factor; i ++){

			int ret = pop_data(node, key, data); //rpc
			if(ret == -1){
				return -1;
			}

			it = std::next(it);
			
			if(it == ring.end()){
				it = ring.begin();
			}

			node = it->second;
			
			if(node == start_node){
				return 0;
			}	
		}
	}
	return 0;
}


int GTStoreManager::put_data(string key, string data) {
	
	// cout << "key of hash in put:" << key << endl;
	size_t position = std::hash<std::string>{}(key);
	position = position % max_ring_size;
	if(position < 0){
		position += max_ring_size;
	}

	int node_position = find_node(position);
	string node = ring[node_position];
	cout << "Inserting data: " << data <<" in server: " << node << " at position " << position << endl;

	std::map<int, string>::iterator it = ring.find(node_position);
	if(it == ring.end()){
		return -1;
	}
	string start_node = node;
	
	if(node != "None"){
		
		// Replication of data
		for(int i = 0; i < replication_factor; i ++){

			int ret = insert_data(node, key, data, start_node); //rpc
			if(ret == -1){
				return -1;
			}

			it = std::next(it);
			
			if(it == ring.end()){
				it = ring.begin();
			}

			node = it->second;
			
			if(node == start_node){
				return 0;
			}	
		}
	}
	return 0;
}


vector<string> GTStoreManager::get_data(string key) {
	
	// cout << "key of hash in put:" << key << endl;
	size_t position = std::hash<std::string>{}(key);
	position = position % max_ring_size;
	if(position < 0){
		position += max_ring_size;
	}

	int node_position = find_node(position);
	string node = ring[node_position];
	cout << "Getting data: from server: " << node << " at position " << position << endl;

	std::map<int, string>::iterator it = ring.find(node_position);
	if(it == ring.end()){
		cerr << "No data found!" << endl;
	}
	string start_node = node;
	vector<string> ret_data;
	
	if(node != "None"){
		try {
			string uri_path;
			uri_path = "http://localhost:"+to_string(process_map[node])+"/";
			string const serverUrl(uri_path + node);
			string const methodName(node+".get");
			
			xmlrpc_c::clientSimple myClient;
			xmlrpc_c::value result;
			vector<xmlrpc_c::value> all_data;

			myClient.call(serverUrl, methodName, "s", &result, key.c_str());
	

			xmlrpc_c::value_array data_array(result);
			// cout << "Received" << endl;
				// Assume the method returned an integer; throws error if not

			// cout << "Result of RPC (sum of 5 and 7): " << sum << endl;

			all_data = data_array.vectorValueValue();
			cout << "Size of all data: " << all_data.size() << endl;
			cout <<"key: " << key<< "; ";
			cout << "Data:" << "[";
			
			for(int i = 0; i < all_data.size();i++){
		

				xmlrpc_c::value_string const d1(all_data[i]);
				string const data(static_cast<string>(d1));

				if(i!=all_data.size() -1 ){cout << data << ", ";}
				else{cout << data << "]";}
				ret_data.push_back(data);
				
				
			}
			cout << endl;

		} catch (exception const& e) {
			cerr << "Client threw error: " << e.what() << endl;
		} catch (...) {
			cerr << "Client threw unexpected error." << endl;
		}
		
	}
	return ret_data;
}

void GTStoreManager::print_nodes() {
	
	cout << "Printing nodes: ";
	map<int, string>::iterator it = ring.begin();
	
	for(it = ring.begin(); it!= ring.end(); ++it){
		cout << (it->first) << " => " << (it->second) << "; ";
		//rpc

		try {
			string uri_path;
			uri_path = "http://localhost:"+to_string(process_map[it->second])+"/";
			string const serverUrl(uri_path + it->second);
			string const methodName(it->second+".get_all");
			
			xmlrpc_c::clientSimple myClient;
			xmlrpc_c::value result;
			vector<xmlrpc_c::value> all_data;

			myClient.call(serverUrl, methodName, "i", &result, 1);
	

			xmlrpc_c::value_array data_array(result);
			// cout << "Received" << endl;
				// Assume the method returned an integer; throws error if not

			// cout << "Result of RPC (sum of 5 and 7): " << sum << endl;

			all_data = data_array.vectorValueValue();
			cout << "Size of all data: " << all_data.size() << endl;
			cout << "Data:" << " ";
			
			for(int i = 0; i < all_data.size();i+=3){
		

				xmlrpc_c::value_string const d1(all_data[i]);
				string const key(static_cast<string>(d1));
				xmlrpc_c::value_string const d2(all_data[i+1]);
				string const data(static_cast<string>(d2));
				xmlrpc_c::value_string const d3(all_data[i+2]);
				string const master_node(static_cast<string>(d3));


				cout << key << " => " << data << " (" << master_node <<  "); ";
			}
			cout << endl;



		} catch (exception const& e) {
			cerr << "Client threw error: " << e.what() << endl;
		} catch (...) {
			cerr << "Client threw unexpected error." << endl;
		}
		
	}
		
}

void test_add(){
	GTStoreManager manager;
	int size = 360;
	manager.init(size);
	manager.add_node("A");
	manager.print_nodes();

	manager.add_node("D");
	manager.print_nodes();

	manager.add_node("E");
	manager.print_nodes();

	manager.add_node("K");
	manager.print_nodes();	
			
	manager.put_data("1","abc");
	manager.print_nodes();
	
	manager.put_data("2","zas");
	manager.print_nodes();
	
	manager.add_node("C");
	manager.print_nodes();
	
	manager.put_data("3","ztas");
	manager.print_nodes();
	
	manager.put_data("4","zgas");
	manager.print_nodes();


	manager.add_node("B");
	manager.print_nodes();

	manager.add_node("G");
	manager.print_nodes();

	manager.put_data("6","abcd");
	manager.print_nodes();

	manager.add_node("F");
	manager.print_nodes();

	manager.put_data("5","nzih" );
	manager.print_nodes();

	manager.add_node("H");
	manager.print_nodes();

	manager.put_data("7","jai");
	manager.print_nodes();

	manager.put_data("8","man");
	manager.print_nodes();

	manager.put_data("9","khu");
	manager.print_nodes();

	manager.print_nodes();
}

void test_remove(){
	cout<<"Started testing remove node\n";
	GTStoreManager manager;
	manager.init();
	int ret;
	string node_to_del;
	manager.add_node("A");
	manager.add_node("B");
	manager.add_node("C");
	manager.add_node("D");
	cout<<"Showing all nodes\n";
	manager.print_nodes();

	manager.put_data("1","abc");
	manager.print_nodes();
	
	manager.put_data("2","zas");
	manager.print_nodes();
	
	manager.add_node("E");
	manager.print_nodes();
	
	manager.put_data("3","ztas");
	manager.print_nodes();
	
	manager.put_data("4","zgas");
	manager.print_nodes();


	node_to_del = "Z";
	cout<<"Trying to delete node: "<<node_to_del<<"\n";
	ret = manager.remove_node(node_to_del);
	cout<<"Status of remove is: "<<ret<<"\n";

	node_to_del = "C";
	cout<<"Trying to delete node: "<<node_to_del<<"\n";
	cout<<"Nodes before remove\n";
	manager.print_nodes();
	ret = manager.remove_node(node_to_del);
	cout<<"Status of remove is: "<<ret<<"\n";
	cout<<"Nodes after remove\n";	
	manager.print_nodes();
	
}

void test_remove_node(){
	cout<<"Started testing remove node\n";
	GTStoreManager manager;
	manager.init();
	int ret;
	string node_to_del;
	manager.add_node("A");
	manager.add_node("B");
	manager.add_node("C");
	cout<<"Showing all nodes\n";
	manager.print_nodes();

	manager.put_data("1","abc");
	manager.print_nodes();
	
	manager.put_data("2","zas");
	manager.print_nodes();
	
	manager.put_data("3","ztas");
	manager.print_nodes();
	
	node_to_del = "A";
	cout<<"Trying to delete node: "<<node_to_del<<"\n";
	cout<<"Nodes before remove\n";
	manager.print_nodes();
	ret = manager.remove_node(node_to_del);
	cout<<"Status of remove is: "<<ret<<"\n";
	cout<<"Nodes after remove\n";	
	manager.print_nodes();
	
}

void test_add_remove(){
	// test_add();
	int pid;
	// pid = fork();
	// if (pid < 0) {
	// 	perror("fork");
	// 	exit(-1);
	// }
	// if(pid == 0){
	// 	int child_pid = getpid();
	// 	GTStoreStorage storage;//= new GTStoreStorage();
	// 	storage.init("A");
	// 	// process_map[node_name] = &storage;

	// }

	GTStoreManager manager;
	int size = 360;
	manager.init(size);
	usleep(2000);
	manager.add_node("A");
	usleep(2000);

	
	manager.put_data("1","abc");
	usleep(2000);

	manager.print_nodes();
	usleep(2000);

	manager.add_node("B");
	usleep(2000);

	manager.get_data("1");
	usleep(2000);

	manager.put_data("1","def");
	usleep(2000);

	manager.get_data("1");
	usleep(2000);

	manager.erase_data("1","abc");
	usleep(2000);


	manager.get_data("1");
	usleep(2000);


	manager.print_nodes();
	usleep(2000);

	manager.add_node("C");
	usleep(2000);


	manager.print_nodes();
	usleep(2000);

	manager.put_data("2","ghi");
	usleep(2000);


	manager.print_nodes();
	usleep(2000);

	manager.add_node("D");
	usleep(2000);

	manager.print_nodes();
	usleep(2000);

	manager.put_data("2","jkl");
	usleep(2000);

	manager.print_nodes();
	usleep(2000);

	// manager.insert_data("A","2","def","A");

	// manager.insert_data("A","3","ghi","A");
	// manager.print_nodes();
	// usleep(2000);

	manager.remove_node("A");
	usleep(2000);

	manager.print_nodes();
	usleep(2000);
	
	manager.remove_node("B");
	usleep(2000);


	manager.print_nodes();
	usleep(2000);

	manager.remove_node("C");
	usleep(2000);


	manager.print_nodes();
	usleep(2000);
	
	manager.remove_node("D");
	usleep(2000);

	// return 0;
}

int main(int argc, char **argv) {
	// test_add_remove();
	GTStoreManager manager;
	int size = 360;
	manager.init(size);
	usleep(2000);

	manager.add_node("A");
	usleep(2000);

	manager.add_node("B");
	usleep(2000);

	manager.add_node("C");
	usleep(2000);

	manager.add_node("D");
	usleep(2000);


	manager.add_node("E");
	usleep(2000);


	manager.add_node("F");
	usleep(2000);



	try {
        
		xmlrpc_c::registry myRegistry;
		if(argc == 2){
			xmlrpc_c::methodPtr const managerRemovePutMethodP(new managerRemovePutMethod(&manager));
       		myRegistry.addMethod("manager.put", managerRemovePutMethodP);

		}
		else{
        	xmlrpc_c::methodPtr const managerPutMethodP(new managerPutMethod(&manager));
        	myRegistry.addMethod("manager.put", managerPutMethodP);
		}
		xmlrpc_c::methodPtr const managerGetMethodP(new managerGetMethod(&manager));
        myRegistry.addMethod("manager.get", managerGetMethodP);

		xmlrpc_c::methodPtr const managerInsertMethodP(new managerInsertMethod(&manager));
        myRegistry.addMethod("manager.insert", managerInsertMethodP);

		xmlrpc_c::methodPtr const managerAddMethodP(new managerAddMethod(&manager));
        myRegistry.addMethod("manager.add", managerAddMethodP);

		xmlrpc_c::methodPtr const managerRemoveMethodP(new managerRemoveMethod(&manager));
        myRegistry.addMethod("manager.remove", managerRemoveMethodP);

		xmlrpc_c::methodPtr const managerEraseMethodP(new managerEraseMethod(&manager));
        myRegistry.addMethod("manager.erase", managerEraseMethodP);

    
        // cout << "Trying to start storage server" << endl;
        // cout << "Socket name:" << name << " number:" << socket_number << endl;
		// myRegistry.addMethod("client.put", clientPutMethodP);
        xmlrpc_c::serverAbyss myAbyssServer(
        xmlrpc_c::serverAbyss::constrOpt()
        .registryP(&myRegistry)
        .uriPath("/manager")
        .portNumber(7000));
        // my_server = &myAbyssServer; 
        myAbyssServer.run(); 
        
        // xmlrpc_c::serverAbyss.run() never returns
        // assert(false);
    } catch (exception const& e) {
        cerr << "Something failed.  " << e.what() << endl;
    }



	exit(0);

	// cout<<"Test remove #############\n";
	// test_remove_node0();
	// test_rpc();
	return 0;
}

// int main(int const, const char ** const) {
	
// }
