COMMON = agent.c

all:
	mkdir -p build
	cd c_src; gcc -g -Wall $(COMMON) main.c -o ../build/main
