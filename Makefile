CC = clang
CFLAGS = -I.
CLIENT = dist/client
SERVER = dist/server
SERVER_DIRS = simple select poll epoll libevent

.PHONY: client $(SERVER_DIRS)

client: $(CLIENT)

$(CLIENT): client.c sockutils.c
	@mkdir -p $(@D)
	$(CC) -o $@ $^ $(CFLAGS)

define SERVER_RULE
$(1):
	@mkdir -p $(dir $(SERVER))
	$(CC) -o $(SERVER) $(1)/server.c sockutils.c $(CFLAGS) $(if $(filter libevent,$(1)),-levent)
endef

$(foreach sd,$(SERVER_DIRS),$(eval $(call SERVER_RULE,$(sd))))