include config.mk
 
all:
	@$(MAKE) -C ./graphics/source
	@$(MAKE) -C ./engine/source

clean:
	@rm -rf $(BUILD_DIR)
