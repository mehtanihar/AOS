To compile the project:
Go to snappy-c directory and run:
make clean && make 

Synchronous communication:
```
./app <input_file_folder> <input_files> num_segments <num_segments> segment_size <segment_size> num_threads <num_threads> is_sync 1 is_qos 0
```
```
./app . sample.txt num_segments 500 segment_size 4 num_threads 1 is_sync 1 is_qos 0
```

Asynchronous communication:
```
./app <input_file_folder> <input_files> num_segments <num_segments> segment_size <segment_size> num_threads <num_threads> is_sync 0
```
```
./app . sample.txt sample1.txt num_segments 500 segment_size 4 num_threads 2 is_sync 0
```
QoS Synchronous communication:
```
./app <input_file_folder> <input_files> num_segments <num_segments> segment_size <segment_size> num_threads <num_threads> is_sync 1 is_qos 1
```
```
./app . sample.txt num_segments 500 segment_size 4 num_threads 1 is_sync 1 is_qos 1
```
