#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <jansson.h>
#include <pwd.h>
#include "mysyslog.h"

#define BUFFER_SIZE 1024

void send_request(int sockfd, const char *login, const char *command) {
    json_t *root = json_object();
    json_object_set_new(root, "login", json_string(login));
    json_object_set_new(root, "command", json_string(command));
    
    char *request = json_dumps(root, JSON_COMPACT);
    send(sockfd, request, strlen(request), 0);
    
    free(request);
    json_decref(root);
}

int main(int argc, char *argv[]) {
    // Парсинг аргументов командной строки
    // ... (использование getopt_long)

    // Получение текущего пользователя
    struct passwd *pw = getpwuid(getuid());
    const char *login = pw->pw_name;

    // Создание сокета
    int sockfd = socket(AF_INET, SOCK_STREAM, 0);
    struct sockaddr_in serv_addr = {
        .sin_family = AF_INET,
        .sin_port = htons(port),
        .sin_addr.s_addr = inet_addr(host)
    };

    if (connect(sockfd, (struct sockaddr*)&serv_addr, sizeof(serv_addr)) < 0) {
        mysyslog(LOG_ERR, "Connection failed");
        exit(EXIT_FAILURE);
    }

    send_request(sockfd, login, command);
    
    // Получение ответа
    char buffer[BUFFER_SIZE];
    recv(sockfd, buffer, BUFFER_SIZE, 0);
    
    json_error_t error;
    json_t *root = json_loads(buffer, 0, &error);
    int code = json_integer_value(json_object_get(root, "code"));
    const char *result = json_string_value(json_object_get(root, "result"));
    
    printf("Code: %d\nResult: %s\n", code, result);
    
    close(sockfd);
    return 0;
}
