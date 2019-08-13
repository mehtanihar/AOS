#include "gtstore.hpp"

#include <cstdlib>
#include <string>
#include <iostream>

#include <xmlrpc-c/girerr.hpp>
#include <xmlrpc-c/base.hpp>
#include <xmlrpc-c/client_simple.hpp>


void GTStoreClient::init(int id) {
	
	cout << "Inside GTStoreClient::init() for client " << id << "\n";
	client_id = id;
}

val_t GTStoreClient::get(string key) {
	
	// cout << "Inside GTStoreClient::get() for client: " << client_id << " key: " << key << "\n";
	val_t value;
	// Get the value!
    try {
        
        cout << "Client " << client_id << " get: key: " << key  << "\n";
        

        string const serverUrl("http://localhost:7000/manager");
        string const methodName("manager.get");

        

        xmlrpc_c::clientSimple myClient;
        xmlrpc_c::value result;
        
        myClient.call(serverUrl, methodName, "s", &result, key.c_str());

        vector<xmlrpc_c::value> all_data;
        xmlrpc_c::value_array data_array(result); 
        all_data = data_array.vectorValueValue();
        
        cout << "Get Data sucessful" << endl;
        cout <<"Data received: "; 
        for(int i = 0; i<all_data.size();i++){
            xmlrpc_c::value_string const d1(all_data[i]);
            string const data(static_cast<string>(d1));
            value.push_back(data);
            cout << data << " ";
        }
        cout << endl;

    } catch (exception const& e) {
        cerr << "Client threw error: " << e.what() << endl;
    } catch (...) {
        cerr << "Client threw unexpected error." << endl;
    }

	return value;
}

bool GTStoreClient::put(string key, val_t value) {

	string print_value = "";
	for (uint i = 0; i < value.size(); i++) {
		print_value += value[i] + " ";
	}
	// Put the value!
    for(int i = 0; i < value.size();i++){
        try {


	        cout << "Client " << client_id << " put: key: " << key << "; value: " << value[i] << "\n";
        
            string const serverUrl("http://localhost:7000/manager");
            string const methodName("manager.put");
            
            xmlrpc_c::clientSimple myClient;
            xmlrpc_c::value result;
            
            myClient.call(serverUrl, methodName, "ss", &result, key.c_str(), value[i].c_str());

            string const ret_key = xmlrpc_c::value_string(result);
                // Assume the method returned an integer; throws error if not

            // cout << "Receive back the key to verify " << ret_key << endl;
            if(ret_key == key){
                cout << "Put data successful" << endl;
            }

        } catch (exception const& e) {
            cerr << "Client threw error: " << e.what() << endl;
        } catch (...) {
            cerr << "Client threw unexpected error." << endl;
        }
    }

	return true;
}


bool GTStoreClient::erase(string key, string value) {

	// string print_value = "";
	// for (uint i = 0; i < value.size(); i++) {
	// 	print_value += value[i] + " ";
	// }
	// Put the value!
    // for(int i = 0; i < value.size();i++){
    try {

        cout << "Client " << client_id << " erase: key: " << key << "; value: " << value << "\n";
    
        string const serverUrl("http://localhost:7000/manager");
        string const methodName("manager.erase");
        
        xmlrpc_c::clientSimple myClient;
        xmlrpc_c::value result;
        
        myClient.call(serverUrl, methodName, "ss", &result, key.c_str(), value.c_str());

        string const ret_key = xmlrpc_c::value_string(result);
            // Assume the method returned an integer; throws error if not

        // cout << "Receive back the key to verify " << ret_key << endl;
        if(ret_key == key){
            cout << "Erasing data successful" << endl;
        }

    } catch (exception const& e) {
        cerr << "Client threw error: " << e.what() << endl;
    } catch (...) {
        cerr << "Client threw unexpected error." << endl;
    }

    // }

	return true;
}


void GTStoreClient::finalize() {
	
	// cout << "Inside GTStoreClient::finalize() for client " << client_id << "\n";
}


// int main() {

//     return 0;
// }
