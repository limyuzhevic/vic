# Simple Makefile for testing NLM optimizations

CC = g++
CFLAGS = -std=c++20 -I. -DNLM_PERFORMANCE_ENABLED -DNLM_SIMD_AVX2 -O3 -march=native -flto
TARGET = test_optimizations
SOURCES = test_optimizations.cpp

all: $(TARGET)

$(TARGET): $(SOURCES)
	$(CC) $(CFLAGS) -o $@ $(SOURCES) -lm

clean:
	rm -f $(TARGET) *.o

.PHONY: all clean
