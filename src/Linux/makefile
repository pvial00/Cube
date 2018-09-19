CC=g++
CFLAGS=-g -lbsd -lssl -lcrypto -std=c++11 -I/usr/local/opt/openssl/include -L/usr/local/opt/openssl/lib -O3
TARGET=cube
all: $(TARGET)

$(TARGET): cubeapp.cpp
	$(CC) cubeapp.cpp $(CFLAGS) -o $(TARGET)

clean:
	$(RM) $(TARGET)
