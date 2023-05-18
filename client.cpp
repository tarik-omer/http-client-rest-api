#include <string>
#include <iostream>
#include <map>
#include <unistd.h>
#include <cstring>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <poll.h>

#include "nlohmann/json.hpp"
#include "helpers.h"
#include "jsonify.h"
#include "requests.h"

#define MAX_LEN 200

using namespace std;

using namespace nlohmann;

using namespace nlohmann::literals;

int logout(int sockfd, string host, char* ip, int port, string session_cookie, map<string, string> paths);

int main()
{
    int sockfd;

    vector<string> empty_str_vector;
    string empty_str;

    /* To send a HTML request there are 4 (or 5) steps:
        - open connection* - only once
        - compute request
        - send request
        - receive response
        - parse response
    */

   /* Get possible commands map */
    map<string, int> commands = init_map();

    /* Get possible paths map (URLs) */
    map<string, string> paths = init_paths();

    /* String for storing session cookie */
    string session_cookie;

    /* String for stored token (JWT) */
    string token;

   /* Set host ip and port */
   char ip[] = "34.254.242.81";
   uint16_t port = 8080;

    /* Set host */
    string host = ip;
    host += ":";
    host += to_string(port);

    while (1) {
        /* Read command */
        string command;
        getline(cin, command);

        switch (commands[command]) {
            case REGISTER_COMMAND: {
                /* Register */
                string username, password;
                json payload;

                /* Check if logged in */
                if (session_cookie.size() != 0) {
                    cout << "You are already logged in!" << endl;
                    break;
                }

                cout << "Username = ";
                getline(cin, username);
                cout << "Password = ";
                getline(cin, password);

                payload = get_credentials_json(username, password);

                /* Get request */
                string request = compute_post_request(host, paths["register"], "application/json", payload.dump(), empty_str_vector, empty_str_vector);

                /* Open connection */
                sockfd = open_connection(ip, port);

                /* Send request to server */
                send_to_server(sockfd, request);

                /* Receive response from server */
                string response = receive_from_server(sockfd);

                /* Find response code */
                int response_code = get_response_code(response);

                /* Check response code */
                if (response_code / 100 == 4) {
                    cout << "Error " << response_code << " - Register failed!" << endl;
                    continue;
                }

                /* Notify user */
                cout << "Code " << response_code << " - Registered successfully!" << endl;

                /* Close connection */
                close(sockfd);

                break;
            }
            case LOGIN_COMMAND: {
                /* Check if logged in */
                if (session_cookie.size() != 0) {
                    cout << "You are already logged in!" << endl;
                    break;
                }
                
                /* Login */
                string username, password;
                json payload;
                cout << "username=";
                getline(cin, username);
                cout << "password=";
                getline(cin, password);

                payload = get_credentials_json(username, password);

                /* Get request */
                string request = compute_post_request(host, paths["login"], "application/json", payload.dump(), empty_str_vector, empty_str_vector);

                /* Connect to the server */
                sockfd = open_connection(ip, port);

                /* Send request to server */
                send_to_server(sockfd, request);

                /* Receive response from server */
                string response = receive_from_server(sockfd);

                /* Find response code */
                int response_code = get_response_code(response);

                /* Check response code */
                if (response_code / 100 == 4) {
                    string payload = get_payload(response);

                    if (payload.size() == 0) {
                        cout << "Error " << response_code << " - Login failed!" << endl;
                        break;
                    }                    

                    json payload_json = json::parse(payload);
                    string error_message = payload_json["error"];
                    cout << "Error " << response_code << " - " << error_message << endl;

                    break;
                }

                /* Get number of words in response */
                int words = get_response_words(response);

                /* Get cookies */
                vector<string> cookies = get_cookies(response, words);

                for (auto cookie : cookies) {
                    session_cookie = cookie;
                    break;
                }


                /* Add cookies to local cookies */
                for (auto cookie : cookies) {
                    cookies.push_back(cookie);
                }

                /* Notify user */
                cout << "Code " << response_code << " - Login successful!" << endl;

                /* Close connection */
                close(sockfd);
            }
                break;
            case ENTER_LIBRARY_COMMAND: {
                /* Send enter_library request to server */
                /* Check if logged in */
                if (session_cookie.size() == 0) {
                    cout << "You are not logged in!" << endl;
                    break;
                }

                /* Get request */
                vector<string> cookies = {session_cookie};
                string request = compute_get_request(host, paths["enter_library"], empty_str, cookies, empty_str_vector);
                
                /* Connect to the server */
                sockfd = open_connection(ip, port);

                /* Send request to server */
                send_to_server(sockfd, request);

                /* Receive response from server */
                string response = receive_from_server(sockfd);

                /* Find response code */
                int response_code = get_response_code(response);

                /* Check response code */
                if (response_code / 100 == 4) {
                    cout << "Error " << response_code << " - Not logged in!" << endl;
                    break;
                }

                /* Get payload */
                string payload = get_payload(response);

                /* Parse payload */
                json payload_json = json::parse(payload);

                /* Get token */
                token = payload_json["token"];

                /* Notify user */
                cout << "Code " << response_code << " - Entered library successfully!" << endl;

                /* Close connection */
                close(sockfd);

                break;
            }
            case GET_BOOKS_COMMAND: {
                /* Send get_books request to server */
                /* Check if logged in */
                if (session_cookie.size() == 0) {
                    cout << "You are not logged in!" << endl;
                    break;
                }

                /* Check if entered library */
                if (token.size() == 0) {
                    cout << "You have not entered the library!" << endl;
                    break;
                }

                /* Get request */
                vector<string> cookies = {session_cookie};
                string auth_header = compute_auth_header(token);
                vector<string> headers = {auth_header};

                string request = compute_get_request(host, paths["get_books"], empty_str, cookies, headers);

                /* Connect to the server */
                sockfd = open_connection(ip, port);

                /* Send request to server */
                send_to_server(sockfd, request);

                /* Receive response from server */
                string response = receive_from_server(sockfd);

                /* Find response code */
                int response_code = get_response_code(response);

                /* Check response code */
                if (response_code / 100 == 4) {
                    cout << "Error " << response_code << " - Not logged in!" << endl;
                    break;
                }

                /* Get payload */
                string payload = get_payload(response);

                /* Parse payload */
                json payload_json = json::parse(payload);

                /* Print books */
                cout << "Books:" << endl;
                cout << payload_json.dump(4) << endl;

                /* Notify user */
                cout << "Code " << response_code << " - Books received successfully!" << endl;

                /* Close connection */
                close(sockfd);

                break;
            }
            case GET_BOOK_COMMAND: {
                /* Send get_book request to server */
                /* Check if logged in */
                if (session_cookie.size() == 0) {
                    cout << "You are not logged in!" << endl;
                    break;
                }

                /* Check if entered library */
                if (token.size() == 0) {
                    cout << "You have not entered the library!" << endl;
                    break;
                }

                /* Get book id */
                string book_id;
                cout << "id:";
                getline(cin, book_id);

                /* Get request */
                vector<string> cookies = {session_cookie};
                string auth_header = compute_auth_header(token);
                vector<string> headers = {auth_header};

                string request = compute_get_request(host, paths["get_book"], book_id, cookies, headers);

                /* Connect to the server */
                sockfd = open_connection(ip, port);

                /* Send request to server */
                send_to_server(sockfd, request);

                /* Receive response from server */
                string response = receive_from_server(sockfd);

                /* Find response code */
                int response_code = get_response_code(response);

                /* Check response code */
                if (response_code / 100 == 4) {
                    /* Get payload */
                    string payload = get_payload(response);

                    /* Parse payload */
                    json payload_json = json::parse(payload);
                    string error_message = payload_json["error"];

                    cout << "Error " << response_code << " - " << error_message << endl;
                    break;
                }

                /* Get payload */
                string payload = get_payload(response);

                /* Parse payload */
                json payload_json = json::parse(payload);

                /* Print book */
                cout << "Book:" << endl;
                cout << payload_json.dump(4) << endl;

                /* Notify user */
                cout << "Code " << response_code << " - Book received successfully!" << endl;

                /* Close connection */
                close(sockfd);

                break;
            }
            case ADD_BOOK_COMMAND: {
                /* Send add_book request to server */
                /* Check if logged in */
                if (session_cookie.size() == 0) {
                    cout << "You are not logged in!" << endl;
                    break;
                }

                /* Check if entered library */
                if (token.size() == 0) {
                    cout << "You have not entered the library!" << endl;
                    break;
                }

                /* Get book data */
                string title, author, genre, publisher, page_count;
                cout << "title:";
                getline(cin, title);
                cout << "author:";
                getline(cin, author);
                cout << "genre:";
                getline(cin, genre);
                cout << "publisher:";
                getline(cin, publisher);
                cout << "page_count:";
                getline(cin, page_count);

                /* Create json */
                json book_json;
                book_json["title"] = title;
                book_json["author"] = author;
                book_json["genre"] = genre;
                book_json["publisher"] = publisher;
                book_json["page_count"] = page_count;

                /* Get request */
                vector<string> cookies = {session_cookie};
                string auth_header = compute_auth_header(token);
                vector<string> headers = {auth_header};

                string request = compute_post_request(host, paths["add_book"], "application/json", book_json.dump(), cookies, headers);

                /* Connect to the server */
                sockfd = open_connection(ip, port);

                /* Send request to server */
                send_to_server(sockfd, request);

                /* Receive response from server */
                string response = receive_from_server(sockfd);

                /* Find response code */
                int response_code = get_response_code(response);

                /* Check response code */
                if (response_code / 100 == 4) {
                    /* Get payload */
                    string payload = get_payload(response);

                    /* Parse payload */
                    json payload_json = json::parse(payload);
                    string error_message = payload_json["error"];

                    cout << "Error " << response_code << " - " << error_message << endl;
                    break;
                }

                /* Notify user */
                cout << "Code " << response_code << " - Book added successfully!" << endl;

                /* Close connection */
                close(sockfd);

                break;
            }
            case DELETE_BOOK_COMMAND: {
                /* Send delete_book request to server */
                /* Check if logged in */
                if (session_cookie.size() == 0) {
                    cout << "You are not logged in!" << endl;
                    break;
                }

                /* Check if entered library */
                if (token.size() == 0) {
                    cout << "You have not entered the library!" << endl;
                    break;
                }

                /* Get book id */
                string book_id;

                cout << "id:";
                getline(cin, book_id);

                /* Get request */
                vector<string> cookies = {session_cookie};
                string auth_header = compute_auth_header(token);
                vector<string> headers = {auth_header};

                string request = compute_delete_request(host, paths["delete_book"], book_id, cookies, headers);

                /* Connect to the server */
                sockfd = open_connection(ip, port);

                /* Send request to server */
                send_to_server(sockfd, request);

                /* Receive response from server */
                string response = receive_from_server(sockfd);

                /* Find response code */
                int response_code = get_response_code(response);

                /* Check response code */
                if (response_code / 100 == 4) {
                    /* Get payload */
                    string payload = get_payload(response);

                    /* Parse payload */
                    json payload_json = json::parse(payload);
                    string error_message = payload_json["error"];

                    cout << "Error " << response_code << " - " << error_message << endl;
                    break;
                }

                /* Notify user */
                cout << "Code " << response_code << " - Book deleted successfully!" << endl;

                /* Close connection */
                close(sockfd);

                break;
            }
            case LOGOUT_COMMAND: {
                /* Send logout request to server */
                int ret = logout(sockfd, host, ip, port, session_cookie, paths);

                if (ret != 0) {
                    break;
                }

                /* Delete session cookie */
                session_cookie = "";

                /* Delete token */
                token = "";

                break;
            }
            case EXIT_COMMAND: {
                /* Close connection and exit program */
                return 0;
                break;
            }
            default:
                cout << "Invalid command" << endl;
                break;
        }
    }

    return 0;
}

/* Logout function - sends a logout request to the server */
int logout(int sockfd, string host, char* ip, int port, string session_cookie, map<string, string> paths) {
    /* Check if logged in */
    if (session_cookie.size() == 0) {
        cout << "You are not logged in!" << endl;
        return 1;
    }

    vector<string> empty_str_vector;
    string empty_str;

    /* Get request */
    vector<string> cookies = {session_cookie};
    string request = compute_get_request(host, paths["logout"], empty_str, cookies, empty_str_vector);

    /* Connect to the server */
    sockfd = open_connection(ip, port);

    /* Send request to server */
    send_to_server(sockfd, request);

    /* Receive response from server */
    string response = receive_from_server(sockfd);

    /* Find response code */
    int response_code = get_response_code(response);

    /* Check response code */
    if (response_code / 100 == 4) {
        cout << "Error " << response_code << " - Not logged in!" << endl;
        return 1;
    }

    /* Notify user */
    cout << "Code " << response_code << " - Logged out successfully!" << endl;

    /* Close connection */
    close(sockfd);

    return 0;
}