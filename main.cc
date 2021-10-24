/*******************************************************
                          main.cc
********************************************************/

#include <stdlib.h>
#include <assert.h>
#include <string>
#include <iostream>
#include <fstream>
using namespace std;

#include "cache.h"
#include "CacheController.h"

int main(int argc, char *argv[])
{
	
	ifstream fin;
	FILE * FP;

	if(argv[1] == NULL){
		 printf("input format: ");
		 printf("./smp_cache <cache_size> <assoc> <block_size> <num_processors> <protocol> <trace_file> \n");
		 exit(EXIT_FAILURE);
        }

	/*****uncomment the next five lines*****/
	int cache_size = atoi(argv[1]);
	int cache_assoc= atoi(argv[2]);
	int blk_size   = atoi(argv[3]);
	int num_processors = atoi(argv[4]);/*1, 2, 4, 8*/
	int protocol   = atoi(argv[5]);	 /*0:MSI, 1:MESI, 2:Dragon*/
	char *fname =  (char *)malloc(100);
 	fname = argv[6];

	std::string info = "===== 506 Personal information =====\n"
                      "FirstName (MiddleNames) LastName (Change it to your own name)\n"
                      "UnityID (Change it to your own UID)\n"
                      "ECE492 Students? YES/NO (Change it according to your own section)\n"
                      "===== 506 SMP Simulator configuration =====\n";
   info += "L1_SIZE: " + std::to_string(cache_size) + "\n";
   info += "L1_ASSOC: " + std::to_string(cache_assoc) + "\n";
   info += "L1_BLOCKSIZE: " + std::to_string(blk_size) + "\n";
   info += "NUMBER OF PROCESSORS: " + std::to_string(num_processors) + "\n";
   info += "COHERENCE PROTOCOL: ";
   if(protocol==MSI)
      info += "MSI\n";
   else if(protocol==MESI)
      info += "MESI\n";
   else
      info += "DRAGON\n";


   info += "TRACE FILE: " + std::string(fname) + "\n";

   std::cout << info;

   CacheController *c;

   switch (protocol) {
      case MSI:
         c = CacheController::instance(cache_size, cache_assoc, blk_size, num_processors, MSI);
         break;
     case MESI:
         c = CacheController::instance(cache_size, cache_assoc, blk_size, num_processors, MESI);
         break;
     case DRAGON:
         c = CacheController::instance(cache_size, cache_assoc, blk_size, num_processors, DRAGON);
         break;
   }

   FP = fopen (fname, "r");
	if(FP == 0)
	{   
		printf("Trace file problem\n");
		exit(0);
	}

   char stra[2], strb[2];
   char * prc = new char;

   uint_fast32_t addr;
   uint_fast8_t rw, pr;

   while (fscanf(FP, "%s %s %lx", stra, strb, &addr) != EOF) {
      rw = strb[0];
      *prc = stra[0];
      pr = (uint_fast8_t ) strtoul((const char *) prc, NULL, 0);
      if (rw == 'r')
         c->access(addr, pr, true);
      else if (rw == 'w')
         c->access(addr, pr, false);
   }

	fclose(FP);

	c->report();

   return EXIT_SUCCESS;
}
