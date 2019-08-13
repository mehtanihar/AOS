CFLAGS  =
LFLAGS  =
CC      = g++
RM      = /bin/rm -rf

LIBRARY = ../libgtfs.a

TESTS = test

all: $(TESTS)

test : test.cpp
	$(CC) -Wall test.cpp $(LIBRARY) -o test

clean:
	$(RM) *.o $(TESTS)
