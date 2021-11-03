/*******************************************************
                          main.cc
********************************************************/

#include <stdlib.h>
#include <assert.h>
#include <string>
#include <iostream>
#include <fstream>
#include <execinfo.h>
#include <signal.h>
#include <err.h>
#include <cstring>

using namespace std;

#include "cache.h"
#include "CacheController.h"




/*
int addr2line(char const * const program_name, void const * const addr)
{
   char addr2line_cmd[512] = {0};

  //  have addr2line map the address to the relent line in the code
#ifdef __APPLE__
//
    sprintf(addr2line_cmd,"atos -o %.256s %p", program_name, addr);
#else
   sprintf(addr2line_cmd,"addr2line -f -p -e %.256s %p", program_name, addr);
#endif

//    This will print a nicely formatted string specifying the
//      function and source line of the address
   return system(addr2line_cmd);
}

#define MAX_STACK_FRAMES 64
static void *stack_traces[MAX_STACK_FRAMES];
void posix_print_stack_trace()
{
   int i, trace_size = 0;
   char **messages = (char **)NULL;

   trace_size = backtrace(stack_traces, MAX_STACK_FRAMES);
   messages = backtrace_symbols(stack_traces, trace_size);

    skip the first couple stack frames (as they are this function and
      our handler) and also skip the last frame as it's (always?) junk.
   // for (i = 3; i < (trace_size - 1); ++i)
   // we'll use this for now so you can see what's going on
   for (i = 0; i < trace_size; ++i)
   {
      if (addr2line("smp_cache", stack_traces[i]) != 0)
      {
         printf("  error determining line # for: %s\n", messages[i]);
      }

   }
   if (messages) { free(messages); }
}

void posix_signal_handler(int sig, siginfo_t *siginfo, void *context)
{
   (void)context;
   switch(sig)
   {
      case SIGSEGV:
         fputs("Caught SIGSEGV: Segmentation Fault\n", stderr);
         break;
      case SIGINT:
         fputs("Caught SIGINT: Interactive attention signal, (usually ctrl+c)\n",
               stderr);
         break;
      case SIGFPE:
         switch(siginfo->si_code)
         {
            case FPE_INTDIV:
               fputs("Caught SIGFPE: (integer divide by zero)\n", stderr);
               break;
            case FPE_INTOVF:
               fputs("Caught SIGFPE: (integer overflow)\n", stderr);
               break;
            case FPE_FLTDIV:
               fputs("Caught SIGFPE: (floating-point divide by zero)\n", stderr);
               break;
            case FPE_FLTOVF:
               fputs("Caught SIGFPE: (floating-point overflow)\n", stderr);
               break;
            case FPE_FLTUND:
               fputs("Caught SIGFPE: (floating-point underflow)\n", stderr);
               break;
            case FPE_FLTRES:
               fputs("Caught SIGFPE: (floating-point inexact result)\n", stderr);
               break;
            case FPE_FLTINV:
               fputs("Caught SIGFPE: (floating-point invalid operation)\n", stderr);
               break;
            case FPE_FLTSUB:
               fputs("Caught SIGFPE: (subscript out of range)\n", stderr);
               break;
            default:
               fputs("Caught SIGFPE: Arithmetic Exception\n", stderr);
               break;
         }
      case SIGILL:
         switch(siginfo->si_code)
         {
            case ILL_ILLOPC:
               fputs("Caught SIGILL: (illegal opcode)\n", stderr);
               break;
            case ILL_ILLOPN:
               fputs("Caught SIGILL: (illegal operand)\n", stderr);
               break;
            case ILL_ILLADR:
               fputs("Caught SIGILL: (illegal addressing mode)\n", stderr);
               break;
            case ILL_ILLTRP:
               fputs("Caught SIGILL: (illegal trap)\n", stderr);
               break;
            case ILL_PRVOPC:
               fputs("Caught SIGILL: (privileged opcode)\n", stderr);
               break;
            case ILL_PRVREG:
               fputs("Caught SIGILL: (privileged register)\n", stderr);
               break;
            case ILL_COPROC:
               fputs("Caught SIGILL: (coprocessor error)\n", stderr);
               break;
            case ILL_BADSTK:
               fputs("Caught SIGILL: (internal stack error)\n", stderr);
               break;
            default:
               fputs("Caught SIGILL: Illegal Instruction\n", stderr);
               break;
         }
         break;
      case SIGTERM:
         fputs("Caught SIGTERM: a termination request was sent to the program\n",
               stderr);
         break;
      case SIGABRT:
         fputs("Caught SIGABRT: usually caused by an abort() or assert()\n", stderr);
         break;
      default:
         break;
   }
   posix_print_stack_trace();
   _Exit(1);
}

static uint8_t alternate_stack[SIGSTKSZ];
void set_signal_handler()
{
//    setup alternate stack
   {
      stack_t ss = {};
//       malloc is usually used here, I'm not 100% sure my static allocation
//         is valid but it seems to work just fine.
      ss.ss_sp = (void*)alternate_stack;
      ss.ss_size = SIGSTKSZ;
      ss.ss_flags = 0;

      if (sigaltstack(&ss, NULL) != 0) { err(1, "sigaltstack"); }
   }

//    register our signal handlers
   {
      struct sigaction sig_action = {};
      sig_action.sa_sigaction = posix_signal_handler;
      sigemptyset(&sig_action.sa_mask);

#ifdef __APPLE__
//       for some reason we backtrace() doesn't work on osx
//           when we use an alternate stack
        sig_action.sa_flags = SA_SIGINFO;
#else
      sig_action.sa_flags = SA_SIGINFO | SA_ONSTACK;
#endif

      if (sigaction(SIGSEGV, &sig_action, NULL) != 0) { err(1, "sigaction"); }
      if (sigaction(SIGFPE,  &sig_action, NULL) != 0) { err(1, "sigaction"); }
      if (sigaction(SIGINT,  &sig_action, NULL) != 0) { err(1, "sigaction"); }
      if (sigaction(SIGILL,  &sig_action, NULL) != 0) { err(1, "sigaction"); }
      if (sigaction(SIGTERM, &sig_action, NULL) != 0) { err(1, "sigaction"); }
      if (sigaction(SIGABRT, &sig_action, NULL) != 0) { err(1, "sigaction"); }
   }
}*/

int main(int argc, char *argv[])
{
	//set_signal_handler();

	ifstream fin;
	FILE * FP;

/*   int *i = 0;
   i = 0;

   std::string s = "asdf";
   s += std::to_string(*i);*/

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

   //char /*stra[2],*/ //strb[2];
   //char * prc = new char;
   char rw;
   unsigned long address;
   uint_fast32_t addr;
   uint_fast8_t pr;
   int pr2;
   //unsigned long pr2;

   char buffer[256];

   uint_fast32_t count = 0;
   while (fgets(buffer, 256, FP)) {
      sscanf((const char *) buffer, "%d %c %lx", &pr2, &rw, &address);

      //rw = strb[0];

     // *prc = stra[0];
      //29 526 506
     // pr = (uint_fast8_t ) strtoul((const char *) prc, NULL, 0);


      if(pr2 >= 0 && pr2 < num_processors && (rw == 'r'||rw == 'w')) {
         pr = (const uint_fast8_t) pr2;
         addr = (const unsigned long) address;
         if (rw == 'r')
            c->access(addr, pr, false);
         else if (rw == 'w')
            c->access(addr, pr, true);

      }
      ++count;
      if(count>500001) {
         std::cerr << "ERROR OVERLENGTH:" << pr << " " << rw << " " << std::to_string(addr) << "\n";
         if(count>500010)
            exit(EXIT_FAILURE);
      }
      bzero(buffer, 256);
   }


	fclose(FP);

	c->report();
  // std::cout << count<<std::endl;
   //std::cout << count<<std::endl;
   if(count>500001)
      std::cerr<< "ERROR OVERLENGTH";
   return EXIT_SUCCESS;
}
