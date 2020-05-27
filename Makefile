all:
	gcc btree.c -o btree -I.

ansi:
	gcc btree.c -o btree -I. -ansi

run:
	./btree
