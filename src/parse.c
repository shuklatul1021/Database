#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <string.h>
#include <winsock2.h>

#include "parse.h"
#include "file.h"

void output_employee(int fd , struct db_header *header , struct employee_t *employee){
    if(fd < 0 || header == NULL || employee == NULL){
        printf("Bad FD or Header or Employee\n");
        return;
    }

    int realCount = header->count;

    header->magic = htonl(header->magic);
    header->version = htons(header->version);
    header->count = htons(header->count);
    header->filesize = htonl(sizeof(struct db_header) + (realCount * sizeof(struct employee_t)));

    lseek(fd , 0 , SEEK_SET);
    int result = write(fd , header , sizeof(struct db_header));
    if(result != sizeof(struct db_header)){
        printf("Failed To Write DB Header\n");
        return;
    }
    int i = 0;
    for( ; i <  realCount ; i++){
        employee[i].hours = htonl(employee[i].hours);
        write(fd , &employee[i] , sizeof(struct employee_t));
    } 

    return;
}

int create_db_header(int fd , struct db_header **head_out){
    struct db_header *header = calloc(1 , sizeof(struct db_header));
    if(header == NULL){
        printf("Malloc Failed To Create DB Error\n");
        return -1;
    }
    header->version = 0x11;
    header->count = 0;
    header->magic = HEADER_MAGIC;
    header->filesize = sizeof(struct db_header);

    *head_out = header;
    return 0;
}
int validate_db_header(int fd , struct db_header **head_out){
    if(fd < 0){
        printf("Got Bad FD\n");
        return -1;
    }

    struct db_header *newheader = calloc(1 , sizeof(struct db_header));
    if(newheader == NULL){
        printf("Malloc Failed While Validating The header\n");
        return -1;
    }

    int result = read(fd , newheader , sizeof(struct db_header));
    if(result != sizeof(struct db_header)){
        printf("Failed To Read DB Header\n");
        free(newheader);
        return -1;
    }
    newheader->version = ntohs(newheader->version);
    newheader->count = ntohs(newheader->count);
    newheader->filesize = ntohl(newheader->filesize);
    newheader->magic = ntohl(newheader->magic);

    if(newheader->version != 1){
        printf("Invalid DB Header Version\n");
        free(newheader);
        return -1;
    }
    

    if(newheader->magic != HEADER_MAGIC){
        printf("Invalid DB Header Magic\n");
        free(newheader);
        return -1;
    }
    struct stat dbstat = {0};
    fstat(fd , &dbstat);
    
    if(newheader->filesize != dbstat.st_size){
        printf("Invalid DB Header Filesize\n");
        free(newheader);
        return -1;
    }

    *head_out = newheader;
    return 0;
}


int read_employee(int fd , struct db_header *head_out , struct employee_t **employee_out){
    if(fd < 0 || head_out == NULL || employee_out == NULL){
        printf("Bad FD or Header Out or Employee Out\n");
        return -1;
    }

    int count = head_out->count; 
    struct employee_t *employee = calloc(1 , sizeof(struct employee_t));

    if(employee == NULL){
        printf("Malloc Failed While Reading Employee\n");
        return -1;
    }

    int result = read(fd , employee , count * sizeof(struct employee_t));
    if(result != count * sizeof(struct employee_t)){
        printf("Failed To Read Employee From DB\n");
        free(employee);
        return -1;
    }

    int i = 0;
    for( ; i < count ; i++){
        employee[i].hours = ntohl(employee[i].hours);
    }

    *employee_out = employee;
    return 0;
}

int add_employee(struct db_header *header , struct employee_t *employee_out ,char *addstring){
    if(header == NULL || employee_out == NULL || addstring == NULL){
        printf("Bad Header or Employee Out or Add String\n");
        return -1;
    }

    char *name = strtok(addstring , ",");
    char *address = strtok(NULL , ",");
    char *hours_str = strtok(NULL , ",");
    strcpy(employee_out[header->count - 1].name , name);
    strcpy(employee_out[header->count - 1].address , address);
    employee_out[header->count - 1].hours = atoi(hours_str);

    return 0;
} 


void list_employees(struct db_header *header , struct employee_t *employee){
    if(header == NULL || employee == NULL){
        printf("Bad Header or Employee\n");
        return;
    }

    int count = header->count;
    int i = 0;
    for( ; i < count ; i++){
        printf("Employee %d:\n", i + 1);
        printf("Name: %s\n", employee[i].name);
        printf("Address: %s\n", employee[i].address);
        printf("Hours: %u\n", employee[i].hours);
        printf("---------------------\n");
    }
}