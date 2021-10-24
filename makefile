all: a.out 

flags= -std=c++2a -g -fsanitize=address -fsanitize=undefined -Wall -Wextra

a.out: main.cpp makefile
	clang++ $(flags) main.cpp

run: a.out 
	./a.out

debug: a.out 
	lldb a.out

img:
	dot -Tpng graph.dot > graph.png