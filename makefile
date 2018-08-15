resistance: Main.c ConspiratorLoad.c Utils.c Fifo.c Log.c
	mpicc -pthread Main.c ConspiratorLoad.c Utils.c Fifo.c Log.c -lm -o resistance
