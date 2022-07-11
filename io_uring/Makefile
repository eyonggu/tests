all: cat_readv cat_iouring cat_liburing

cat_readv: cat_readv.o
	$(CC) $^ -o $@

cat_iouring: cat_iouring.o
	$(CC) $^ -o $@

cat_liburing: cat_liburing.o
	$(CC) $^ -o $@ -luring

clean:
	rm -f *.o cat_readv cat_iouring cat_liburing


