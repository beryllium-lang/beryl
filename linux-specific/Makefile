# --- Project root ---
ROOT := ..

# --- Compiler ---
CXX := g++

# --- Base flags (yours override LLVM) ---
BASE_CXXFLAGS := -std=c++20 -O2 -Wall -Wextra -I$(ROOT) -fexceptions

# --- LLVM flags ---
LLVM_CXXFLAGS := $(shell llvm-config --cxxflags)
LLVM_LDFLAGS  := $(shell llvm-config --ldflags --libs --system-libs)

# --- Sources ---
SRCS := $(ROOT)/beryl.cpp $(wildcard $(ROOT)/utils/*.cpp)

# --- Objects (mirror directory structure locally) ---
OBJS := beryl.o $(patsubst $(ROOT)/utils/%.cpp,utils/%.o,$(wildcard $(ROOT)/utils/*.cpp))

TARGET := beryl

# --- Rules ---
all: $(TARGET)

# Link
$(TARGET): $(OBJS)
	$(CXX) $(OBJS) $(LLVM_LDFLAGS) -o $@

# Root source
beryl.o: $(ROOT)/beryl.cpp
	$(CXX) $(LLVM_CXXFLAGS) $(BASE_CXXFLAGS) -c $< -o $@

# utils/*.cpp → utils/*.o
utils/%.o: $(ROOT)/utils/%.cpp
	@mkdir -p utils
	$(CXX) $(LLVM_CXXFLAGS) $(BASE_CXXFLAGS) -c $< -o $@

clean:
	rm -rf $(TARGET) beryl.o utils

.PHONY: all clean
