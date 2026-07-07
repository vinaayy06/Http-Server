#include <iostream>
#include <cstdlib>
#include <cstring>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>

using namespace std;

int main(int argc, char **argv) {
    cout << unitbuf;
    cerr << unitbuf;

    cout << "Logs from your program will appear here!" << endl;

    // Create socket
    int server_fd = socket(AF_INET, SOCK_STREAM, 0);

    if (server_fd < 0) {
        cerr << "Failed to create socket" << endl;
        return 1;
    }

    // Allow address reuse
    int yes = 1;
    setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(yes));

    // Server address
    sockaddr_in server_addr{};
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = htons(4221);

    // Bind socket
    if (bind(server_fd, (sockaddr *)&server_addr, sizeof(server_addr)) < 0) {
        cerr << "Bind failed" << endl;
        return 1;
    }

    // Start listening
    if (listen(server_fd, 5) < 0) {
        cerr << "Listen failed" << endl;
        return 1;
    }

    while (true) {

        sockaddr_in client_addr{};
        socklen_t client_addr_len = sizeof(client_addr);

        int client_fd = accept(
            server_fd,
            (sockaddr *)&client_addr,
            &client_addr_len
        );

        if (client_fd < 0) {
            continue;
        }

        // HTTP Response
        string response = "HTTP/1.1 200 OK\r\n\r\n";

        send(
            client_fd,
            response.c_str(),
            response.size(),
            0
        );

        close(client_fd);
    }

    close(server_fd);

    return 0;
}