tbsh.out: tbsh.c
	gcc -o tbsh.out tbsh.c

.PHONY: clean
clean:
	rm tbsh.out
