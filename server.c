#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <sys/un.h>

#define BUF_SIZE 1024
#define SOCK_NAME "mysocket"
#define PORT 8080

int main()
{
    int server_fd, client_fd;
    struct sockaddr_un server_addr, client_addr;
    socklen_t client_addr_len;
    char buffer[BUF_SIZE];
    int optval = 1;

    // Создание Unix-сокета
    server_fd = socket(PF_UNIX, SOCK_STREAM, 0);
    if (server_fd == -1)
    {
        perror("socket error");
        exit(1);
    }

    // Установка опции для повторного использования адреса
    if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &optval, sizeof(optval)) == -1)
    {
        perror("setsockopt error");
        exit(1);
    }

    // Заполнение структуры адреса
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sun_family = AF_UNIX;
    strcpy(server_addr.sun_path, SOCK_NAME);

    // Привязка сокета к адресу
    if (bind(server_fd, (struct sockaddr *)&server_addr, sizeof(server_addr)) == -1)
    {
        perror("bind error");
        exit(1);
    }

    // Прослушивание подключений
    if (listen(server_fd, 5) == -1)
    {
        perror("listen error");
        exit(1);
    }

    // Создание сокета для сетевой связи
    int inet_fd = socket(PF_INET, SOCK_DGRAM, 0);
    if (inet_fd == -1)
    {
        perror("socket error");
        exit(1);
    }

    // Заполнение структуры адреса для сетевого сокета
    struct sockaddr_in inet_addr;
    memset(&inet_addr, 0, sizeof(inet_addr));
    inet_addr.sin_family = AF_INET;
    inet_addr.sin_addr.s_addr = INADDR_ANY;
    inet_addr.sin_port = htons(PORT);

    // Привязка сокета к адресу
    if (bind(inet_fd, (struct sockaddr *)&inet_addr, sizeof(inet_addr)) == -1)
    {
        perror("bind error");
        exit(1);
    }

    while (1)
    {
        printf("Сервер ожидает подключения...\n");
        client_addr_len = sizeof(client_addr);

        // Принятие соединения от клиента (Unix-сокет)
        client_fd = accept(server_fd, (struct sockaddr *)&client_addr, &client_addr_len);
        if (client_fd == -1)
        {
            perror("accept error");
            continue;
        }

        // Прием данных от клиента (Unix-сокет)
        if (recv(client_fd, buffer, BUF_SIZE, 0) == -1)
        {
            perror("recv error");
            close(client_fd);
            continue;
        }

        // Отправка ответа клиенту (Unix-сокет)
        if (send(client_fd, "Сообщение получено!", strlen("Сообщение получено!"), 0) == -1)
        {
            perror("send error");
            close(client_fd);
            continue;
        }

        // Прием данных от клиента (сетевой сокет)
        if (recvfrom(inet_fd, buffer, BUF_SIZE, 0, (struct sockaddr *)&client_addr, &client_addr_len) == -1)
        {
            perror("recv from error");
            close(client_fd);
            continue;
        }

        printf("Получено сообщение от клиента: %s\n", buffer);

        // Отправка ответа клиенту (сетевой сокет)
        if (sendto(inet_fd, "Сообщение получено!", strlen("Сообщение получено!"), 0, (struct sockaddr *)&client_addr, client_addr_len) == -1)
        {
            perror("send to error");
            close(client_fd);
            continue;
        }

        close(client_fd);
    }

    close(server_fd);
    unlink(SOCK_NAME);

    return 0;
}

