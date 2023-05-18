#include <iostream>
#include <string>
#include <poll.h>
#include <cstring>
#include <arpa/inet.h>
#include <unistd.h>
#include <string.h>
#include <sys/socket.h>
#include <map>
#include <cstring>
#include <cstdlib>

#include "nlohmann/json.hpp"
#include "helpers.h"

using namespace std;

/* Error function */
void error(const char *msg) {
    perror(msg);
    exit(EXIT_FAILURE);
}

/* Open a socket to the server */
int open_connection(char *ip, uint16_t port) {
    int sockfd;
    struct sockaddr_in serv_addr;

    /* Create a socket point */
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0) {
        error("ERROR opening socket");
    }

    /* Fill in the server's address */
    memset(&serv_addr, 0, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(port);
    inet_aton(ip, &serv_addr.sin_addr);

    /* Connect to the server */
    int ret = connect(sockfd, (struct sockaddr *) &serv_addr, sizeof(serv_addr));
    if (ret < 0) {
        error("ERROR connecting");
    }

    return sockfd;
}

/* Initialize commands */
map<string, int> init_map() {
    map<string, int> commands;
    commands["register"] = REGISTER_COMMAND;
    commands["login"] = LOGIN_COMMAND;
    commands["enter_library"] = ENTER_LIBRARY_COMMAND;
    commands["get_books"] = GET_BOOKS_COMMAND;
    commands["get_book"] = GET_BOOK_COMMAND;
    commands["add_book"] = ADD_BOOK_COMMAND;
    commands["delete_book"] = DELETE_BOOK_COMMAND;
    commands["logout"] = LOGOUT_COMMAND;
    commands["exit"] = EXIT_COMMAND;

    return commands;
}

/* Initialize paths */
map<string, string> init_paths() {
    map<string, string> paths;
    paths["register"] = "/api/v1/tema/auth/register";
    paths["login"] = "/api/v1/tema/auth/login";
    paths["enter_library"] = "/api/v1/tema/library/access";
    paths["get_books"] = "/api/v1/tema/library/books";
    paths["get_book"] = "/api/v1/tema/library/books/";
    paths["add_book"] = "/api/v1/tema/library/books";
    paths["delete_book"] = "/api/v1/tema/library/books/";
    paths["logout"] = "/api/v1/tema/auth/logout";

    return paths;
}

/* Send request to server */
void send_to_server(int sockfd, string str_message) {
    char* message = str_message.data();
    int bytes, sent = 0;
    int total = strlen(message);

    do {
        bytes = write(sockfd, message + sent, total - sent);
        if (bytes < 0) {
            error("ERROR writing message to socket");
        }

        if (bytes == 0) {
            break;
        }

        sent += bytes;
    } while (sent < total);
}

/* Receive response from server */
string receive_from_server(int sockfd) {
    string response;

    char buffer[BUFLEN];
    memset(buffer, 0, BUFLEN);

    int bytes = read(sockfd, buffer, BUFLEN);
    if (bytes < 0) {
        error("ERROR reading message from socket");
    }

    response = buffer;

    while (bytes > 0) {
        memset(buffer, 0, BUFLEN);
        bytes = read(sockfd, buffer, BUFLEN);
        if (bytes < 0) {
            error("ERROR reading message from socket");
        }

        response += buffer;
    }

    return response;
}

/* Get number of lines in response */
int get_response_words(string response) {
    int words = 0;

    stringstream stream_str(response);

    string word;

    while (stream_str >> word) {
        words++;
    }

    return words;
}

/* Get code from response */
int get_response_code(string response) {
    int code = 0;

    stringstream stream_str(response);

    string word;

    stream_str >> word;
    stream_str >> word;

    code = stoi(word);

    return code;
}

/* Get cookies from response */
vector<string> get_cookies(string response, int words) {
    vector<string> cookies;

    stringstream stream_str(response);

    string word;

    for (int i = 0; i < words; i++) {
        stream_str >> word;

        if (word == "Set-Cookie:") {
            stream_str >> word;
            cookies.push_back(word);
        }
    }

    return cookies;
}

string get_payload(string response) {
    string payload;

    stringstream stream_str(response);

    string line;

    while (getline(stream_str, line)) {
        if (line == "\r") {
            break;
        }
    }

    while (getline(stream_str, line)) {
        payload += line;
    }

    return payload;
}

/* Get header for token authentication */
string compute_auth_header(string token) {
    string auth_header = "Authorization: Bearer " + token;

    return auth_header;
}