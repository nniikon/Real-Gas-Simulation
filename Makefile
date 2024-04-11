include config.mk
 
all:
	@$(MAKE) -C ./graphics/source

clean:
	@rm -rf $(BUILD_DIR)
