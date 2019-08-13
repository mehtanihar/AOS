#define WIN32_LEAN_AND_MEAN  /* required by xmlrpc-c/server_abyss.hpp */

#include <cassert>
#include <stdexcept>
#include <iostream>
#ifdef _WIN32
#  include <windows.h>
#else
#  include <unistd.h>
#endif

using namespace std;

#include <xmlrpc-c/base.hpp>
#include <xmlrpc-c/registry.hpp>
#include <xmlrpc-c/server_abyss.hpp>

#ifdef _WIN32
  #define SLEEP(seconds) SleepEx(seconds * 1000);
#else
  #define SLEEP(seconds) sleep(seconds);
#endif


class sampleAddMethod : public xmlrpc_c::method {
public:
    sampleAddMethod() {
        // signature and help strings are documentation -- the client
        // can query this information with a system.methodSignature and
        // system.methodHelp RPC.
        this->_signature = "A:i";
            // method's result and two arguments are integers
        this->_help = "This method adds two integers together";
    }
    void execute(xmlrpc_c::paramList const& paramList,
		xmlrpc_c::value *   const  retvalP) {

        std::vector<xmlrpc_c::value> arr = paramList.getArray(0);
        
        
        cout << "In execute" << endl;
        // std::vector<xmlrpc_c::value> keys(paramList.getArray(0));
        vector<xmlrpc_c::value> nodes;
        
        // int const addend(paramList.getInt(0));
        // int const adder(paramList.getInt(1));
        nodes.push_back(xmlrpc_c::value_string("1"));

        nodes.push_back(xmlrpc_c::value_string("2"));
        // paramList.verifyEnd(1);
        
        *retvalP = xmlrpc_c::value_array(nodes);

        // Sometimes, make it look hard (so client can see what it's like
        // to do an RPC that takes a while).
        // if (adder == 1)
        //     SLEEP(2);
    }
};



int 
main(int           const, 
     const char ** const) {

    try {
        xmlrpc_c::registry myRegistry;

        xmlrpc_c::methodPtr const sampleAddMethodP(new sampleAddMethod);

        myRegistry.addMethod("sample.add", sampleAddMethodP);
        
        xmlrpc_c::serverAbyss myAbyssServer(
            xmlrpc_c::serverAbyss::constrOpt()
            .registryP(&myRegistry)
            .portNumber(8080));
        
        myAbyssServer.run();
        // xmlrpc_c::serverAbyss.run() never returns
        assert(false);
    } catch (exception const& e) {
        cerr << "Something failed.  " << e.what() << endl;
    }
    return 0;
}