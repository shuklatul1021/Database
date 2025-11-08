#ifndef PARSE_H
#define PARSE_H
#define HEADER_MAGIC 0x4c4c4144

struct db_header
{
    unsigned int magic;
    unsigned int version;
    unsigned int count;
    unsigned int filesize;
};

struct employee_t
{
    char name[256];
    char address[256];
    unsigned int hours;
};

int create_db_header(int fd , struct db_header **head_out);
int validate_db_header(int fd , struct db_header **head_out);
int read_employee(int fd , struct db_header *head_out , struct employee_t **employee_out);
void output_employee(int fd , struct db_header *header , struct employee_t *employee);
void list_employees(struct db_header *header , struct employee_t *employee);

#endif