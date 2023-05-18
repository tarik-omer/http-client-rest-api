#include <string>
#include <vector>
#include <iostream>

using namespace std;

string compute_get_request(string host, string url, string query_params, vector<string> cookies, vector<string> headers) {
    /* Set method, path, query parameters, protocol and host */
    string request = "GET " + url + query_params + " HTTP/1.1\r\n";
    request += "Host " + host + "\r\n";

    /* Set headers */
    for (auto header : headers) {
        request += header + "\r\n";
    }

    /* Set cookies */
    if (cookies.size() > 0) {
        /* Add cookie header */
        request += "Cookie: ";
        /* Add cookies separated by ';' */
        for (auto cookie : cookies) {
            request += cookie + "; ";
        }
        request += "\r\n";
    }

    /* Add final new line */
    request += "\r\n";
    
    return request;
}

string compute_post_request(string host, string url, string content_type, string body_data, vector<string> cookies, vector<string> opt_headers) {
    /* Set method, path, query parameters, protocol and host */
    string request = "POST " + url + " HTTP/1.1\r\n";
    request += "Host " + host + "\r\n";

    /* Set headers mandatory headers */
    request += "Content-Type: " + content_type + "\r\n";
    request += "Content-Length: " + to_string(body_data.size()) + "\r\n";

    /* Set optional headers */
    for (auto header : opt_headers) {
        request += header + "\r\n";
    }

    /* Set cookies */
    if (cookies.size() > 0) {
        /* Add cookie header */
        request += "Cookie: ";
        for (auto cookie : cookies) {
            /* Add cookies separated by ';' */
            request += cookie + "; ";
        }
        request += "\r\n";
    }

    /* Add final new line */
    request += "\r\n";

    /* Add body data */
    request += body_data;

    return request;
}

string compute_delete_request(string host, string url, string query_params, vector<string> cookies, vector<string> headers) {
    /* Set method, path, query parameters, protocol and host */
    string request = "DELETE " + url + query_params + " HTTP/1.1\r\n";
    request += "Host: " + host + "\r\n";

    /* Set headers */
    for (auto header : headers) {
        request += header + "\r\n";
    }

    /* Set cookies */
    if (cookies.size() > 0) {
        /* Add cookie header */
        request += "Cookie: ";
        for (auto cookie : cookies) {
            /* Add cookies separated by ';' */
            request += cookie + "; ";
        }
        request += "\r\n";
    }

    /* Add final new line */
    request += "\r\n";
    
    return request;
}
