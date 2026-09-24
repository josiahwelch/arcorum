TARGET := arcorum

CFLAGS := -Iinclude -I/usr/local/include -L/usr/local/lib
CC := cc
SRCS := src/arcorum.c src/lexer.c

.PHONY: all clean

all:
	mkdir -p build
	$(CC) $(CFLAGS) $(SRCS) -o ./build/$(TARGET)

run:
	./build/$(TARGET)

clean:
	rm -rf build
