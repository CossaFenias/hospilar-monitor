CC = gcc
CFLAGS = -Wall -Wextra -pthread -g
LDFLAGS = -pthread -lncurses

SRCS = main.c synchronization.c events.c logger.c sensors.c monitor.c alarms.c gui.c
OBJS = $(SRCS:.c=.o)
TARGET = hospital_monitor

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS)

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(OBJS) $(TARGET) hospital.log

run: $(TARGET)
	./$(TARGET)