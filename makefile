PROJECT = socker
SRC_DIR = src
BLD_DIR = build
SRCS := $(wildcard $(SRC_DIR)/*.c)
OBJS := $(addprefix $(BLD_DIR)/,$(notdir $(SRCS:.c=.o)))

CFLAGS := -DAPP_NAME=\"$(PROJECT)\"

$(BLD_DIR)/$(PROJECT): $(OBJS)
	$(CC) $(CFLAGS) -o $@ $^

$(BLD_DIR)/%.o: $(SRC_DIR)/%.c
	mkdir -p $(@D)
	$(CC) $(CFLAGS) -c $< -o $@

.PHONY: clean
clean:
	rm -rf $(BLD_DIR)
