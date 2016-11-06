
include ./common.mk

BFG9000_NATIVE_TAG=$(NATIVE_TAGS)/bfg9000

$(BFG9000_NATIVE_TAG): $(PIP_ENV_NATIVE_TAG)
	. $(PYSOURCE) && pip install -r requirements.txt
	touch $(BFG9000_NATIVE_TAG)

$(PIP_ENV_NATIVE_TAG):
	make -C ../pip_env

.PHONY: clean
clean:
	rm -rf $(BFG9000_NATIVE_TAG)
