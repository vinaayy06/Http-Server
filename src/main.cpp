#include <iostream>
#include <cstring>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <thread>
#include <fstream>      
#include <sstream>   

using namespace std;
string directory = "";
void handle_client(int client_fd) {
    char buffer[1024] = {};
    recv(client_fd, buffer, sizeof(buffer), 0);
    string request = buffer;
    int first_space = request.find(' ');
    int second_space = request.find(' ', first_space + 1);
    string path = request.substr(first_space + 1, second_space - first_space - 1);

    cout << "Requested path: " << path << endl;

    string user_agent = "";
    string ua_key = "User-Agent: ";
    int ua_start = request.find(ua_key);

    if (ua_start != string::npos) {
        ua_start += ua_key.length();
        int ua_end = request.find("\r\n", ua_start);
        user_agent = request.substr(ua_start, ua_end - ua_start);
    }
    string response;

    if (path == "/") {

        response = "HTTP/1.1 200 OK\r\n\r\n";

    } else if (path.substr(0, 6) == "/echo/") {
        string body = path.substr(6);

        response = "HTTP/1.1 200 OK\r\n";
        response += "Content-Type: text/plain\r\n";
        response += "Content-Length: " + to_string(body.length()) + "\r\n";
        response += "\r\n";
        response += body;

    } else if (path == "/user-agent") {
        string body = user_agent;

        response = "HTTP/1.1 200 OK\r\n";
        response += "Content-Type: text/plain\r\n";
        response += "Content-Length: " + to_string(body.length()) + "\r\n";
        response += "\r\n";
        response += body;

    } else if (path.substr(0, 7) == "/files/") {
        string filename = path.substr(7);
        string filepath = directory + filename;

        cout << "Looking for file: " << filepath << endl;
        ifstream file(filepath);

        if (file.is_open()) {
            stringstream file_contents;
            file_contents << file.rdbuf();
            string body = file_contents.str();
            file.close();
            response = "HTTP/1.1 200 OK\r\n";
            response += "Content-Type: application/octet-stream\r\n";
            response += "Content-Length: " + to_string(body.length()) + "\r\n";
            response += "\r\n";
            response += body;

        } else {
            response = "HTTP/1.1 404 Not Found\r\n\r\n";

        }

    } else {

        response = "HTTP/1.1 404 Not Found\r\n\r\n";

    }

    send(client_fd, response.c_str(), response.length(), 0);

    close(client_fd);
}

int main(int argc, char* argv[]) {
    for (int i = 1; i < argc; i++) {

        if (string(argv[i]) == "--directory") {h
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