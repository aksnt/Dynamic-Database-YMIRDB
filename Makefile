TARGET = ymirdb

CC = clang

CFLAGS = -c -Wall -Wvla -Werror -g -Werror=format-security -std=c11 #-fsanitize=address
SRC = ymirdb.c
OBJ = $(SRC:.c=.o)

all:$(TARGET)

$(TARGET):$(OBJ)
	$(CC) -o $@ $(OBJ) 

.SUFFIXES: .c .o

.c.o:
	 $(CC) $(CFLAGS) $<

test:
	bash test.sh

clean:
	rm -f *.o *.obj $(TARGET)
