#include <stdio.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>

void DieWithError(char *errorMessage)
{
    perror(errorMessage);
    exit(1);
}

typedef struct {
    int row_number;
    int bookshelf_number;
    int book_numer;
    int id;
} Book;

typedef struct {
    Book* books;
    int num_books;
} Catalog;

#define MAXPENDING 3

void bubbleSort(Catalog *lib) {
    Book *arr = lib->books;
    int n = lib->num_books;
    int i, j;
    Book temp;
    for (i = 0; i < n-1; i++) {
        for (j = 0; j < n-i-1; j++) {
            if (arr[j].id > arr[j+1].id) {
                temp = arr[j];
                arr[j] = arr[j+1];
                arr[j+1] = temp;
            }
        }
    }
}

int main(int argc, char *argv[])
{
    int servSock;
    int clntSock;
    struct sockaddr_in echoServAddr;
    struct sockaddr_in echoClntAddr;
    unsigned short echoServPort;
    unsigned int clntLen;
    Catalog library;
    int amount_of_books = atoi(argv[2]);
    library.books = (Book*) malloc(amount_of_books);
    if (argc != 2)
    {
        fprintf(stderr, "Используйте порт сервера\n", argv[0]);
        exit(1);
    }
    echoServPort = atoi(argv[1]);
    if ((servSock = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP)) < 0)
        DieWithError("socket() failed");
    memset(&echoServAddr, 0, sizeof(echoServAddr));
    echoServAddr.sin_family = AF_INET;
    echoServAddr.sin_addr.s_addr = htonl(INADDR_ANY);
    echoServAddr.sin_port = htons(echoServPort);
    if (bind(servSock, (struct sockaddr *) &echoServAddr, sizeof(echoServAddr)) < 0)
        DieWithError("bind() failed");
    printf("Айпи сервера = %s. Wait...\n", inet_ntoa(echoClntAddr.sin_addr));
    if (listen(servSock, MAXPENDING) < 0)
        DieWithError("listen() failed");
    while (amount_of_books != library.num_books)
    {
        clntLen = sizeof(echoClntAddr);
        if ((clntSock = accept(servSock, (struct sockaddr *) &echoClntAddr, &clntLen)) < 0)
            DieWithError("accept() failed");
        printf("Обработка клиента %s\n", inet_ntoa(echoClntAddr.sin_addr));
        Book bookBuffer;
        int recvMsgSize;
        if ((recvMsgSize = recv(clntSock, &bookBuffer, sizeof(Book), 0)) < 0)
            DieWithError("recv() failed");
        while (recvMsgSize > 0)
        {
            if ((recvMsgSize = recv(clntSock, &bookBuffer, sizeof(Book), 0)) < 0)
                DieWithError("recv() failed");
        }
        library.books[library.num_books] = bookBuffer;
        ++library.num_books;
        bubbleSort(&library);
        close(clntSock);
    }
}