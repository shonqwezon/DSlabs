CXX := g++
PYTHON := $(abspath ../.venv)/bin/python

THREADS ?= 4
POINTS ?= 10000
KIND ?= square
SIZES ?= 10000 100000 1000000
KINDS ?= square circle
THREAD_LIST ?= 1 2 4
REPEATS ?= 3

.DEFAULT_GOAL := run
.PHONY: build data run visualize bench clean plot

build:
	mkdir -p output
	$(CXX) -std=c++17 -O3 -Wall -Wextra -fopenmp main.cpp -o output/program

data:
	mkdir -p output
	"$(PYTHON)" tools.py generate $(POINTS) $(KIND) output/points.txt

run: build data
	./output/program omp $(THREADS) < output/points.txt > output/hull.txt
	"$(PYTHON)" tools.py hull

visualize: bench
	"$(PYTHON)" tools.py plot

plot:
	"$(PYTHON)" tools.py plot

bench: build
	@echo "kind,repeat,mode,n,threads,seconds" > output/results.csv
	@set -e; 	for kind in $(KINDS); do 		for points in $(SIZES); do 			"$(PYTHON)" tools.py generate $$points $$kind output/bench.txt; 			for repeat in $$(seq 1 $(REPEATS)); do 				printf '%s,%s,' "$$kind" "$$repeat" >> output/results.csv; 				./output/program seq 1 < output/bench.txt > /dev/null 2>> output/results.csv; 				for threads in $(THREAD_LIST); do 					printf '%s,%s,' "$$kind" "$$repeat" >> output/results.csv; 					./output/program omp $$threads < output/bench.txt > /dev/null 2>> output/results.csv; 				done; 			done; 		done; 	done
	@echo "Saved output/results.csv"

clean:
	rm -rf output
