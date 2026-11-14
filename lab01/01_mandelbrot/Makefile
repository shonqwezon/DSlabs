THREADS ?= 4
POINTS ?= 10000

PYTHON := $(abspath ../.venv)/bin/python

.DEFAULT_GOAL := visualize

build:
	gcc -O2 -Wall -pthread main.c my_rand.c -o output/program

run: build
	 ./output/program $(THREADS) $(POINTS)

visualize: run
	"$(PYTHON)" visualize.py

test: build
	@printf "%-7s %-12s %s\n" "THREAD" "POINTS" "SEC"; \
	for threads in 13 14 15 16 17 18 20; do \
		result=$$(./output/program "$$threads" 100000000) || exit $$?; \
		seconds=$$(printf '%s\n' "$$result" | \
			awk '/^Elapsed time = / { printf "%.2f", $$4 }'); \
		printf "%-7s %-12s %s\n" "$$threads" "100000000" "$$seconds"; \
	done
