#include "client.h"

int main(int argc, char **argv) {
    int socket_fd;
    struct sockaddr_in server;
    char *message;
    char server_reply[2000] = "";

    //socket setup
    socket_fd = socket(AF_INET, SOCK_STREAM, 0);
    if(socket_fd == -1) {
        printf("failed to create socket\n");
        return 1;
    }

    //define destination server
    server.sin_addr.s_addr = inet_addr(LOCALHOST_IP);
    server.sin_family = AF_INET;
    server.sin_port = htons(LOCALHOST_PORT);

    //connect to server
    int server_debug = connect(socket_fd, (struct sockaddr *)&server, sizeof(server));
    if(server_debug < 0) {
        printf("server connection error\n");
        return 2;
    }

    //sending data to server
    message = "GET /script.js HTTP/1.1\r\nHost: 127.0.0.1\r\n\r\n"; //http command to fetch the mainpage of a website.
    int send_debug = send(socket_fd, message, strlen(message), 0);
    if(send_debug < 0) {
        printf("failed to send data");
        return 3;
    }

    //receiving data from server
    int receive_debug = recv(socket_fd, server_reply, 2000, 0); //max length of buffer 2000 chars
    if(receive_debug < 0) {
        printf("failed to receive data");
        return 4;
    }
    puts(server_reply);

    char i[1024];
    fgets(i, 1024, stdin);
    while (strcmp(i, "quit\n") != 0) {
        fgets(i, 1024, stdin);
    }
    
    //close socket and exit program
    close(socket_fd);
    return 0;
    
}