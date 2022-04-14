CC = g++
LIBS  = -lcppcanfd -leci116DriverLinux-usb-1.0 -lpthread
TARGET = spamreader

all: $(TARGET)

$(TARGET): $(TARGET).cpp
	$(CC)  $(TARGET).cpp -o $(TARGET).out $(LIBS)

clean:
	$(RM) $(TARGET)