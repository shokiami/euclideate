CXX := g++
CXXFLAGS := -std=c++17 -Wall -Wextra -Wpedantic -O2 -MMD -MP
TARGET := main

SRC_DIR := src
BUILD_DIR := build

SRCS := $(wildcard src/*.cc)
OBJS := $(SRCS:$(SRC_DIR)/%.cc=$(BUILD_DIR)/%.o)
DEPS := $(OBJS:.o=.d)

$(TARGET): $(OBJS)
	$(CXX) $(CXXFLAGS) $^ -o $@

$(BUILD_DIR)/%.o: $(SRC_DIR)/%.cc
	@mkdir -p $(BUILD_DIR)
	$(CXX) $(CXXFLAGS) -c $< -o $@

.PHONY: clean

clean:
	rm -rf $(BUILD_DIR) $(TARGET)

-include $(DEPS)
