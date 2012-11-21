all: fat

OBJS += ff.o
OBJS += diskio.o
OBJS += main.o

CFLAGS += -g -Wall

%.o: %.c
	$(CC) -c -o $@ $(CFLAGS) $<
	
fat: $(OBJS)
	$(CC) $(CLDFLAGS) -o $@ $(OBJS) -lreadline -lhistory
	
clean:
	- rm -rf fat $(OBJS)
	