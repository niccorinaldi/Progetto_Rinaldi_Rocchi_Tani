all: InputManager P1 P2 P3 FailureManager DecisionFunction Watchdog

Watchdog: ./src/Watchdog.c 
		cc ./src/Watchdog.c -o Watchdog 

DecisionFunction: ./src/DecisionFunction.c 
			cc ./src/DecisionFunction.c -o DecisionFunction 

FailureManager: ./src/FailureManager.c 
		cc ./src/FailureManager.c -o FailureManager 

P3: ./src/P3.c 
	cc ./src/P3.c -o P3 

P2: ./src/P2.c 
	cc ./src/P2.c -o P2 

P1: ./src/P1.c 
	cc ./src/P1.c -o P1 

InputManager: ./src/InputManager.c 
		cc ./src/InputManager.c -o InputManager 

install:
	mkdir ./log
	mkdir ./bin
	find . -maxdepth 1 -type f ! -name "*.*" -exec mv -t ./bin/ {} \+
	mv ./bin/Makefile ./ 

clean:
	@rm -r log # Inseriamo le @ per non far visualizzare l'errore in caso le cartelle bin e log non esistessero, come nel caso della prima esecuzione
	@rm -r bin
