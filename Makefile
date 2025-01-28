CC = gcc
CFLAGS = -Wall -Wextra
TARGETS = main cashier customer firefighter
DEPS = common.h

all: $(TARGETS)

# Główne programy
main: main.c $(DEPS)
	$(CC) $(CFLAGS) $< -o $@

cashier: cashier.c $(DEPS)
	$(CC) $(CFLAGS) $< -o $@

customer: customer.c $(DEPS)
	$(CC) $(CFLAGS) $< -o $@

firefighter: firefighter.c $(DEPS)
	$(CC) $(CFLAGS) $< -o $@

# Czyszczenie
clean:
	rm -f $(TARGETS)

.PHONY: all clean