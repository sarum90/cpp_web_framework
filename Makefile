

.PHONY: all
all:
	cd third_party && $(MAKE)

.PHONY: clean
clean:
	cd third_party && $(MAKE) clean
