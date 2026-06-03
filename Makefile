CC = gcc
CFLAGS = -Wall -pthread
TARGET = hospital_monitor
OBJS = main.o sensors.o monitor.o alarms.o events.o logger.o

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) -o $@ $^

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(OBJS) $(TARGET)

run: $(TARGET)
	./$(TARGET)