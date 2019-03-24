PROJECT = socker
SRC_DIR = src
BLD_DIR = build
SRCS := $(wildcard $(SRC_DIR)/*.c)
OBJS := $(SRCS:.c=.o)

CFLAGS := -DAPP_NAME=\"$(PROJECT)\"

$(BLD_DIR)/$(PROJECT): $(OBJS)
	mkdir -p $(@D)
	$(CC) -o $@ $(CFLAGS) $^
	rm $(SRC_DIR)/*.o

.PHONY: clean
clean:
	rm -rf $(BLD_DIR)
