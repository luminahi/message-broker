
all:
	gcc ./src/client.c ./src/lib/queue.c ./src/lib/socket.c -o client
	gcc ./src/broker.c ./src/lib/socket.c ./src/lib/queue.c ./src/lib/linked_list.c -o broker

clean:
	rm client broker