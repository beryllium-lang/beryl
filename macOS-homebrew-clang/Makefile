# This Makefile was generated fully by Gemini

# --- Compiler Selection ---
LLVM_PATH   := /opt/homebrew/opt/llvm
LLVM_CONFIG := $(LLVM_PATH)/bin/llvm-config
CXX         := $(LLVM_PATH)/bin/clang++
SDK_PATH    := $(shell xcrun --show-sdk-path)

# --- LLVM Dynamic Flags ---
LLVM_CXXFLAGS := $(shell $(LLVM_CONFIG) --cppflags)
LLVM_LDFLAGS  := $(shell $(LLVM_CONFIG) --ldflags --libs --system-libs)

# --- Paths & Flags ---
INCLUDES  := -I$(SDK_PATH)/usr/include/c++/v1 \
             -I$(SDK_PATH)/usr/include \
             -I..

CXXFLAGS  := -std=c++20 -O2 -nostdinc++ $(INCLUDES) $(LLVM_CXXFLAGS) -isysroot $(SDK_PATH)
LDFLAGS   := -Wl,-rpath,$(LLVM_PATH)/lib $(LLVM_LDFLAGS) -lc++

SRCS      := ../beryl.cpp \
             $(wildcard ../be1/lexer/*.cpp) \
             $(wildcard ../be1/parser/*.cpp) \
             $(wildcard ../struct-opt/*.cpp) \
             $(wildcard ../utils/*.cpp) \
             $(wildcard ../clparser/*.cpp)

OBJS      := $(SRCS:.cpp=.o)
TARGET    := beryl

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CXX) $(OBJS) $(LDFLAGS) -o $(TARGET)

%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

clean:
	rm -f $(TARGET) $(OBJS)

.PHONY: all clean

debug:
	@echo "LLVM Flags: $(LLVM_CXXFLAGS)"
	@echo "Source files: $(SRCS)"