# GRR20206147 Rubens Zandomenighi Laszlo 

DEBUG_FLAGS = -g -DDEBUG
CFLAGS = -lm -lpthread -O3

.PHONY : all debug clean purge

all : findKLeast

findKLeast: max-heap.o chrono.o findKLeast.o
	gcc -o $@ $^ $(CFLAGS)

max-heap.o : max-heap.c
	gcc $(CFLAGS) -c $^

chrono.o : chrono.c
	gcc $(CFLAGS) -c $^
	
debug : CFLAGS += $(DEBUG_FLAGS)
debug : all

clean :
	$(RM) *.o

purge : clean
	$(RM) findKLeast 