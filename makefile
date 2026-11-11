CC = gcc
CFLAGS = -Wall -pthread
TARGET = campus_compute
OBJS = main.o scheduler.o logger.o ipc_worker.o bankers.o

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) -o $(TARGET) $(OBJS) -lrt

%.o: %.c
	$(CC) $(CFLAGS) -c $<

clean:
	rm -f $(TARGET) $(OBJS) log.txt
