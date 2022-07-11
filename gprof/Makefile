test: test.o
	$(CC) $^ -o $@ -pg

test.o: test.c
	(CC) -g -c $^ -o $@ -pg
	#$(CC) -g -c $^ -o $@ -pg -mfentry

clean:
	rm -f test test.o gmon.out


