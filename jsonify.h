#ifndef JSONIFY_H
#define JSONIFY_H

#include <string>

using namespace std;

nlohmann::json get_credentials_json(string username, string password);
nlohmann::json get_add_book_json(string title, string author, string genre, string publisher, int page_count);

#endif