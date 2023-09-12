src = $(wildcard *.c)
obj = $(src:.c=.o)

CFLAGS = -g -I../src/

LDFLAGS = -static -L ../src/client/linux -lbreakpad_client


crash: $(obj)
	$(CXX) -o $@ $^ $(LDFLAGS)

%.o: %.c
	$(CXX) $(CFLAGS) -o $@ -c $<


.PHONY: clean

clean:
	rm -f $(obj) crash
