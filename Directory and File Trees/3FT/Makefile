CC=gcc217

all: ft

clean:
	rm -f ft meminfo*.out

clobber: clean
	rm -f ft_client.o ft.o nodeFT.o checkerFT.o path.o dynarray.o *~

ft: ft.o ft_client.o nodeFT.o checkerFT.o path.o dynarray.o
	$(CC) -g ft_client.o ft.o nodeFT.o checkerFT.o path.o dynarray.o -o ft

ft.o: ft.c dynarray.h checkerFT.h nodeFT.h ft.h path.h a4def.h
	$(CC) -g -c ft.c

ft_client.o: ft_client.c ft.h a4def.h
	$(CC) -g -c ft_client.c

nodeFT.o: nodeFT.c dynarray.h checkerFT.h nodeFT.h path.h a4def.h
	$(CC) -g -c nodeFT.c

checkerFT.o: checkerFT.c nodeFT.h dynarray.h path.h
	$(CC) -g -c checkerFT.c

path.o: path.c dynarray.h path.h a4def.h
	$(CC) -g -c path.c

dynarray.o: dynarray.c dynarray.h
	$(CC) -g -c dynarray.c
