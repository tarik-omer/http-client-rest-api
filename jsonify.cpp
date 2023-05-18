#include <string>
#include <iostream>

#include "nlohmann/json.hpp"

using namespace std;

using namespace nlohmann;

json get_credentials_json(string username, string password) {
    json json;

    json["username"] = username.c_str();
    json["password"] = password.c_str();

    return json;
}

json get_add_book_json(string title, string author, string genre, string publisher, int page_count) {
    json json;

    json["title"] = title.c_str();
    json["author"] = author.c_str();
    json["genre"] = genre.c_str();
    json["publisher"] = publisher.c_str();
    json["page_count"] = page_count;

    return json;
}
