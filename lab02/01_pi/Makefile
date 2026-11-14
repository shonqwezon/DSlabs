CXX := g++
PYTHON := $(abspath ../.venv)/bin/python

THREADS ?= 4
POINTS ?= 10000000
SEED ?= 42

SIZES ?= 100000 1000000 10000000
THREAD_LIST ?= 1 2 4 8
REPEATS ?= 3

.DEFAULT_GOAL := run

.PHONY: build run bench plot visualize clean

build:
	mkdir -p output
	$(CXX) -std=c++17 -O3 -Wall -Wextra -fopenmp main.cpp -o output/program

run: build
	./output/program omp $(POINTS) $(THREADS) $(SEED)

bench: build
	@echo "mode,n,threads,seed,hits,pi,seconds" > output/results.csv
	@set -e; \
	for points in $(SIZES); do \
		for repeat in $$(seq 1 $(REPEATS)); do \
			seed=$$(($(SEED) + repeat - 1)); \
			./output/program seq $$points 1 $$seed >> output/results.csv; \
			for threads in $(THREAD_LIST); do \
				./output/program omp $$points $$threads $$seed >> output/results.csv; \
			done; \
		done; \
	done
	@echo "Saved output/results.csv"

plot:
	"$(PYTHON)" tools.py

visualize: bench
	"$(PYTHON)" tools.py

clean:
	rm -rf output
