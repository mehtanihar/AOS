#ifndef GTSTORE
#define GTSTORE

#include <ctime> 
#include <random>
#include  <sys/types.h>
#include  <stdio.h>
#include <string>
#include <cstdio>
#include <cstdlib>
#include <iostream>
#include <vector>
#include <unistd.h>
#include <sys/wait.h>
#include <map>
#include <string>
#include <utility> 
#include <cassert>
#include <stdexcept>
#include <string>
#include <algorithm>

#include <xmlrpc-c/base.hpp>
#include <xmlrpc-c/registry.hpp>
#include <xmlrpc-c/server_abyss.hpp>
#include <xmlrpc-c/girerr.hpp>
#include <xmlrpc-c/client_simple.hpp>

using namespace std;

typedef vector<string> val_t;


class GTStoreStorage {
public:
	void init();
	void init(string name, int socket_number);
	void put();
	string node_name;
	int socket_number;
	map<string, vector<string> > node_data;
	map<string,string> master_node_map;
	void init(string name, string node, string data);
	xmlrpc_c::serverAbyss * my_server;
};

class GTStoreClient {
private:
	int client_id;
	val_t value;
public:
	void init(int id);
	void finalize();
	val_t get(string key);
	bool put(string key, val_t value);
	bool erase(string key, string value);
};

class GTStoreManager {

public:
	vector<string> prev_rpc_node_data, next_rpc_node_data;
	string prev_rpc_node_name, next_rpc_node_name;
	int max_ring_size;
	int min_dist;
	int replication_factor;
	map<int,string> ring;
	map<string,int> ring_reverse;
	map<string, int> process_map;
	bool wait_flag;
	int socket_number;
	
	void init();	
	void init(int max_ring_size);
	int add_node(string node_name);
	int remove_node(string node_name);
	void print_nodes();
	int insert_data(string node_name, string key, string data, string master_server);
	int remove_data(string node_name, string key);
	int find_node(int key);
	int put_data(string key, string data);
	void set_wait_flag(bool val);
	vector<string> get_data(string key);
	int erase_data(string key, string data);
	int pop_data(string node_name, string key, string data);
	
};


#endif