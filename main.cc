/*******************************************************
                          main.cc
********************************************************/

#include <cstdlib>
#include <assert.h>
#include <string>
#include <iostream>
#include <fstream>
#include <execinfo.h>
#include <signal.h>
#include <sstream>
#include <thread>
#include <err.h>
#include <cstring>

using namespace std;

#include "cache.h"
#include "CacheController.h"

int main(int argc, char *argv[])
{
	//fstream fp, fo;
	FILE *FP1;
   FILE *FP2;
   FILE *FP3;
   FILE *FP4;
   FILE *FP5;

   std::this_thread::sleep_for(std::chrono::milliseconds(100));

	if(argv[1] == NULL){
		 printf("input format: ");
		 printf("./smp_cache <cache_size> <assoc> <block_size> <num_processors> <protocol> <trace_file> \n");
		 exit(EXIT_FAILURE);
        }

	int cache_size = atoi(argv[1]);
	int cache_assoc= atoi(argv[2]);
	int blk_size   = atoi(argv[3]);
	int num_processors = atoi(argv[4]);/*1, 2, 4, 8*/
	int protocol   = atoi(argv[5]);	 /*0:MSI, 1:MESI, 2:Dragon*/
	char *fname =  (char *)malloc(60);
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
   /*fp.open(fname, std::ios::in);
   fo.open("tracedump", std::ios::out);*/
   std::this_thread::sleep_for(std::chrono::milliseconds(100));
   FP1 = fopen (fname, "r");
   FP2 = fopen (fname, "r");
   FP3 = fopen (fname, "r");
   FP4 = fopen (fname, "r");
   FP5 = fopen (fname, "r");
	if(FP1 == 0 || FP2 == 0 || FP3 == 0|| FP4 == 0|| FP5 == 0)
	{   
		printf("Trace file problem\n");
		exit(0);
	}

   //char /*stra[2],*/ //strb[2];
   //char * prc = new char;
   char rw;
   unsigned long address;
   uint_fast32_t addr;
   uint_fast8_t pr;
   unsigned int pr2;

   char buf1[30];
   char buf2[30];
   char buf3[30];
   char buf4[30];
   char buf5[30];
   //const char *buf6;

   bzero(buf1, 30);
   bzero(buf2, 30);
   bzero(buf3, 30);
   bzero(buf4, 30);
   bzero(buf5, 30);

std::string line;
   uint_fast32_t count = 0;
   while (fgets(buf1, 29, FP1)) {


      fgets(buf2, 29, FP2);
      fgets(buf3, 29, FP3);
      fgets(buf4, 29, FP4);
      fgets(buf5, 29, FP5);
     // buf6=line.c_str();

      if(strcmp((const char *) buf1, (const char *) buf2) == 0 && strcmp((const char *) buf1, (const char *) buf3) == 0
                                                                  && strcmp((const char *) buf1, (const char *) buf4) == 0
                                                                     && strcmp((const char *) buf1, (const char *) buf5) == 0) {
         sscanf((const char *) buf1, "%u %c %lx", &pr2, &rw, &address);
      } else if (strcmp((const char *) buf2, (const char *) buf3) == 0 && strcmp((const char *) buf3, (const char *) buf4) == 0){
         //std::cerr << "File CONSENSUS  FAILURE. Continuing.\n";
         sscanf((const char *) buf2, "%u %c %lx", &pr2, &rw, &address);
      } else if (strcmp((const char *) buf1, (const char *) buf3) == 0&& strcmp((const char *) buf1, (const char *) buf5) == 0){
         //std::cerr << "File CONSENSUS  FAILURE. Continuing.\n";
         sscanf((const char *) buf3, "%u %c %lx", &pr2, &rw, &address);
      } else if (strcmp((const char *) buf1, (const char *) buf2) == 0&& strcmp((const char *) buf1, (const char *) buf5) == 0){
         //std::cerr << "File CONSENSUS  FAILURE. Continuing.\n";
         sscanf((const char *) buf5, "%u %c %lx", &pr2, &rw, &address);
      } else if (strcmp((const char *) buf3, (const char *) buf4) == 0&& strcmp((const char *) buf4, (const char *) buf5) == 0){
         //std::cerr << "File CONSENSUS  FAILURE. Continuing.\n";
         sscanf((const char *) buf5, "%u %c %lx", &pr2, &rw, &address);
      } else {
         std::cerr << "File CONSENSUS READ FAILURE. ABORT.\n";
         std::cerr << "Received: " << std::string(buf1)<<std::endl;
         std::cerr << "Received: " << std::string(buf2)<<std::endl;
         std::cerr << "Received: " << std::string(buf3)<<std::endl;

         fclose(FP1);
         fclose(FP2);
         fclose(FP3);
         exit(EXIT_FAILURE);
      }

    //  sscanf((const char *) buf6, "%u %c %lx", &pr2, &rw, &address);

      if(pr2 >= 0 && pr2 < (unsigned int) num_processors && (rw == 'r'||rw == 'w')) {
         pr = pr2;
         addr = (const unsigned long) address;
         if (rw == 'r') {
            c->access(addr, pr, false);}
         else if (rw == 'w'){
            c->access(addr, pr, true);}
         //fo << std::to_string(pr)<<  " " << rw << " " << line.substr(4,8)<<std::endl;
      } else
      {
         std::cerr << "File VALUE READ FAILURE. ABORT.\n";
      }
      ++count;
      if(count>500001) {
         std::cerr << "ERROR OVERLENGTH:" << pr << " " << rw << " " << std::to_string(addr) << "\n";
         if(count>500010)
            exit(EXIT_FAILURE);
      }
      bzero(buf1, 30);
      bzero(buf2, 30);
      bzero(buf3, 30);
      bzero(buf4, 30);
      bzero(buf5, 30);

   }


/*
	fclose(FP1);
   fclose(FP2);
   fclose(FP3);
   fclose(FP4);
   fclose(FP5);*/

  // fp.close();
	c->report();
  // std::cout << count<<std::endl;
   //std::cout << count<<std::endl;
   if(count>500001)
      std::cerr<< "ERROR OVERLENGTH";
   return EXIT_SUCCESS;
}
