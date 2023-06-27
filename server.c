/*
*********************************************
*         COSE342 Computer Networks         *
*               Term Project                *
*          Student Name: KIM, Inho          *
*          Student ID: 2023951398           *
*********************************************
*/

#include "server.h"

char* parse_request(int otherSocket, char* req_buffer, char* resources_dir) {
    //return path to requested resource file,
    //send 400 request if the request is invalid (i.e. not GET request)

    char resdir[strlen(resources_dir)];
    strcpy(resdir, resources_dir);

    char filePath[strlen(req_buffer)];
    strcpy(filePath, req_buffer);

    char* flush = strtok(filePath, " ");
    char* uri = strtok(NULL, " ");

    // printf("flush: %s, uri: %s\n", flush, uri);

    char* header;
    if(strcmp(flush, "GET")!=0) {
        //since the project requires GET only, treat any other request type as a bad request
        header = "HTTP/1.1 400 Bad Request\r\nContent-Length: 70\r\nConnection: close\r\n\r\n<html><head><title>Error</title></head><body>Bad Request</body></html>";
        send(otherSocket, header, strlen(header), 0);
        return NULL;
    }

    if(strstr(req_buffer, "Host:")==NULL) {
        header = "HTTP/1.1 400 Bad Request\r\nContent-Length: 70\r\nConnection: close\r\n\r\n<html><head><title>Error</title></head><body>Bad Request</body></html>";
        send(otherSocket, header, strlen(header), 0);
        return NULL;
    }


    if(strcmp(uri, "/")==0) {
        size_t index_len = strlen(INDEX);
        size_t resources_len = strlen(resdir);
        char *newa = malloc(index_len + resources_len + 1);
        if(newa) {
            strcpy(newa, resdir);
            strcat(newa, INDEX);
        }
        return newa;
        
    }
    else {
        size_t resources_len = strlen(resdir);
        size_t uri_len = strlen(uri);

        char *newb = malloc(resources_len + uri_len + 1);
        if(newb) {
            strcpy(newb, resdir);
            strcat(newb, uri);
        }
        return newb;
    }
}

void send_res(int otherSocket, char* path) {
    //send header and file content, 404 if file cannot be found

    char filePath[strlen(path)];
    strcpy(filePath, path);

    char* flush = strtok(filePath, ".");
    char* format = strtok(NULL, ".");
    char* header;
    long file_size;
    int bytes_read, debug;

    header = "HTTP/1.1 200 OK\r\nConnection: Keep-Alive\r\n";
    char* fileType;
    FILE* file = fopen(path, "rb");

    if(file == NULL) {
        //invalid file, send 404

        header = "HTTP/1.1 404 Not Found\r\nContent-Length: 68\r\nConnection: close\r\n\r\n<html><head><title>Error</title></head><body>Not Found</body></html>";
        // printf("failed at %s\n", path);
        debug = send(otherSocket, header, strlen(header), 0);
        if(debug < 0) {
            // perror("failed to send header");
            close(otherSocket);
            exit(EXIT_FAILURE);
        }
    }
    else {
        //the requested file is found, send appropriate header and file content
        
        if (!strcmp(format, "html")) {
            fileType = "Content-Type: text/html\r\n\r\n";
        } else if(!strcmp(format, "js")) {
            fileType = "Content-Type: text/javascript\r\n\r\n";
        } else if(!strcmp(format, "jpg")) {
            fileType = "Content-Type: image/jpg\r\n\r\n";
        } else if(!strcmp(format, "css")) {
            fileType = "Content-Type: image/css\r\n\r\n";
        } else if(!strcmp(format, "png")) {
            fileType = "Content-Type: image/png\r\n\r\n";
        } else {
            fileType = "\r\n";
        }
        if(!strcmp(format, "html") || !strcmp(format, "js") || !strcmp(format, "css")) {
            //html or js or css file

            long remainder, num_buffers, temp;
            long filelen;
            fseek(file, 0, SEEK_END);
            filelen = ftell(file);
            fseek(file, 0, SEEK_SET);
            remainder = filelen % FILEBUFF_SIZE;
            num_buffers = filelen / FILEBUFF_SIZE;
            // printf("num_buffers %ld, remainder %ld\n", num_buffers, remainder);

            char str[19+sizeof(char)];
            sprintf(str, "Content-Length: %ld\r\n", filelen);
            size_t total_len = strlen(header) + strlen(fileType) + strlen(str);
            char* total_response = (char *)malloc((sizeof *total_response) * total_len);
            sprintf(total_response, "%s%s%s", header, str, fileType);
            send(otherSocket, total_response, strlen(total_response), 0);

            if(num_buffers==0) {
                char file_buff2[remainder];
                fread(file_buff2, sizeof(file_buff2), 1, file);
                send(otherSocket, file_buff2, remainder, 0);
            }
            else {
                char file_buff[FILEBUFF_SIZE]; //for images
                while(fread(file_buff, sizeof(file_buff), 1, file)==1) {
                    send(otherSocket, file_buff, FILEBUFF_SIZE, 0);
                }
                if(remainder!=0) {
                    char file_buff3[remainder];
                    fread(file_buff3, sizeof(file_buff3), 1, file);
                    send(otherSocket, file_buff3, remainder, 0);
                }
            }
            //free allocated memory
            free(total_response);
        }
        else {
            //image file
            long remainder, num_buffers, temp;

            long filelen;
            fseek(file, 0, SEEK_END);
            filelen = ftell(file);
            fseek(file, 0, SEEK_SET);
            remainder = filelen % FILEBUFF_SIZE;
            num_buffers = filelen / FILEBUFF_SIZE;
            // printf("num_buffers %ld, remainder %ld\n", num_buffers, remainder);

            char str[22+sizeof(char)];
            sprintf(str, "Content-Length: %ld\r\n", filelen);
            size_t total_len = strlen(header) + strlen(fileType) + strlen(str);
            char* total_response = (char *)malloc((sizeof *total_response) * total_len);
            sprintf(total_response, "%s%s%s", header, str, fileType);
            send(otherSocket, total_response, strlen(total_response), 0);

            unsigned char file_buff[FILEBUFF_SIZE]; //for images
            while(fread(file_buff, sizeof(file_buff), 1, file)==1) {
                send(otherSocket, file_buff, FILEBUFF_SIZE, 0);
            }
            
            //free allocated memory
            free(total_response);
        }
        fclose(file);
    }
}

int main(int argc, char **argv) {
    int port_num = atoi(argv[1]);
    char* resources_str = (char *)malloc((sizeof argv[2]));
    strcpy(resources_str, argv[2]);
    // printf("port num: %d and %ld\n", port_num, sizeof(argv[2]));
    // printf("resources dir: %s\n", resources_str);
    
    //define variables
    int tru = 1;
    int mainSocket, bindDebug, socketSize, acceptSocket, addressLength, _mainSocket, otherSocket, wakeup, readSocket;
    int clientSockets[10], maxClients = 10; 
    struct sockaddr_in server, client;
    char buffer[REQBUFF_SIZE]; //2K buffer

    //socket setup
    fd_set fdSet;
    for(int i = 0; i < maxClients; i++) {
        clientSockets[i] = 0;
    }
    mainSocket = socket(AF_INET, SOCK_STREAM, 0);
    if(mainSocket == 0) {
        // printf("failed at creating socket\n");
        close(mainSocket);
        exit(EXIT_FAILURE);
    }

    if (setsockopt(mainSocket, SOL_SOCKET, SO_REUSEADDR, (char *)&tru, sizeof(tru))) {
        // printf("failed at setsockopt\n");
        close(mainSocket);
        exit(EXIT_FAILURE);
    }

    //define local server parameters
    server.sin_addr.s_addr = inet_addr(IP_ADDRESS);
    server.sin_family = AF_INET;
    server.sin_port = htons(port_num);

    //bind socket to server
    bindDebug = bind(mainSocket, (struct sockaddr *)&server, sizeof(server));
    if(bindDebug < 0) {
        // printf("failed at binding mainSocket to server\n");
        close(mainSocket);
        exit(EXIT_FAILURE);
    }
    // printf("binded to socket, listening at port %d\n", PORT_NUMBER);
    if(listen(mainSocket, 5) < 0) {
        // printf("erorr listening to socket\n");
        close(mainSocket);
        exit(EXIT_FAILURE);
    }

    addressLength = sizeof(server);
    // printf("addressLength: %d\n", addressLength);
    // printf("listening to socket for connections\n");

    while(1) {
        FD_ZERO(&fdSet);
        FD_SET(mainSocket, &fdSet);
        _mainSocket = mainSocket;

        //add secondary sockets fdSet
        for(int i = 0; i < maxClients; i++) {
            otherSocket = clientSockets[i];
            if(otherSocket > 0) FD_SET(otherSocket, &fdSet);
            if(otherSocket > _mainSocket) _mainSocket = otherSocket;
        }

        //new incoming activity (request, etc.) from current socket
        wakeup = select(_mainSocket+1, &fdSet, NULL, NULL, NULL);
        if(wakeup < 0) {
            close(mainSocket);
            exit(EXIT_FAILURE);
        }

        //new incoming activity from mainSocket (new connection)
        if(FD_ISSET(mainSocket, &fdSet)) {
            int newSocket = accept(mainSocket, (struct sockaddr *)&server, (socklen_t *)&addressLength);
            if(newSocket < 0) {
                // printf("failed at accepting connection from newSocket\n");
                exit(EXIT_FAILURE);
            }
            // printf("new connection socket fd: %d, ip: %s, port: %d \n", newSocket, inet_ntoa(server.sin_addr), ntohs(server.sin_port));

            for(int i=0; i < maxClients; i++) {
                if(clientSockets[i] == 0 ) {
                    clientSockets[i] = newSocket;
                    break;
                }
            }
        }

        //other connections from existing socket fds
        for(int i=0; i < maxClients; i++) {
            otherSocket = clientSockets[i];

            if(FD_ISSET(otherSocket, &fdSet)) {
                readSocket = recv(otherSocket, buffer, REQBUFF_SIZE, 0);
                if(readSocket==0) {
                    //a client disconnected from server
                    getpeername(otherSocket, (struct sockaddr *)&server, (socklen_t *)&addressLength);
                    // printf("some dude disconnected\n");
                    close(otherSocket);
                    clientSockets[i] = 0;
                }
                else { 
                    //process request
                    buffer[readSocket] = '\0';
                    // printf("%s\n", buffer);
                    char* path = parse_request(otherSocket, buffer, resources_str);
                    if(path == NULL) {
                        free(path);
                        break;
                    }
                    send_res(otherSocket, path);

                    //free memory allocated in newa/newb from parse_request
                    free(path);

                }
            }
        }
    }
    free(resources_str);
    return 0;
}