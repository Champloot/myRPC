#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include "../common/libmysyslog.h"

#define BUFFER_SIZE 1024

void print_help() {
    printf("Usage: myRPC-client [options]\n");
    printf("Options:\n");
    printf("  -c, --command \"bash command\"  Command to execute on server\n");
    printf("  -h, --host \"ip_addr\"         Server IP address\n");
    printf("  -p, --port port_num          Server port number\n");
    printf("  -s, --stream                 Use stream socket (TCP)\n");
    printf("  -d, --dgram                  Use datagram socket (UDP)\n");
    printf("  --help                       Show this help message\n");
}

int main(int argc, char *argv[]) {
    char *command = NULL;
    char *host = "127.0.0.1";
    int port = 1234;
    int socket_type = SOCK_STREAM; // дефолт
    char *username = NULL;
    
    // парсинг аргуементов
    for (int i = 1; i < argc; i++) {
        if (strcmp(argv[i], "-c") == 0 || strcmp(argv[i], "--command") == 0) {
            if (i + 1 < argc) {
                command = argv[++i];
            } else {
                fprintf(stderr, "Error: Command argument missing\n");
                print_help();
                return 1;
            }
        } else if (strcmp(argv[i], "-h") == 0 || strcmp(argv[i], "--host") == 0) {
            if (i + 1 < argc) {
                host = argv[++i];
            } else {
                fprintf(stderr, "Error: Host argument missing\n");
                print_help();
                return 1;
            }
        } else if (strcmp(argv[i], "-p") == 0 || strcmp(argv[i], "--port") == 0) {
            if (i + 1 < argc) {
                port = atoi(argv[++i]);
            } else {
                fprintf(stderr, "Error: Port argument missing\n");
                print_help();
                return 1;
            }
        } else if (strcmp(argv[i], "-s") == 0 || strcmp(argv[i], "--stream") == 0) {
            socket_type = SOCK_STREAM;
        } else if (strcmp(argv[i], "-d") == 0 || strcmp(argv[i], "--dgram") == 0) {
            socket_type = SOCK_DGRAM;
        } else if (strcmp(argv[i], "--help") == 0) {
            print_help();
            return 0;
        }
    }
    
    if (!command) {
        fprintf(stderr, "Error: Command not specified\n");
        print_help();
        return 1;
    }
    
    // получение имени пользователя
    username = getenv("USER");
    if (!username) {
        fprintf(stderr, "Error: Could not determine username\n");
        return 1;
    }
    
    // создание сокета
    int sockfd = socket(AF_INET, socket_type, 0);
    if (sockfd < 0) {
        mysyslog("Failed to create socket", ERROR, 1, 0, "/var/log/myRPC.log");
        perror("socket");
        return 1;
    }

    struct hostent *server = gethostbyname(host);
    if (server == NULL) {
        mysyslog("Failed to resolve host", ERROR, 1, 0, "/var/log/myRPC.log");
        fprintf(stderr, "Error: No such host\n");
        close(sockfd);
        return 1;
    }

    // настройка адреса сервера. заполнение структуры адреса сервера~
    struct sockaddr_in serv_addr;
    memset(&serv_addr, 0, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    memcpy(&serv_addr.sin_addr.s_addr, server->h_addr, server->h_length);
    serv_addr.sin_port = htons(port);
    
    // соединение с сервером
    if (connect(sockfd, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0) {
        mysyslog("Failed to connect to server", ERROR, 1, 0, "/var/log/myRPC.log");
        perror("connect");
        close(sockfd);
        return 1;
    }
    
    // создание запроста в формате: "username" : "command"
    char request[BUFFER_SIZE];
    snprintf(request, BUFFER_SIZE, "\"%s\": \"%s\"", username, command);
    
    // отправка этого запроса
    if (send(sockfd, request, strlen(request), 0) < 0) {
        mysyslog("Failed to send request", ERROR, 1, 0, "/var/log/myRPC.log");
        perror("send");
        close(sockfd);
        return 1;
    }
    
    // получение ответа от севервера
    char response[BUFFER_SIZE];
    int n = recv(sockfd, response, BUFFER_SIZE - 1, 0);
    if (n < 0) {
        mysyslog("Faled to receive response", ERROR, 1, 0, "/var/log/myRPC.log");
        perror("recv");
        close(sockfd);
        return 1;
    }
    response[n] = '\0';
    
    // обработка ответа
    int code;
    char result[BUFFER_SIZE];
    if (sscanf(response, "%d: \"%[^\"]\"", &code, result) != 2) {
        fprintf(stderr, "Inavlid response format from server\n");
        close(sockfd);
        return 1;
    }
    
    if (code == 0) {
        printf("Command executed successfully:\n%s\n", result);
    } else {
        printf("Error executing command:\n%s\n", result);
    }
    
    close(sockfd);
    return 0;
}
