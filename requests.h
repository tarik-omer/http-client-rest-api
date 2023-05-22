#ifndef REQUESTS_H
#define REQUESTS_H

using namespace std;

string compute_get_request(string host, string url, string query_params, vector<string> cookies, vector<string> headers);
string compute_post_request(string host, string url, string content_type, string body_data, vector<string> cookies, vector<string> opt_headers);
string compute_delete_request(string host, string url, string query_params, vector<string> cookies, vector<string> headers);

#endif