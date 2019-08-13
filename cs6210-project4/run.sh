#/bin/bash

make clean
make

# Launch the GTStore Manager
./manager &
sleep 5

# Launch couple GTStore Storage Nodes
# ./storage &
# sleep 5
# ./storage &
# sleep 5

# Launch the client testing app
# Usage: ./test_app <test> <client_id>
./test_app single_set_get 1 &
./test_app single_set_get 2 &
./test_app single_set_get 3 
killall -9 manager
