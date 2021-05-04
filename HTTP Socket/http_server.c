#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>
#include <netdb.h> // for getnameinfo()

// Usual socket headers
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>

#include <arpa/inet.h>

#define SIZE 1024
#define BACKLOG 10  // Passed to listen()

void report(struct sockaddr_in *server_address);

void sethttp_header(char http_header[])
{
    // File object to return
    FILE *htmlData = fopen("index.html", "r");

    char line[100];
    char response_data[8000];
    while (fgets(line, 100, htmlData) != 0) {
        strcat(response_data, line);
    }
    // char http_header[8000] = "HTTP/1.1 200 OK\r\n\n";
    strcat(http_header, response_data);
}

int main(void)
{
    char http_header[8000] = "HTTP/1.1 200 OK\r\n\n";

    // Socket setup: creates an endpoint for communication, returns a descriptor
    int server_socket = socket(
        AF_INET,      // Domain: specifies protocol family
        SOCK_STREAM,  // Type: specifies communication semantics
        0             // Protocol: 0 because there is a single protocol for the specified family
    );

    // Construct local address structure
    struct sockaddr_in server_address;
    server_address.sin_family = AF_INET;
    server_address.sin_port = htons(8001);
    server_address.sin_addr.s_addr = htonl(INADDR_LOOPBACK);//inet_addr("127.0.0.1");

    // Bind socket to local address
    // bind() assigns the address specified by server_address to the socket
    // referred to by the file descriptor server_socket.
    bind(
        server_socket,                         // file descriptor referring to a socket
        (struct sockaddr *) &server_address,   // Address to be assigned to the socket
        sizeof(server_address)                 // Size (bytes) of the address structure
    );

    // Mark socket to listen for incoming connections
    int listening = listen(server_socket, BACKLOG);
    if (listening < 0) {
        printf("Error: The server is not listening.\n");
        return 1;
    }
    report(&server_address);     // Custom report function
    sethttp_header(http_header);  // Custom function to set header
    int client_socket;

    // Wait for a connection, create a connected socket if a connection is pending
    while(1) {
        client_socket = accept(server_socket, NULL, NULL);
        send(client_socket, http_header, sizeof(http_header), 0);
        close(client_socket);
    }
    return 0;
}

void report(struct sockaddr_in *server_address)
{
    char hostBuffer[INET6_ADDRSTRLEN];
    char serviceBuffer[NI_MAXSERV]; // defined in `<netdb.h>`
    socklen_t addr_len = sizeof(*server_address);
    int err = getnameinfo(
        (struct sockaddr *) server_address,
        addr_len,
        hostBuffer,
        sizeof(hostBuffer),
        serviceBuffer,
        sizeof(serviceBuffer),
        NI_NUMERICHOST
    );
    if (err != 0) {
        printf("It's not working!!\n");
    }
    printf("\n\n\tServer listening on http://%s:%s\n", hostBuffer, serviceBuffer);
}