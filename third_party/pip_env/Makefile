
include ./common.mk


$(PIP_ENV_NATIVE_TAG):
	rm -rf $(PYENV)
	pip install virtualenv==15.0.3
	virtualenv $(PYENV)
	. $(PYSOURCE) && pip install pip==9.0.0
	mkdir -p $(NATIVE_TAGS)
	touch $(PIP_ENV_NATIVE_TAG)

.PHONY: clean
clean:
	rm -rf $(PIP_ENV_NATIVE_TAG)
	rm -rf $(PYENV)

