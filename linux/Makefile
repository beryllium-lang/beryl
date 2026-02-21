# This Makefile was copied from the apple clang macOS and more modifications

# --- Compiler Selection ---
LLVM_CONFIG := llvm-config
CXX         := g++

# --- LLVM Dynamic Flags ---
LLVM_CXXFLAGS := $(shell $(LLVM_CONFIG) --cppflags)
LLVM_LDFLAGS  := $(shell $(LLVM_CONFIG) --ldflags --libs --system-libs)

# --- Paths & Flags ---
INCLUDES  := -I..

CXXFLAGS  := -std=c++20 -O2 $(INCLUDES) $(LLVM_CXXFLAGS)
LDFLAGS   := -Wl,-rpath,$(LLVM_PATH)/lib $(LLVM_LDFLAGS)

SRCS      := ../beryl.cpp \
             $(wildcard ../be1/lexer/*.cpp) \
             $(wildcard ../be1/parser/*.cpp) \
             $(wildcard ../struct-opt/*.cpp) \
             $(wildcard ../utils/*.cpp) \
             $(wildcard ../clparser/*.cpp) \
			 $(wildcard ../be1/biir-codegen/*.cpp)

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