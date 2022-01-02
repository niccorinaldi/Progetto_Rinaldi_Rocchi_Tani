all: InputManager P1 P2 P3 FailureManager DecisionFunction Watchdog

Watchdog: ./src/Watchdog.c 
		cc ./src/Watchdog.c -o bin/Watchdog 

DecisionFunction: ./src/DecisionFunction.c 
			cc ./src/DecisionFunction.c -o bin/DecisionFunction 

FailureManager: ./src/FailureManager.c 
		cc ./src/FailureManager.c -o bin/FailureManager 

P3: ./src/P3.c 
	cc ./src/P3.c -o bin/P3 

P2: ./src/P2.c 
	cc ./src/P2.c -o bin/P2 

P1: ./src/P1.c 
	cc ./src/P1.c -o bin/P1 

InputManager: ./src/InputManager.c 
		cc ./src/InputManager.c -o bin/InputManager 

install:
	mkdir ./log
	mkdir ./bin
#	find . -maxdepth 1 -type f ! -name "*.*" -exec mv -t ./bin/ {} \+
#	mv ./bin/makefile ./ 

clean:
	rm -r log
	rm -r bin

