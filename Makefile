CC=g++
CFLAGS=-Wall -g -Wextra -Werror -std=c++17

build: client

client: client.cpp helpers.cpp requests.cpp jsonify.cpp
	$(CC) $(CFLAGS) -o client client.cpp helpers.cpp requests.cpp jsonify.cpp

run: client
	./client

clean:
	rm -f *.o client
