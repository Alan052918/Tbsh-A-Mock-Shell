CS302_Mock_Shell: main.o
	g++ -o CS302_Mock_Shell main.o

main.o: main.c

.PHONY: clean
clean:
	rm CS302_Mock_Shell *.o
