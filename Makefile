TARGET=bin2cstr
CC=gcc
CFLAGS+=-Os
SOURCES=main.c
OBJS=$(patsubst %.c,%.o,$(SOURCES))

$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) -o $(TARGET) $(OBJS)

%.o: %.c
	$(CC) $(CFLAGS) -c $<

.PHONY: clean install

PREFIX=~/.local

install: $(TARGET)
	install -d $(PREFIX)/bin/
	cp $(TARGET) $(PREFIX)/bin/

clean:
	rm -f $(TARGET) $(OBJS)
