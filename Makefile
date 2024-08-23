CC = gcc
BROKER = broker
PUB = pub
SUB = sub
BROKER_SRC = ./src/broker.c
PUB_SRC = ./src/pub.c
SUB_SRC = ./src/sub.c
SOCKET_SRC = ./src/lib/socket.c
QUEUE_SRC = ./src/lib/queue.c

# default target
all: $(BROKER) $(PUB) $(SUB)

# build broker bin
$(BROKER): $(BROKER_SRC) $(SOCKET_SRC) $(QUEUE_SRC)
	$(CC) $^ -o $@

# build publisher bin
$(PUB): $(PUB_SRC) $(SOCKET_SRC)
	$(CC) $^ -o $@

# build subscriber bin
$(SUB): $(SUB_SRC) $(SOCKET_SRC)
	$(CC) $^ -o $@

clean:
	rm $(BROKER) $(PUB) $(SUB)
