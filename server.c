#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>

#define MAX_MESSAGE_SIZE 1024
#define BACKLOG 5

void error(const char *msg) {
    perror(msg);
    exit(1);
}

int main(int argc, char *argv[]) {
    // Проверяем, ввел ли пользователь IP адрес и порт
    if (argc != 3) {
        fprintf(stderr,"Usage: %s <server_ip> <server_port>\n", argv[0]);
        exit(1);
    }

    int sockfd, newsockfd, portno, clilen;
    char buffer[MAX_MESSAGE_SIZE];
    struct sockaddr_in serv_addr, cli_addr;
    int n;

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
    serv_addr.sin_addr.s_addr = INADDR_ANY;
    serv_addr.sin_port = htons(portno);

    // Привязываем сокет к адресу
    if (bind(sockfd, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0) {
        error("ERROR on binding");
    }

    // Начинаем прослушивание сокета
    listen(sockfd, BACKLOG);
    clilen = sizeof(cli_addr);

    // Принимаем входящее соединение от клиента 1
    newsockfd = accept(sockfd, (struct sockaddr *) &cli_addr, &clilen);
    if (newsockfd < 0) {
        error("ERROR on accept");
    }

    // Соединяемся с клиентом 2
    int sockfd2;
    struct sockaddr_in serv_addr2;

    sockfd2 = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd2 < 0) {
        error("ERROR opening socket");
    }

    memset(&serv_addr2, '0', sizeof(serv_addr2));
    serv_addr2.sin_family = AF_INET;
    serv_addr2.sin_port = htons(portno); // Используем тот же порт, что и клиент 1
    if (inet_pton(AF_INET, argv[1], &serv_addr2.sin_addr) <= 0) {
        error("Invalid address");
    }

    if (connect(sockfd2, (struct sockaddr *) &serv_addr2, sizeof(serv_addr2)) < 0) {
        error("ERROR connecting to client 2");
    }

    // Цикл обмена сообщениями
    while (1) {
        // Читаем сообщение от клиента 1
        bzero(buffer, MAX_MESSAGE_SIZE);
        n = read(newsockfd, buffer, MAX_MESSAGE_SIZE);
        if (n < 0) {
            error("ERROR reading from socket");
        }
        printf("Client 1: %s", buffer);

        // Пересылаем сообщение клиенту 2
        n = write(sockfd2, buffer, strlen(buffer));
        if (n < 0) {
            error("ERROR writing to socket");
        }

        // Завершаем цикл
        if (strcmp(buffer, "The End\n") == 0) {
            break;
        }
    }

    // Закрываем сокеты
    close(newsockfd);
    close(sockfd);
    close(sockfd2);
    return 0;
}
