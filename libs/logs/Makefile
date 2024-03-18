SRCS = $(wildcard *.cpp)
OBJS = $(patsubst %.cpp, $(BUILD_DIR)/%.o, $(SRCS))

all: $(BUILD_DIR) $(OBJS)

$(BUILD_DIR)/%.o: %.cpp
	@$(GXX) $^ $(CFLAGS) -c -o $@

$(BUILD_DIR):
	@mkdir -p $(BUILD_DIR)