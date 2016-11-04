
.PHONY: all
all:
	$(MAKE) -C patchelf
	$(MAKE) -C pip_env/
	$(MAKE) -C third_party/bfg9000/
	$(MAKE) -C boost
	$(MAKE) -C mettle

.PHONY: clean
clean:
	$(MAKE) -C patchelf clean
	$(MAKE) -C pip_env/ clean
	$(MAKE) -C third_party/bfg9000/ clean
	$(MAKE) -C boost clean
	$(MAKE) -C mettle clean
