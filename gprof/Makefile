test: test.o
	$(CC) $^ -o $@ -pg

test.o: test.c
	$(CC) -g -c $^ -o $@ -pg

clean:
	rm -f test test.o gmon.out


