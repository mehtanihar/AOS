#include "gtstore.hpp"
#include <chrono>
#define WIN32_LEAN_AND_MEAN  /* required by xmlrpc-c/server_abyss.hpp */

#include <cassert>
#include <stdexcept>
#include <iostream>
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


class clientRemoveMethod : public xmlrpc_c::method {
public:
	GTStoreStorage* my_storage ;//= new GTStoreStorage();
    clientRemoveMethod(GTStoreStorage* storage) {
        this->_signature = "i:s";
        this->_help = "This method adds two integers together";
		my_storage = storage;
    }
    void execute(xmlrpc_c::paramList const& paramList,
            xmlrpc_c::value *   const  retvalP) {
        
        (*(*my_storage).my_server).terminate();

        *retvalP = xmlrpc_c::value_int(0);
        // usleep(5000);
        // exit(0);
        // free(my_storage);
        
    }
};


class clientPopMethod : public xmlrpc_c::method {
public:
	GTStoreStorage* my_storage ;//= new GTStoreStorage();
    clientPopMethod(GTStoreStorage* storage) {
        this->_signature = "i:ss";
        this->_help = "This method adds two integers together";
		my_storage = storage;
    }
    void execute(xmlrpc_c::paramList const& paramList,
            xmlrpc_c::value *   const  retvalP) {

        // cout<<"In execute of clientPutMethod" << endl;

        string const key(paramList.getString(0));
		string const data(paramList.getString(1));
	
        vector<string>::iterator it = find ((*my_storage).node_data[key].begin(), (*my_storage).node_data[key].end(),data);
        if(it != (*my_storage).node_data[key].end()){
             (*my_storage).node_data[key].erase(it);
        }
        else{
            cout <<" Cannot erase.. No data exists!" << endl;
        }
		//make_pair(data,master_node);
        // (*my_storage).master_node_map[key] = master_node;

        // cout<<"The received str is : "<< key <<" data : "<< data << " master :"<< master_node <<"\n";
		// paramList.verifyEnd(3);
        *retvalP = xmlrpc_c::value_int(0);
        
    }
};


class clientPutMethod : public xmlrpc_c::method {
public:
	GTStoreStorage* my_storage ;//= new GTStoreStorage();
    clientPutMethod(GTStoreStorage* storage) {
        this->_signature = "i:sss";
        this->_help = "This method adds two integers together";
		my_storage = storage;
    }
    void execute(xmlrpc_c::paramList const& paramList,
            xmlrpc_c::value *   const  retvalP) {

        // cout<<"In execute of clientPutMethod" << endl;

        string const key(paramList.getString(0));
		string const data(paramList.getString(1));
		string const master_node(paramList.getString(2));
	
        vector<string>::iterator it = find ((*my_storage).node_data[key].begin(), (*my_storage).node_data[key].end(),data);
        if(it == (*my_storage).node_data[key].end()){
            (*my_storage).node_data[key].push_back(data); 
        }
        else{
             *it = data;
        }
		//make_pair(data,master_node);
        (*my_storage).master_node_map[key] = master_node;

        // cout<<"The received str is : "<< key <<" data : "<< data << " master :"<< master_node <<"\n";
		paramList.verifyEnd(3);
        *retvalP = xmlrpc_c::value_int(0);
        
    }
};


class clientEraseMethod : public xmlrpc_c::method {
public:
	GTStoreStorage* my_storage;
    clientEraseMethod(GTStoreStorage* storage) {
        this->_signature = "i:s";
        this->_help = "This method adds two integers together";
		my_storage = storage;
    }
    void execute(xmlrpc_c::paramList const& paramList,
            xmlrpc_c::value *   const  retvalP) {
        		
        string const key(paramList.getString(0));
	
		
		(*my_storage).node_data.erase(key);
        (*my_storage).master_node_map.erase(key);

        // cout<<"The received str is : "<< key <<"data : "<<"\n";
		paramList.verifyEnd(1);
        *retvalP = xmlrpc_c::value_int(0);
    }
};

class clientGetMethod : public xmlrpc_c::method {
public:
	GTStoreStorage* my_storage;
    clientGetMethod(GTStoreStorage* storage) {
        this->_signature = "A:s";
        this->_help = "Get function";
		my_storage = storage;
    }
    void execute(xmlrpc_c::paramList const& paramList,
            xmlrpc_c::value *   const  retvalP) {    		
        string const key(paramList.getString(0));

		paramList.verifyEnd(1);
        vector<xmlrpc_c::value> all_data;
        for(int i = 0; i < (*my_storage).node_data[key].size();i++){
            all_data.push_back(xmlrpc_c::value_string((*my_storage).node_data[key][i]));
        }
        *retvalP = xmlrpc_c::value_array(all_data);
    }
};


class clientGetAllMethod : public xmlrpc_c::method {
public:
	GTStoreStorage* my_storage;
    clientGetAllMethod(GTStoreStorage* storage) {
		string my_sign = "A:i";
        this->_signature = my_sign;
        this->_help = "Get function";
		my_storage = storage;
    }
    void execute(xmlrpc_c::paramList const& paramList,
        xmlrpc_c::value *   const  retvalP) {    		

			int count = 0;
            vector<xmlrpc_c::value> all_data;


			for(map<string, vector<string> >::iterator it = (*my_storage).node_data.begin();it!=(*my_storage).node_data.end(); it++){
				

                for(int i = 0; i < (it->second).size();i++){
                    all_data.push_back(xmlrpc_c::value_string(it->first));
                    all_data.push_back(xmlrpc_c::value_string((it->second)[i]));
                    all_data.push_back(xmlrpc_c::value_string(my_storage->master_node_map[it->first]));
                }                

            }

            *retvalP = xmlrpc_c::value_array(all_data);
            
    }
};


void GTStoreStorage::init(string name, int socket) {
	// string name = "A";
	// cout << "Inside GTStoreStorage::init()\n";
	node_name = name;
    socket_number = socket;
    // usleep(5000);
    // vector<xmlrpc_c::value> all_data;
    // all_data.push_back(xmlrpc_c::value_string("1"));
    // all_data.push_back(xmlrpc_c::value_string("nzih"));
    // all_data.push_back(xmlrpc_c::value_string("A"));
    // all_data.push_back(xmlrpc_c::value_string("A"));



    // try {
    //     // for(int i = 0; i < all_data.size(); i+=3 ){
    //     string const serverUrl("http://localhost:8080/RPC2");
    //     string const methodName("manager.get_all_response");
        
    //     xmlrpc_c::clientSimple myClient;
    //     xmlrpc_c::value result;
        
    //     myClient.call(serverUrl, methodName, "A", &result, all_data);
        
    //         // ret_key_next = result;
    //     // }
        
    // } catch (exception const& e) {
    //     cerr << "Client threw error: " << e.what() << endl;
    // } catch (...) {
    //     cerr << "Client threw unexpected error." << endl;
    // }

	try {
        xmlrpc_c::registry myRegistry;
        xmlrpc_c::methodPtr const clientPutMethodP(new clientPutMethod(this));
        myRegistry.addMethod(name +".put", clientPutMethodP);

        xmlrpc_c::methodPtr const clientPopMethodP(new clientPopMethod(this));
		myRegistry.addMethod(name +".pop", clientPopMethodP);

        xmlrpc_c::methodPtr const clientEraseMethodP(new clientEraseMethod(this));
        myRegistry.addMethod(name +".erase", clientEraseMethodP);

		xmlrpc_c::methodPtr const clientGetMethodP(new clientGetMethod(this));
		myRegistry.addMethod(name +".get", clientGetMethodP);

		xmlrpc_c::methodPtr const clientGetAllMethodP(new clientGetAllMethod(this));
		myRegistry.addMethod(name +".get_all", clientGetAllMethodP);

        xmlrpc_c::methodPtr const clientRemoveMethodP(new clientRemoveMethod(this));
		myRegistry.addMethod(name +".remove", clientRemoveMethodP);

        cout << "Trying to start storage server" << endl;
        cout << "Socket name:" << name << " number:" << socket_number << endl;
		// myRegistry.addMethod("client.put", clientPutMethodP);
        xmlrpc_c::serverAbyss myAbyssServer(
        xmlrpc_c::serverAbyss::constrOpt()
        .registryP(&myRegistry)
        .uriPath(("/" + name).c_str())
        .portNumber(socket_number));
        my_server = &myAbyssServer; 
        myAbyssServer.run(); 
        
        // xmlrpc_c::serverAbyss.run() never returns
        // assert(false);
    } catch (exception const& e) {
        cerr << "Something failed.  " << e.what() << endl;
    }

}


void GTStoreStorage::put(){

    cout << "abc" << endl;
}

// int main(int argc, char **argv) {

// 	GTStoreStorage storage;
// 	// storage.init();
	
// }
