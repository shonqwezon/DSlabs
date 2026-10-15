THREADS ?= 4

INSERTS ?= 1000
OPS     ?= 1000000
SEARCH  ?= 0.8
INSERT  ?= 0.1

STDIN_INPUT = $(INSERTS)\n$(OPS)\n$(SEARCH)\n$(INSERT)\n

.DEFAULT_GOAL := run

build:
	gcc -O2 -Wall -pthread pth_ll_rwl.c my_rand.c -o output/pth_ll_rwl
	gcc -O2 -Wall -pthread main.c my_rwlock.c my_rand.c -o output/program

run: build
	@echo "pth_ll_rwl:"
	printf "$(STDIN_INPUT)" | ./output/pth_ll_rwl $(THREADS)
	@echo "my_rwlock:"
	printf "$(STDIN_INPUT)" | ./output/program $(THREADS)

test: build
	@printf "%-7s %-12s %-12s %s\n" "THREAD" "RWL" "MY_RWL" "RATIO"; \
	for threads in 1 2 3 4 5 6 7 8 9 10 11 12; do \
		rwl=$$(printf "$(STDIN_INPUT)" | ./output/pth_ll_rwl "$$threads" | \
			awk '/^Elapsed time = / { printf "%.4f", $$4 }'); \
		program=$$(printf "$(STDIN_INPUT)" | ./output/program "$$threads" | \
			awk '/^Elapsed time = / { printf "%.4f", $$4 }'); \
		ratio=$$(awk -v a="$$program" -v b="$$rwl" 'BEGIN { printf "%.2f", a/b }'); \
		printf "%-7s %-12s %-12s %s\n" "$$threads" "$$rwl" "$$program" "$$ratio"; \
	done
