CC = arm-linux-gcc
TARGET = wifi_up
$(TARGET): $(TARGET).c
	$(CC) -o $(TARGET) $(TARGET).c -static

clean:
	rm $(TARGET)
