CC = g++
OPT = -O3
OPT = -s
WARN = -Wall
ERR = -Werror

CFLAGS = $(OPT) -std=c++11 $(WARN) $(ERR) $(INC) $(LIB)

SIM_SRC = main.cc cache.cc CacheController.cpp

SIM_OBJ = main.o cache.o CacheController.o

all: smp_cache
	rm *.o
	@echo "Compilation Done ---> nothing else to make"

smp_cache: $(SIM_OBJ)
	$(CC) -o smp_cache $(CFLAGS) $(SIM_OBJ) -lm
	@echo "----------------------------------------------------------"
	@echo "-----------FALL21-506-406 SMP SIMULATOR (SMP_CACHE)-----------"
	@echo "----------------------------------------------------------"
 
.cc.o:
	$(CC) $(CFLAGS)  -c $*.cc

clean:
	rm -f *.o smp_cache

clobber:
	rm -f *.o


