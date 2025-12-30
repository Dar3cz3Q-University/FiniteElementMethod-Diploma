CXX := g++
CXXFLAGS := -std=c++23 -fopenmp -DEIGEN_USE_MKL_ALL

MKL_ROOT ?= /opt/intel/oneapi/mkl/latest

SRC_DIR := src
BUILD_DIR := build
BIN_DIR := $(BUILD_DIR)/bin

SRCS := $(wildcard $(SRC_DIR)/*.cpp)
OBJS_DEBUG := $(SRCS:$(SRC_DIR)/%.cpp=$(BUILD_DIR)/debug/%.o)
OBJS_RELEASE := $(SRCS:$(SRC_DIR)/%.cpp=$(BUILD_DIR)/release/%.o)

INCLUDES := -I$(SRC_DIR) -isystem external -isystem $(MKL_ROOT)/include
LDFLAGS := -L$(MKL_ROOT)/lib/intel64 -Wl,-rpath,$(MKL_ROOT)/lib/intel64
LIBS := -lmkl_intel_lp64 -lmkl_gnu_thread -lmkl_core -lgomp -lpthread -lm -ldl

TARGET := Solver

.PHONY: all debug release clean

all: release

debug: CXXFLAGS += -DDEBUG -g -O0
debug: $(BIN_DIR)/debug/$(TARGET)

release: CXXFLAGS += -DNDEBUG -O3 -march=native -flto
release: LDFLAGS += -flto
release: $(BIN_DIR)/release/$(TARGET)

$(BIN_DIR)/debug/$(TARGET): $(OBJS_DEBUG)
	@mkdir -p $(dir $@)
	$(CXX) $(CXXFLAGS) $(LDFLAGS) -o $@ $^ $(LIBS)

$(BIN_DIR)/release/$(TARGET): $(OBJS_RELEASE)
	@mkdir -p $(dir $@)
	$(CXX) $(CXXFLAGS) $(LDFLAGS) -o $@ $^ $(LIBS)

$(BUILD_DIR)/debug/%.o: $(SRC_DIR)/%.cpp
	@mkdir -p $(dir $@)
	$(CXX) $(CXXFLAGS) $(INCLUDES) -c -o $@ $<

$(BUILD_DIR)/release/%.o: $(SRC_DIR)/%.cpp
	@mkdir -p $(dir $@)
	$(CXX) $(CXXFLAGS) $(INCLUDES) -c -o $@ $<

clean:
	rm -rf $(BUILD_DIR)
