CFLAGS=-Wall -g
all: llast
llast: llast.o time_read.o
	$(CC) $(LDFLAGS) -o llast llast.o time_read.o

clean:
	rm llast time_read.o llast.o
