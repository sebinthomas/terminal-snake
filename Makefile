CC=g++
CFLAGS= -Wall -std=c++0x
LIBS= -lncurses
TARGET= snake

all: $(TARGET)

$(TARGET): $(TARGET).cpp
	$(CC) $(CFLAGS) $(TARGET).cpp -o $(TARGET) $(LIBS)

clean:
	$(RM) $(TARGET)
