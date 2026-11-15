CXX := g++
PYTHON := $(abspath ../.venv)/bin/python

THREADS ?= 4
POINTS ?= 1048576
GENERATOR ?= mcg59
SEED ?= 42
S ?= 100
K ?= 100
RATE ?= 0.05
SIGMA ?= 0.2
TIME ?= 1
PARAMS = $(S) $(K) $(RATE) $(SIGMA) $(TIME)
SIZES ?= 1024 16384 262144 1048576
THREAD_LIST ?= 1 2 4
REPEATS ?= 3

.DEFAULT_GOAL := run
.PHONY: build data run seq bench plot visualize clean

build:
	mkdir -p output
	$(CXX) -std=c++17 -O3 -Wall -Wextra -fopenmp main.cpp -o output/program

data:
	mkdir -p output
	"$(PYTHON)" tools.py generate $(GENERATOR) $(POINTS) $(SEED)

run: build data
	./output/program omp $(THREADS) output/numbers.txt $(GENERATOR) $(PARAMS)

seq: build data
	./output/program seq 1 output/numbers.txt $(GENERATOR) $(PARAMS)

bench: build
	@echo "seed,mode,generator,n,threads,price,exact,error,seconds" > output/results.csv
	@set -e; \
	for generator in mcg59 sobol; do \
		for points in $(SIZES); do \
			for repeat in $$(seq 1 $(REPEATS)); do \
				seed=$$(($(SEED) + repeat - 1)); \
				"$(PYTHON)" tools.py generate $$generator $$points $$seed; \
				printf '%s,' "$$seed" >> output/results.csv; \
				./output/program seq 1 output/numbers.txt $$generator $(PARAMS) >> output/results.csv; \
				for threads in $(THREAD_LIST); do \
					printf '%s,' "$$seed" >> output/results.csv; \
					./output/program omp $$threads output/numbers.txt $$generator $(PARAMS) >> output/results.csv; \
				done; \
			done; \
		done; \
	done
	@echo "Saved output/results.csv"

plot:
	"$(PYTHON)" tools.py plot

visualize: bench
	"$(PYTHON)" tools.py plot

clean:
	rm -rf output
