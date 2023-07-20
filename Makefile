CFLAGS = -g
LDFLAGS = -lpthread
CC = gcc

OBJ = huge.o

$(ODIR)/%.o: %.c
	$(CC) -c -o $@ $< $(CFLAGS)

huge: $(OBJ)
	$(CC) ${LDFLAGS} -o $@ $^ $(CFLAGS)

clean:
	rm -fr huge
