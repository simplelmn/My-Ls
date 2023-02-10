CC = gcc
CFLAGS = -g -Wall -Wextra -Werror
SOURCES = tes.c
TARGET = my_ls

$(TARGET):$(SOURCES)
	$(CC) $(CFLAGS) -o $@ $^

.PHONY: fclean

fclean:
	@rm -r $(TARGET)