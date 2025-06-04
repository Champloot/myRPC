#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/wait.h>
#include <time.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <ctype.h>
#include "../common/libmysyslog.h"

#define BUFFER_SIZE 1024
#define MAX_USERS 100
#define TEMP_FILE_PATTERN "/tmp/myRPC_%06d"

// конфигурация.
// тут идет чтение конфигурации. в частности порт и тип сокета.
int read_config(const char *config_path, int *port, int *socket_type) {
    FILE *config_file = fopen(config_path, "r");
    if (!config_file) {
        return -1;
    }
    
    char line[256];
    *port = 1234; // default
    *socket_type = SOCK_STREAM; // default
    
    while (fgets(line, sizeof(line), config_file)) {
        if (strstr(line, "port = ")) {
            *port = atoi(line + 7);
        } else if (strstr(line, "socket_type = ")) {
            if (strstr(line + 14, "dgram")) {
                *socket_type = SOCK_DGRAM;
            }
        }
    }
    
    fclose(config_file);
    return 0;
}

// проверка пользователя в конфиге
int is_user_allowed(const char *username, const char *users_conf_path) {
    FILE *users_file = fopen(users_conf_path, "r");
    if (!users_file) {
        return 0;
    }
    
    char line[256];
    while (fgets(line, sizeof(line), users_file)) {
        line[strcspn(line, "\n")] = '\0';
        
        if (strcmp(line, username) == 0) {
            fclose(users_file);
            return 1;
        }
    }
    
    fclose(users_file);
    return 0;
}

// функция выполнения команд:
// создание временных файлов
// дальше чтение этих самых временных файлов
// потом конечно удаление этих фалов
// и обработка ошибок, если они были
void execute_command(const char *command, char *output, int output_size) {
    
    char stdout_file[64], stderr_file[64];
    pid_t pid = getpid();
    snprintf(stdout_file, sizeof(stdout_file), TEMP_FILE_PATTERN ".stdout", pid);
    snprintf(stderr_file, sizeof(stderr_file), TEMP_FILE_PATTERN ".stderr", pid);
    
    char full_command[BUFFER_SIZE * 2];
    snprintf(full_command, sizeof(full_command), "%s > %s 2> %s", command, stdout_file, stderr_file);
    
    int status = system(full_command);
    
    FILE *out_file = fopen(stdout_file, "r");
    if (out_file) {
        fread(output, 1, output_size - 1, out_file);
        fclose(out_file);
        unlink(stdout_file);
    }
    
    if (status != 0) {
        strcat(output, "\nErrors:\n");
        FILE *err_file = fopen(stderr_file, "r");
        if (err_file) {
            size_t len = strlen(output);
            fread(output + len, 1, output_size - len - 1, err_file);
            fclose(err_file);
            unlink(stderr_file);
        }
    }
}

int main(int argc, char *argv[]) {
    int port, socket_type;
    const char *config_path = "/etc/myRPC/myRPC.conf";
    const char *users_conf_path = "/etc/myRPC/users.conf";

    // инициализация: загрузка онфигурации, создание соркета, привязка к порту ...
    if (read_config(config_path, &port, &socket_type) < 0) {
        mysyslog("Failed to read config file", CRITICAL, 0, 0, "/var/log/myRPC.log");
        fprintf(stderr, "Error: Could not read config file %s\n", config_path);
        return 1;
    }
    
    int sockfd = socket(AF_INET, socket_type, 0);
    if (sockfd < 0) {
        mysyslog("Failed to create socket", CRITICAL, 0, 0, "/var/log/myRPC.log");
        perror("socket");
        return 1;
    }
    
    struct sockaddr_in serv_addr;
    memset(&serv_addr, 0, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = INADDR_ANY;
    serv_addr.sin_port = htons(port);
    
    if (bind(sockfd, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0) {
        mysyslog("Failed to bind socket", CRITICAL, 0, 0, "/var/log/myRPC.log");
        perror("bind");
        close(sockfd);
        return 1;
    }
    
    if (socket_type == SOCK_STREAM) {
        listen(sockfd, 5);
    }
    
    printf("myRPC server started on port %d (%s)\n", 
           port, socket_type == SOCK_STREAM ? "TCP" : "UDP");

    // основной цикл обработки
    while (1) {
        struct sockaddr_in cli_addr;
        socklen_t clilen = sizeof(cli_addr);
        int newsockfd;

        // если tcp, то использую accept() для новго соединения
        // если udp то исходный сокет
        if (socket_type == SOCK_STREAM) {
            newsockfd = accept(sockfd, (struct sockaddr *)&cli_addr, &clilen);
            if (newsockfd < 0) {
                mysyslog("Failed to accept connection", ERROR, 0, 0, "/var/log/myRPC.log");
                perror("accept");
                continue;
            }
        } else {
            newsockfd = sockfd;
        }
        
        char buffer[BUFFER_SIZE];
        int n;
        
        if (socket_type == SOCK_STREAM) {
            n = recv(newsockfd, buffer, BUFFER_SIZE - 1, 0);
        } else {
            n = recvfrom(newsockfd, buffer, BUFFER_SIZE - 1, 0, 
                         (struct sockaddr *)&cli_addr, &clilen);
        }
        
        if (n < 0) {
            mysyslog("Failed to receive data", ERROR, 0, 0, "/var/log/myRPC.log");
            perror("recv");
            if (socket_type == SOCK_STREAM) {
                close(newsockfd);
            }
            continue;
        }
        buffer[n] = '\0';
        
        // разбор запроса. ожидается что он будет в формате "username" : "command"
        char username[256], command[BUFFER_SIZE];
        if (sscanf(buffer, "\"%[^\"]\": \"%[^\"]\"", username, command) != 2) {
            mysyslog("Invalid request format", WARN, 0, 0, "/var/log/myRPC.log");
            const char *response = "1: \"Invalid request format\"";
            if (socket_type == SOCK_STREAM) {
                send(newsockfd, response, strlen(response), 0);
                close(newsockfd);
            } else {
                sendto(newsockfd, response, strlen(response), 0, 
                      (struct sockaddr *)&cli_addr, clilen);
            }
            continue;
        }
        
        // проверка првав пользователя
        if (!is_user_allowed(username, users_conf_path)) {
            mysyslog("Unauthorized access attempt", WARN, 0, 0, "/var/log/myRPC.log");
            char response[BUFFER_SIZE];
            snprintf(response, sizeof(response), "1: \"User %s is not authorized\"", username);
            if (socket_type == SOCK_STREAM) {
                send(newsockfd, response, strlen(response), 0);
                close(newsockfd);
            } else {
                sendto(newsockfd, response, strlen(response), 0, 
                      (struct sockaddr *)&cli_addr, clilen);
            }
            continue;
        }
        
        // выполнение команд
        char output[BUFFER_SIZE] = {0};
        execute_command(command, output, sizeof(output));
        
        // формирование ответа: 0 - успех, 1 - ошибка (и её описание)
        char response[BUFFER_SIZE * 2];
        snprintf(response, sizeof(response), "0: \"%s\"", output);
        
        if (socket_type == SOCK_STREAM) {
            send(newsockfd, response, strlen(response), 0);
            close(newsockfd);
        } else {
            sendto(newsockfd, response, strlen(response), 0, 
                  (struct sockaddr *)&cli_addr, clilen);
        }
        
        // логирование
        char log_msg[BUFFER_SIZE * 2];
        snprintf(log_msg, sizeof(log_msg), "User %s executed command: %s", username, command);
        mysyslog(log_msg, INFO, 0, 0, "/var/log/myRPC.log");
    }
    
    close(sockfd);
    return 0;
}
