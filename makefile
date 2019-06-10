CC = gcc
SRC_DIR = src
SRC = $(wildcard $(SRC_DIR)/*.c)
CFLAGS = -Wall -lncursesw

out: 
	$(CC) -o out $(SRC) $(CFLAGS) 
