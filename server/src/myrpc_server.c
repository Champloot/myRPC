#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <jansson.h>
#include <pwd.h>
#include "mysyslog.h"

#define MAX_USERS 100
#define BUFFER_SIZE 1024

char *allowed_users[MAX_USERS];
int load_users(const char *path) {
    FILE *file = fopen(path, "r");
    char line[256];
    int count = 0;
    while (fgets(line, sizeof(line), file)) {
        line[strcspn(line, "\n")] = 0;
        allowed_users[count++] = strdup(line);
    }
    fclose(file);
    return count;
}

void execute_command(const char *command, char **result) {
    FILE *fp = popen(command, "r");
    char buffer[BUFFER_SIZE];
    *result = malloc(BUFFER_SIZE);
    while (fgets(buffer, BUFFER_SIZE, fp)) {
        strcat(*result, buffer);
    }
    pclose(fp);
}

int main() {
    // Загрузка конфигурации
    int port = 1234;
    int sock_type = SOCK_STREAM;
    
    int num_users = load_users("/etc/myRPC/users.conf");
    
    // Создание сокета
    int sockfd = socket(AF_INET, sock_type, 0);
    struct sockaddr_in serv_addr = {
        .sin_family = AF_INET,
        .sin_port = htons(port),
        .sin_addr.s_addr = INADDR_ANY
    };
    
    bind(sockfd, (struct sockaddr*)&serv_addr, sizeof(serv_addr));
    listen(sockfd, 5);
    
    while (1) {
        int client_fd = accept(sockfd, NULL, NULL);
        char buffer[BUFFER_SIZE];
        recv(client_fd, buffer, BUFFER_SIZE, 0);
        
        json_error_t error;
        json_t *root = json_loads(buffer, 0, &error);
        const char *login = json_string_value(json_object_get(root, "login"));
        const char *command = json_string_value(json_object_get(root, "command"));
        
        // Проверка пользователя
        int allowed = 0;
        for (int i = 0; i < num_users; i++) {
            if (strcmp(login, allowed_users[i]) == 0) {
                allowed = 1;
                break;
            }
        }
        
        json_t *response = json_object();
        if (allowed) {
            char *result;
            execute_command(command, &result);
            json_object_set_new(response, "code", json_integer(0));
            json_object_set_new(response, "result", json_string(result));
            free(result);
        } else {
            json_object_set_new(response, "code", json_integer(1));
            json_object_set_new(response, "result", json_string("Access denied"));
        }
        
        char *response_str = json_dumps(response, JSON_COMPACT);
        send(client_fd, response_str, strlen(response_str), 0);
        
        free(response_str);
        json_decref(response);
        close(client_fd);
    }
    
    return 0;
}
