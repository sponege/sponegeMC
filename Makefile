CC      := cc
LIBS    := 
OUT_DIR := build
TARGETS := sponegeMC

.PHONY: $(TARGETS) clean install

all: $(TARGETS) | $(OUT_DIR)

$(OUT_DIR):
	mkdir $@

sponegeMC: src/socket/packet.c src/logger/logger.c | $(OUT_DIR)
	$(CC) -g src/socket/handshake.c src/socket/status.c src/socket/packet.c src/socket/main.c src/logger/logger.c -o $(OUT_DIR)/$@ $(LIBS)

clean:
	rm -r $(OUT_DIR)

install: $(OUT_DIR)
	cp $(OUT_DIR)/* /bin