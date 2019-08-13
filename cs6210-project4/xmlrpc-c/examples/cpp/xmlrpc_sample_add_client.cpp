#include <cstdlib>
#include <string>
#include <iostream>

using namespace std;

#include <xmlrpc-c/girerr.hpp>
#include <xmlrpc-c/base.hpp>
#include <xmlrpc-c/client_simple.hpp>

int main(int argc, char **) {

    if (argc-1 > 0) {
        cerr << "This program has no arguments" << endl;
        exit(1);
    }

    try {
        string const serverUrl("http://localhost:8080/RPC2");
        string const methodName("sample.add");

        xmlrpc_c::clientSimple myClient;
        xmlrpc_c::value result;
        vector<xmlrpc_c::value> all_data;
        
        
        // all_data.push_back(xmlrpc_c::value_string("1"));
        // all_data.push_back(xmlrpc_c::value_string("nzih"));

        cout << "Sending" << endl;
        // xmlrpc_c::value_array data_array(all_data);
        myClient.call(serverUrl, methodName, "i", &result, 1);
       

        xmlrpc_c::value_array data_array(result);
        cout << "Received" << endl;
            // Assume the method returned an integer; throws error if not

        // cout << "Result of RPC (sum of 5 and 7): " << sum << endl;

        all_data = data_array.vectorValueValue();
        //  cout << "Conv" << endl;
        xmlrpc_c::value_string const k_temp_final(all_data[0]);
		string const node(static_cast<string>(k_temp_final));


        xmlrpc_c::value_string const k_temp_final_1(all_data[1]);
		string const node1(static_cast<string>(k_temp_final_1));

        cout << "Result of RPC" << node << " " << node1 << endl;


    } catch (exception const& e) {
        cerr << "Client threw error: " << e.what() << endl;
    } catch (...) {
        cerr << "Client threw unexpected error." << endl;
    }

    return 0;
}