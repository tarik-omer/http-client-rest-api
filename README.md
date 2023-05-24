Copyright 2022 - 2023 Omer Tarik Ilhan 324CA
# HTTP CLIENT with REST API

## Description

This project is a HTTP client that uses the REST API to communicate with
a server. It is a CLI application that takes commands from the user and
sends them to the server. The server responds with the result of the
command. The connection is opened and closed for every command. The
only command that does not require a connection is the 'exit' command.

## Implementation

The client is implemented in C++ and uses the sockets API to communicate
with the server.

The client uses the nlohmann json library to parse the JSON response from
the server and to create the JSON request to the server. 

The client uses a stringstream to parse the response from the server.

In order to be more readable and flexible, we use a map to store the
commands and their corresponding 'names'. This way, we can easily add
new commands. We also use a map to store the URLs for the commands.

The flow of the program is:

- init map with commands and URLs
- init socket
- enter _while_ loop until **'exit'** command is received
    - read command from user
    - parse command
    - create request
    - open connection
    - send request
    - receive response
    - close connection
    - parse response
    - print result

## Commands

The commands are:

- 'login' - login to the server; expects username and password;
receives a session cookie
- 'logout' - logout from the server
- 'register' - register a new user; expects username and password
- 'enter_library' - enter the library; receives a token
- 'get_books' - get all books from the library; receives a JSON array
of books
- 'get_book' - get a book from the library; expects book id; receives a
JSON of the requested book
- 'add_book' - add a book to the library; expects book title, author and
genre, number of pages, publisher
- 'delete_book' - delete a book from the library; expects book id
- 'exit' - exit the application

## Compile

```
make
```

## Run

```
./client
```

or

```
make run
```

## Bibliography

- [stringstream](https://www.geeksforgeeks.org/stringstream-c-applications/)

- [nlohmann json library](https://github.com/nlohmann/json#serialization--deserialization)

## Comments and Observations

- The connection is opened and closed for every command, because the server
disconnects us after a certain amount of time

- stringstream is used to parse the response from the server

- The nlohmann json library is used to parse the JSON response from the
server and to create the JSON request to the server, because it is
easier to use and offers more readability and flexibility to the code

## Conclusion

The project helped me understand how a REST API works and how to
implement a client for it in C++.