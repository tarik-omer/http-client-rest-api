#ifndef HELPERS_H
#define HELPERS_H

#define BUFLEN 4096

#define REGISTER_COMMAND 1
#define LOGIN_COMMAND 2
#define ENTER_LIBRARY_COMMAND 3
#define GET_BOOKS_COMMAND 4
#define GET_BOOK_COMMAND 5
#define ADD_BOOK_COMMAND 6
#define DELETE_BOOK_COMMAND 7
#define LOGOUT_COMMAND 8
#define EXIT_COMMAND 9

#include <iostream>

using namespace std;

int open_connection(char *ip, uint16_t port);
void error(const char *msg);
map<string, int> init_commands();
map<string, string> init_paths();
void send_to_server(int sockfd, string str_message);
string receive_from_server(int sockfd);
int get_response_code(string response);
vector<string> get_cookies(string response, int lines);
int get_response_words(string response);
string get_payload(string response);
string compute_auth_header(string token);
bool is_number(string s);

#endif