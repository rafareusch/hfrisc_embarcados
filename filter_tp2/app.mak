APP_DIR = $(SRC_DIR)/$(APP)

app: kernel
	$(CC) $(CFLAGS) \
		$(APP_DIR)/filter_tp2.c 
