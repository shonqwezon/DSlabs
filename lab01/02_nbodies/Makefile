THREADS ?= 1
TIME ?= 20000

PYTHON := $(abspath ../.venv)/bin/python

.DEFAULT_GOAL := visualize

build:
	gcc -O2 -Wall -pthread main.c pthread_barrier.c -o output/program

run: build
	 ./output/program $(THREADS) $(TIME) input.txt

visualize: run
	"$(PYTHON)" visualize.py

only_visualize:
	"$(PYTHON)" visualize.py

test: build
	@printf "%-7s %-7s %-7s %s\n" "THREAD" "TIME" "SEC" "POINTS"; \
	for threads in 1 2 3 4 5 6 7 8 9 10 11 12; do \
		result=$$(./output/program "$$threads" 100 input.txt) || exit $$?; \
		seconds=$$(printf '%s\n' "$$result" | \
			awk '/^Elapsed time = / { printf "%.2f", $$4 }'); \
		printf "%-7s %-12s %-7s %s\n" "$$threads" "100" "$$seconds" "500"; \
	done

generate:
	"$(PYTHON)" generate.py

check:
	"$(PYTHON)" check.py
