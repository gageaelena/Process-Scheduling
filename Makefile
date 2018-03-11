.PHONEY: build planificator clean

build: planificator

planificator: planificator.c
	gcc -Wall planificator.c -o planificator -g

clean:
	rm -rf planificator