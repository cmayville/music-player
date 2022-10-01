
CC=gcc
LIBS=-lsqlite3
INSTALLPATH=/home/charli/.local/bin


hist-reader: main.c
	$(CC) -o hist-reader main.c $(LIBS)

install: main.c
	$(CC) -o hist-reader main.c $(LIBS)
	mv hist-reader $(INSTALLPATH)

clean:
	rm $(INSTALLPATH)/hist-reader

