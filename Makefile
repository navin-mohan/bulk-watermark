CC = g++
CFLAGS = -O2 
CFLAGS += $(shell Magick++-config --cxxflags --cppflags)
CFLAGS += $(shell Magick++-config --ldflags --libs)

watermark: watermark.cpp
	$(CC) $^ -o $@ $(CFLAGS)

clean: 
	rm watermark