// server.c 
// Developed by: 
// João Araujo - 201705577
// Leonor Sousa - 201705377
// Afonso Mendonça - 201706708
// SOPE - MIEIC2 - 2018/2019
#include <pthread.h> 
#include <string.h>
#include "log_writing.h"
#include "utils.h"

int server_fifo_fd;

static bank_account_t accounts[MAX_BANK_ACCOUNTS]

bank_account_t createAccount(int id, int balance, char* password, int thread_id)
{
    bank_account_t account;
    account.account_id=id;
    account.balance=balance;

    char salt[SALT_LEN+1];
    produceSalt(salt);
    strcpy(account.salt, salt);

    char salt_plus_password[strlen(password)+SALT_LEN+1];
    strcpy(salt_plus_password, password);
    strcat(salt_plus_password, salt);

    char hash[HASH_LEN+1];
    produceSha(salt_plus_password, hash);
    strcpy(account.hash, hash);

    accountCreationLogWriting(&account, thread_id);

    return account;
}

int argument_handler(int argc, char* argv[])
{
    if (argc!=3)
    {
        printf("Number of arguments unexpected.\nPlease use the following syntax: ./server <number_eletronic_counters> \"<password>\"\n");
        exit(1);
    }
    int number_counters = atoi(argv[1]);
    char password[MAX_PASSWORD_LEN+1];
    strcpy(password,argv[2]);
    if (strlen(password)<MIN_PASSWORD_LEN || strlen(password)>MAX_PASSWORD_LEN)
    {
        printf("Password should have a length between %d and %d.\n", MIN_PASSWORD_LEN, MAX_PASSWORD_LEN);
        exit(2);
    } 

    //inicializing administrator account
    accounts[ADMIN_ACCOUNT_ID]=createAccount(ADMIN_ACCOUNT_ID, 0, password, 0);

    return number_counters;
}

int main(int argc, char* argv[])
{
    int number_counters = argument_handler(argc, argv);

    int fd_dummy;
    createFifo(SERVER_FIFO_PATH);
    server_fifo_fd = openReadFifo(SERVER_FIFO_PATH, &fd_dummy);

    for (int i=0; i<number_counters; i++)
    {
        void bankOfficeOpenLogWriting(i);
    }

    closeUnlinkFifo(SERVER_FIFO_PATH, fd, fd_dummy)

    return 0;
}