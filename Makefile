include config.mk
 
all:
	@$(MAKE) -C ./graphics/source
	@$(MAKE) -C ./engine/source
	@$(MAKE) -C ./engine/atoms_list
	@$(MAKE) -C ./libs/logs/
	@$(GXX) ./glad_fork/src/glad.c -c -o $(BUILD_DIR)/glad.o $(CFLAGS)
	@$(GXX) $(BUILD_DIR)/*.o -o $(BUILD_DIR)/$(EXEC_NAME) $(CFLAGS) -lglfw -lGL -lX11 -lpthread -lXrandr -lXi

clean:
	@rm -rf $(BUILD_DIR)

run:
	@$(BUILD_DIR)/$(EXEC_NAME)
