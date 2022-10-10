TARGET = ymirdb

CC = gcc

CFLAGS = -c -Wall -Wvla -Werror -g -Werror=format-security -std=c11 -DCMAKE_EXE_LINKER_FLAGS="-fsanitize=address"
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
