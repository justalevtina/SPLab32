#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#define BUF_SIZE 1024
#define PORT 8080

int main()
{
    int client_fd;
    struct sockaddr_in server_addr;
    char buffer[BUF_SIZE];

    // Создание сетевого сокета
    client_fd = socket(PF_INET, SOCK_DGRAM, 0);
    if (client_fd == -1)
    {
        perror("socket error");
        exit(1);
    }

    // Заполнение структуры адреса
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = inet_addr("127.0.0.1"); // Адрес сервера (в данном случае localhost)
    server_addr.sin_port = htons(PORT);

    // Отправка сообщения серверу
    if (sendto(client_fd, "Привет от Тани!", strlen("Привет от Тани!"), 0, (struct sockaddr *)&server_addr, sizeof(server_addr)) == -1)
    {
        perror("sendto error");
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

