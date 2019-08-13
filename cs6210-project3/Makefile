CFLAGS  =
LFLAGS  =
CC      = g++
RM      = /bin/rm -rf
AR      = ar rc
RANLIB  = ranlib

LIBRARY = libgtfs.a

LIB_SRC = gtfs.cpp

LIB_OBJ = $(patsubst %.cpp,%.o,$(LIB_SRC))

# pattern rule for object files
%.o: %.cpp
	$(CC) -c $(CFLAGS) $< -o $@

all: $(LIBRARY) 

$(LIBRARY): $(LIB_OBJ)
	$(AR) $(LIBRARY) $(LIB_OBJ)
	$(RANLIB) $(LIBRARY)

$(LIB_OBJ) : $(LIB_SRC)
	$(CC) -c $(CFLAGS) $< -o $@

clean:
	$(RM) $(LIBRARY) *.o
