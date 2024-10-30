#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/un.h>

#define BUF_SIZE 1024
#define SOCK_NAME "mysocket"

int main()
{
    int client_fd;
    struct sockaddr_un server_addr;
    char buffer[BUF_SIZE];

    // Создание Unix-сокета
    client_fd = socket(PF_UNIX, SOCK_STREAM, 0);
    if (client_fd == -1)
    {
        perror("socket error");
        exit(1);
    }

    // Заполнение структуры адреса
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sun_family = AF_UNIX;
    strcpy(server_addr.sun_path, SOCK_NAME);

    // Подключение к серверу
    if (connect(client_fd, (struct sockaddr *)&server_addr, sizeof(server_addr)) == -1)
    {
        perror("connect error");
        exit(1);
    }

    // Отправка сообщения серверу
    if (send(client_fd, "Привет от Маши!", strlen("Привет от Маши!"), 0) == -1)
    {
        perror("send error");
        exit(1);
    }

    // Прием ответа от сервера
    if (recvfrom(client_fd, buffer, BUF_SIZE, 0, NULL, NULL) == -1)
    {
        perror("recvfrom error");
        exit(1);
    }

    printf("Ответ от сервера: %s\n", buffer);

    close(client_fd);

    return 0;
}

