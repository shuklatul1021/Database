TARGET = bin/database_app
SRC = src/main.c src/file.c src/parse.c
OBJ = $(SRC:.c=.o)

run : clean default
	./$(TARGET) -f ./testdb.db -n
	./$(TARGET) -f ./testdb.db

default : $(TARGET)

clean :
	rm -f $(OBJ) $(TARGET)
	rm -f bin/*.db
	rm -f *.db

${TARGET} : $(OBJ)
	gcc -o $@ $^

obj/%.o : src/%.c
	gcc -Iinclude -c -o $@ $<