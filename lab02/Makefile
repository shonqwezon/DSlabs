VENV ?= .venv
PYTHON ?= python3

.PHONY: bootstrap

bootstrap:
	$(PYTHON) -m venv $(VENV)
	$(VENV)/bin/pip install --upgrade pip
	$(VENV)/bin/pip install matplotlib numpy pandas scipy
	@echo
	@echo "Done. Activate with: source $(VENV)/bin/activate"
