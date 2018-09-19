CC=g++
CFLAGS=-lssl -lcrypto -std=c++11 -I/usr/local/opt/openssl/include -L/usr/local/opt/openssl/lib -O3
TARGET=cube
all: $(TARGET)

$(TARGET): cubeapp.cpp
	$(CC) $(CFLAGS) -o $(TARGET) cubeapp.cpp

clean:
	$(RM) $(TARGET)
