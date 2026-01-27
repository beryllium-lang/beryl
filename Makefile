# --- Compiler Selection ---
LLVM_PATH := /opt/homebrew/opt/llvm
CXX       := $(LLVM_PATH)/bin/clang++

# --- SDK Detection ---
SDK_PATH  := $(shell xcrun --show-sdk-path)

# --- Paths & Flags ---
# We manually add the Apple SDK C++ include paths. 
# This prevents Clang from using the Homebrew C++ headers.
INCLUDES  := -I$(SDK_PATH)/usr/include/c++/v1 \
             -I$(SDK_PATH)/usr/include \
             -I$(LLVM_PATH)/include \
             -I.

# -nostdinc++: Tell the compiler "Don't use your own C++ headers"
# -isysroot: Tell it where the system root is
CXXFLAGS  := -std=c++20 -O2 -nostdinc++ $(INCLUDES) -isysroot $(SDK_PATH)

# LDFLAGS: Link LLVM and rely on the system libc++
LDFLAGS   := -L$(LLVM_PATH)/lib -Wl,-rpath,$(LLVM_PATH)/lib -lLLVM -lc++

# --- Files ---
SRCS      := beryl.cpp $(wildcard utils/*.cpp)
TARGET    := beryl

# --- Rules ---

all: clean $(TARGET)

$(TARGET):
	$(CXX) $(CXXFLAGS) $(SRCS) $(LDFLAGS) -o $(TARGET)

clean:
	rm -f $(TARGET) *.o utils/*.o

.PHONY: all clean