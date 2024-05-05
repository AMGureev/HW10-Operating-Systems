#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>

#define MAX_MESSAGE_SIZE 1024

void error(const char *msg) {
    perror(msg);
    exit(1);
}

int main(int argc, char *argv[]) {
    // Проверяем, что пользователь ввёл IP адрес и порт
    if (argc != 3) {
        fprintf(stderr,"Usage: %s <server_ip> <server_port>\n", argv[0]);
        exit(1);
    }

    int sockfd, portno, n;
    struct sockaddr_in serv_addr;
    char buffer[MAX_MESSAGE_SIZE];

    // Получаем порт из аргументов командной строки
    portno = atoi(argv[2]);
    // Создаём сокет
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0) {
        error("ERROR opening socket");
    }

    // Заполняем структуру serv_addr
    memset(&serv_addr, '0', sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(portno);

    // Преобразуем IP адрес из текстового вида в сетевой формат
    if (inet_pton(AF_INET, argv[1], &serv_addr.sin_addr) <= 0) {
        error("Invalid address");
    }

    // Устанавливаем соединение с сервером
    if (connect(sockfd, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0) {
        error("ERROR connecting");
    }

    // Соединение установлено
    printf("Connected to server. Start typing messages. Type 'The End' to quit.\n");
    while (1) {
        printf("Enter message: ");
        // Очищаем буфер
        bzero(buffer, MAX_MESSAGE_SIZE);
        // Получаем ввод от пользователя
        fgets(buffer, MAX_MESSAGE_SIZE, stdin);
        // Отправляем сообщение на сервер
        n = write(sockfd, buffer, strlen(buffer));
        if (n < 0) {
            error("ERROR writing to socket");
        }
        // Если введено "The End\n", завершаем цикл
        if (strcmp(buffer, "The End\n") == 0) {
            break;
        }
    }

    // Закрываем сокет
    close(sockfd);
    return 0;
}
