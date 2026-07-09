#include <iostream>
#include <cstring>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <thread>
#include <fstream>
#include <sstream>
#include <zlib.h>

using namespace std;
string directory = "";
string gzip_compress(const string& data) {
    z_stream zs;
    memset(&zs, 0, sizeof(zs));
    deflateInit2(&zs, Z_DEFAULT_COMPRESSION, Z_DEFLATED, 15 + 16, 8, Z_DEFAULT_STRATEGY);
    zs.next_in = (Bytef*)data.data();
    zs.avail_in = data.size();
    string compressed = "";
    char buffer[32768];
    int result;

    do {
        zs.next_out = (Bytef*)buffer;
        zs.avail_out = sizeof(buffer);
        result = deflate(&zs, Z_FINISH);
        int bytes_written = sizeof(buffer) - zs.avail_out;
        compressed.append(buffer, bytes_written);

    } while (result == Z_OK);
    deflateEnd(&zs);

    return compressed;
}
string handle_request(const string& request) {
    int first_space = request.find(' ');
    string method = request.substr(0, first_space);

    int second_space = request.find(' ', first_space + 1);
    string path = request.substr(first_space + 1, second_space - first_space - 1);

    cout << "Method: " << method << endl;
    cout << "Path:   " << path << endl;
    string user_agent = "";
    string ua_key = "User-Agent: ";
    int ua_start = request.find(ua_key);

    if (ua_start != string::npos) {
        ua_start += ua_key.length();
        int ua_end = request.find("\r\n", ua_start);
        user_agent = request.substr(ua_start, ua_end - ua_start);
    }
    string accept_encoding = "";
    string ae_key = "Accept-Encoding: ";
    int ae_start = request.find(ae_key);

    if (ae_start != string::npos) {
        ae_start += ae_key.length();
        int ae_end = request.find("\r\n", ae_start);
        accept_encoding = request.substr(ae_start, ae_end - ae_start);
    }
    bool supports_gzip = (accept_encoding.find("gzip") != string::npos);
    string body = "";
    int body_start = request.find("\r\n\r\n");
    if (body_start != string::npos) {
        body = request.substr(body_start + 4);
    }
    string content_length_str = "";
    string cl_key = "Content-Length: ";
    int cl_start = request.find(cl_key);
    if (cl_start != string::npos) {
        cl_start += cl_key.length();
        int cl_end = request.find("\r\n", cl_start);
        content_length_str = request.substr(cl_start, cl_end - cl_start);
    }

    int content_length = 0;
    if (!content_length_str.empty()) {
        content_length = stoi(content_length_str);
    }
    if (content_length > 0 && body.length() > (size_t)content_length) {
        body = body.substr(0, content_length);
    }
    string response;

    if (path == "/") {
        response = "HTTP/1.1 200 OK\r\n\r\n";

    } else if (path.substr(0, 6) == "/echo/") {
        string echo_body = path.substr(6);
        response = "HTTP/1.1 200 OK\r\n";

        if (supports_gzip) {
            string compressed_body = gzip_compress(echo_body);

            response += "Content-Encoding: gzip\r\n";
            response += "Content-Type: text/plain\r\n";
            response += "Content-Length: " + to_string(compressed_body.length()) + "\r\n";
            response += "\r\n";
            response += compressed_body;

        } else {
            response += "Content-Type: text/plain\r\n";
            response += "Content-Length: " + to_string(echo_body.length()) + "\r\n";
            response += "\r\n";
            response += echo_body;
        }

    } else if (path == "/user-agent") {
        response = "HTTP/1.1 200 OK\r\n";
        response += "Content-Type: text/plain\r\n";
        response += "Content-Length: " + to_string(user_agent.length()) + "\r\n";
        response += "\r\n";
        response += user_agent;

    } else if (path.substr(0, 7) == "/files/") {
        string filename = path.substr(7);
        string filepath = directory + filename;

        if (method == "GET") {
            ifstream file(filepath);

            if (file.is_open()) {

                stringstream file_contents;
                file_contents << file.rdbuf();
                string file_body = file_contents.str();
                file.close();

                response = "HTTP/1.1 200 OK\r\n";
                response += "Content-Type: application/octet-stream\r\n";
                response += "Content-Length: " + to_string(file_body.length()) + "\r\n";
                response += "\r\n";
                response += file_body;

            } else {

                response = "HTTP/1.1 404 Not Found\r\n\r\n";
            }

        } else if (method == "POST") {
            ofstream file(filepath);

            if (file.is_open()) {

                file << body;
                file.close();
                response = "HTTP/1.1 201 Created\r\n\r\n";

            } else {

                response = "HTTP/1.1 500 Internal Server Error\r\n\r\n";
            }
        }

    } else {
        response = "HTTP/1.1 404 Not Found\r\n\r\n";
    }

    return response;
}
void handle_client(int client_fd) {
    while (true) {
        char buffer[4096] = {};
        int bytes_received = recv(client_fd, buffer, sizeof(buffer), 0);
        if (bytes_received <= 0) {
            cout << "Client disconnected." << endl;
            break;
        }
        string request = buffer;
        bool should_close = (request.find("Connection: close") != string::npos);
        string response = handle_request(request);
        send(client_fd, response.c_str(), response.length(), 0);
        if (should_close) {
            cout << "Client requested connection close." << endl;
            break;

        }
        cout << "Waiting for next request on same connection..." << endl;
    }
    close(client_fd);
}

int main(int argc, char* argv[]) {
    for (int i = 1; i < argc; i++) {
        if (string(argv[i]) == "--directory") {
            if (i + 1 < argc) {
                directory = argv[i + 1];
                cout << "Serving files from: " << directory << endl;
            }
        }
    }
    int server_fd = socket(AF_INET, SOCK_STREAM, 0);

    int opt = 1;
    setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

    struct sockaddr_in server_addr;
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(4221);
    server_addr.sin_addr.s_addr = INADDR_ANY;

    bind(server_fd, (struct sockaddr *)&server_addr, sizeof(server_addr));
    listen(server_fd, 5);

    cout << "Server is running on port 4221..." << endl;

    while (true) {
        int client_fd = accept(server_fd, NULL, NULL);
        thread t(handle_client, client_fd);
        t.detach();
    }

    close(server_fd);
    return 0;
}