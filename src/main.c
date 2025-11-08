#include <stdio.h>
#include<stdbool.h>
#include <getopt.h>
#include <stdlib.h>
#include <unistd.h>

#include "parse.h"
#include "file.h"

void define_usage(char *argv[]){
    printf("Usage : \n -n -f <databasefile>\n" , argv[0]);
    printf("\t -n - Createing New Database\n");
    printf("\t -f - [require] path to the database \n" , argv);
}

int main(int argc , char *argv[]) {
    int c;
    bool newFile = false;
    char *filename = NULL;
    char *addstring = NULL;
    bool lFlag = false;

    struct db_header *header = NULL;
    struct employee_t *employee = NULL;
    int dbfd = -1;
    while ((c = getopt(argc , argv , "nf:a:l:")) != -1)
    {
       switch (c){
            case 'n':
                newFile = true;
                break;
            case 'f':
                filename = optarg;
                break;
            case 'a':
                addstring = optarg;
                break;
            case 'l':
                lFlag = true;
                break;
            case '?':
                printf("Unknown Option  -%c\n" ,c);
                break;
            default:
                return -1;
       }
    }

    if(filename == NULL){
        printf("File Path Is Require:\n");
        define_usage(argv);
    }

    if(newFile){
        dbfd = create_db_file(filename);
        if(dbfd == -1){
            printf("Unable To Create file Because File Already Exist");
            return -1;
        }
        if((create_db_header(dbfd ,  &header)) == -1){
            printf("Failed To create Database Error\n");
            return -1;
        }
    }else{
        dbfd = open_db_file(filename);
        if(dbfd == -1){
            printf("Unable To Create file Because File Already Exist");
            return -1;
        }

        if((validate_db_header(dbfd , &header)) == -1){
            printf("Failed To Validate Database Error\n");
            return -1;
        }   
    }

    if(read_employee(dbfd , header , &employee) == -1){
        printf("Failed To Read Employee\n");
        return -1;
    }

    if(addstring != NULL){
        header->count += 1;
        employee = realloc(employee , header->count * sizeof(struct employee_t));
        add_employee(header , employee , addstring);
    }

    if(lFlag){
        printf("Employee List:\n");
        list_employees(header , employee);
    }
    output_employee(dbfd , header , employee);

}