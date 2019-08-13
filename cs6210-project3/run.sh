#/bin/bash

DO_VERBOSE=$1

make clean
make

cd tests
make clean
make

if [  $# -le 0 ] 
then 
  echo "Usage:"
  echo "./run.sh 1 -- for DEBUG"
  echo "./run_sh 0 -- for testing"
  exit 1
fi 

./test $DO_VERBOSE
