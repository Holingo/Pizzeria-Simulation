CC = gcc
CFLAGS = -Wall -pthread
LDFLAGS = -lncurses

OBJS = main.o client.o cashier.o firefighter.o utilities.o

pizzeria: $(OBJS)
	$(CC) $(CFLAGS) $(OBJS) $(LDFLAGS) -o pizzeria

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f *.o pizzeria