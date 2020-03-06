tbsh: tbsh.c
	clang -o tbsh tbsh.c

.PHONY: clean
clean:
	rm tbsh
