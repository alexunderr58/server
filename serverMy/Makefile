CXX = g++
CXXFLAGS = -std=c++11 -Wall -Wextra -O2
LDFLAGS = -lssl -lcrypto
TARGET = server
SOURCES = main.cpp server.cpp clientdb.cpp authenticator.cpp vectorprocessor.cpp logger.cpp

$(TARGET): $(SOURCES)
	$(CXX) $(CXXFLAGS) -o $(TARGET) $(SOURCES) $(LDFLAGS)

.PHONY: clean install

clean:
	rm -f $(TARGET)

install: $(TARGET)
	cp $(TARGET) /usr/local/bin/
	mkdir -p /etc /var/log
	touch /etc/vcalc.conf /var/log/vcalc.log
	chmod 644 /etc/vcalc.conf /var/log/vcalc.log
