# This Makefile was written by Gemini but i forgot when i made it
# It only works on my machine (MacOS with homebrew llvm)

# --- Compiler Selection ---
# MAC OS X specific LLVM path
LLVM_PATH := /opt/homebrew/opt/llvm
CXX       := $(LLVM_PATH)/bin/clang++
SDK_PATH  := $(shell xcrun --show-sdk-path)

# --- Paths & Flags ---
INCLUDES  := -I$(SDK_PATH)/usr/include/c++/v1 \
             -I$(SDK_PATH)/usr/include \
             -I$(LLVM_PATH)/include \
             -I.

CXXFLAGS  := -std=c++20 -O2 -nostdinc++ $(INCLUDES) -isysroot $(SDK_PATH)
LDFLAGS   := -L$(LLVM_PATH)/lib -Wl,-rpath,$(LLVM_PATH)/lib -lLLVM -lc++

# --- Files ---
SRCS      := $(shell find . -name "*.cpp")
# This transforms the list of .cpp files into .o files
OBJS      := $(SRCS:.cpp=.o)
TARGET    := beryl

# --- Rules ---

all: $(TARGET)

# Linking Step
$(TARGET): $(OBJS)
	$(CXX) $(OBJS) $(LDFLAGS) -o $(TARGET)

# Compilation Step (The % is a wildcard for the filename)
%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

clean:
	rm -f $(TARGET) $(OBJS)

.PHONY: all clean

debug:
	@echo "Source files: $(SRCS)"
	@echo "Object files: $(OBJS)"