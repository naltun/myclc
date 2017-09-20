all:
		gcc -std=c99 -Wall src/myclc.c libs/mpc.c -ledit -lm -o myclc

clean:
		rm myclc
