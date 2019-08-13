CC = gcc
CFLAGS = -g
LDFLAGS = 
LIBS = .
SRC = src/gt_kthread.c src/gt_uthread.c src/gt_pq.c src/gt_signal.c src/gt_spinlock.c
OBJ = $(SRC:.c=.o)

OUT = bin/libuthread.a

.c.o:
	$(CC) $(CFLAGS) -c $< -o $@

$(OUT): $(OBJ)
	ar rcs $(OUT) $(OBJ)

matrix:
	$(CC) $(CFLAGS) src/gt_matrix.c $(OUT) -o bin/matrix -lm

#all : gt_include.h gt_kthread.c gt_kthread.h gt_uthread.c gt_uthread.h gt_pq.c gt_pq.h gt_signal.h gt_signal.c gt_spinlock.h gt_spinlock.c gt_matrix.c
#	@echo Building...
#	@gcc -o matrix gt_matrix.c gt_kthread.c gt_pq.c gt_signal.c gt_spinlock.c gt_uthread.c
#	@echo Done!
#	@echo Now run './matrix'


clean :
	@rm src/*.o bin/*.a bin/matrix
	@echo Cleaned!
