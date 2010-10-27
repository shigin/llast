CFLAGS=-Wall -g
all: llast test_read
llast: llast.o time_read.o
	$(CC) $(LDFLAGS) -o llast llast.o time_read.o

test_read: test_read.o time_read.o
	$(CC) $(LDFLAGS) -o test_read test_read.o time_read.o

test: llast
	./test_read

clean:
	rm test_read llast test_read.o time_read.o llast.o
