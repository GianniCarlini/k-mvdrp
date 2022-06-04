all: main

main: main.cpp
		g++ -o main main.cpp
run: 
		./main instance_1.txt
clean: 
	rm -rf *o main