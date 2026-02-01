# --- Project root ---
ROOT := .

# --- Compiler ---
CXX := g++

# --- LLVM flags ---
LLVM_CXXFLAGS := $(shell llvm-config --cxxflags)
LLVM_LDFLAGS  := $(shell llvm-config --ldflags --libs --system-libs)

# --- Base flags ---
BASE_CXXFLAGS := -std=c++20 -O2 -Wall -Wextra -I$(ROOT) -fexceptions

CXXFLAGS := $(BASE_CXXFLAGS) $(LLVM_CXXFLAGS)

# --- Sources (auto-discover) ---
SRCS := $(shell find $(ROOT) -type f -name "*.cpp")

# --- Objects (mirror directory structure locally) ---
OBJS := $(patsubst $(ROOT)/%.cpp,%.o,$(SRCS))

# --- Target ---
TARGET := beryl

# --- Rules ---
all: $(TARGET)

# Link
$(TARGET): $(OBJS)
	$(CXX) $(OBJS) $(LLVM_LDFLAGS) -o $@

# Compile rule
%.o: $(ROOT)/%.cpp
	@mkdir -p $(dir $@)
	$(CXX) $(CXXFLAGS) -c $< -o $@

clean:
	rm -rf $(TARGET) *.o be1 utils

debug:
	@echo "Sources:"
	@printf "  %s\n" $(SRCS)
	@echo
	@echo "Objects:"
	@printf "  %s\n" $(OBJS)

.PHONY: all clean debug
