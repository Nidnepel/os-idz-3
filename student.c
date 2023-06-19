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

int main(int argc, char *argv[])
{
    int sock;
    struct sockaddr_in echoServAddr;
    unsigned short echoServPort;
    char *servIP;
    unsigned int sizeOfBook;

    // Указатель на файл.
    FILE *fpc;
    // Название файла ввода.
    char* filename;
    if (argc == 4) {
        filename = argv[1];
    }
    fpc = fopen(filename, "r");
    if (fpc == NULL) {
        printf("Error opening file!\n");
        return 1;
    }
    int a, b, c, d;
    int books_amount = 0;
    while (fscanf(fpc, "%d %d %d %d", &a, &b, &c, &d) != EOF) {
        ++books_amount;
    }
    fclose(fpc);
    FILE *fp;
    fp = fopen(filename, "r");
    if (fp == NULL) {
        printf("Error opening file!\n");
        return 1;
    }
    Catalog books;
    books.books = (Book*) malloc(books_amount);
    int iterator = 0;
    while (fscanf(fp, "%d %d %d %d", &books.books->row_number, &books.books->bookshelf_number, &books.books->book_numer, &books.books->id) != EOF) {
        printf("%d %d %d %d\n", books.books->row_number, books.books->bookshelf_number, books.books->book_numer, books.books->id);
        ++iterator;
    }
    fclose(fp);
    printf("Input done.\n");
    servIP = argv[1];
    if (argc == 4)
        echoServPort = atoi(argv[3]);
    else
        echoServPort = 7;
    if ((sock = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP)) < 0)
        DieWithError("socket() failed");
    memset(&echoServAddr, 0, sizeof(echoServAddr));
    echoServAddr.sin_family      = AF_INET;
    echoServAddr.sin_addr.s_addr = inet_addr(servIP);
    echoServAddr.sin_port        = htons(echoServPort);
    if (connect(sock, (struct sockaddr *) &echoServAddr, sizeof(echoServAddr)) < 0)
        DieWithError("connect() failed");
    sizeOfBook = sizeof(Book);
    pid_t pid1 = fork();
    if (pid1 == -1) {
        printf("Ошибка при создании дочернего процесса\n");
        exit(-1);
    } else if (pid1 == 0) {
        pid_t pid2 = fork();
        if (pid2 == 0) {
            iterator = 2;
            if (books_amount > 1) {
                do {
                    if (send(sock, &books.books[iterator], sizeOfBook, 0) != sizeOfBook)
                        DieWithError("send() sent a different number of bytes than expected");
                    printf("\n");
                    iterator += 3;
                } while (iterator < books_amount);

                close(sock);
                exit(0);
            }
        } else {
            iterator = 1;
            if (books_amount > 0) {
                do {
                    if (send(sock, &books.books[iterator], sizeOfBook, 0) != sizeOfBook)
                        DieWithError("send() sent a different number of bytes than expected");
                    printf("\n");
                    iterator += 3;
                } while (iterator < books_amount);

                close(sock);
                exit(0);
            }
        }
    } else {
        iterator = 0;
        do {
            if (send(sock, &books.books[iterator], sizeOfBook, 0) != sizeOfBook)
                DieWithError("send() sent a different number of bytes than expected");
            printf("\n");
            iterator += 3;
        } while (iterator < books_amount);
        close(sock);
        exit(0);
    }
}
