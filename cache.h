/*******************************************************
                          cache.h
********************************************************/

#ifndef CACHE_H
#define CACHE_H

#include <cmath>
#include <iostream>

typedef unsigned long ulong;
typedef unsigned char uchar;
typedef unsigned int uint;

enum states{
	INVALID = 0,
	VALID,
	DIRTY,
   I=0,
   M=1,
   E=2,
   S=3
};

enum coherence_types {MSI=0, MESI=1, DRAGON=2};

class cacheLine 
{
protected:
   ulong tag;
   ulong Flags;   // 0:invalid, 1:valid, 2:dirty
   uint_fast8_t state, proc;
   ulong seq;

 
public:
   cacheLine()            { tag = 0; Flags = INVALID; state=INVALID; }
   ulong getTag()         { return tag; }
   ulong getFlags()			{ return Flags;}
   ulong getSeq()         { return seq; }
   uint_fast8_t get_state()   {return state;}
   uint_fast8_t get_proc() {return proc;}

   void setSeq(ulong Seq)			{ seq = Seq;}
   void setFlags(ulong flags)			{  Flags = flags;}
   void setTag(ulong a)   { tag = a; }
   void set_proc(uint_fast8_t in_proc) {this->proc = in_proc;}
   void set_state(uint_fast8_t in_state) {this->state = in_state;}
   void invalidate()      { tag = 0; Flags = INVALID; state=INVALID; }//useful function
   bool isValid()         { return ((Flags) != INVALID); }
};

class Cache
{
protected:
   ulong size, lineSize, assoc, sets, log2Sets, log2Blk, tagMask, numLines;
   ulong reads,read_misses,writes,write_misses,write_backs;

   uint_fast8_t proc, type;

   uint_fast32_t invalidations, interventions, cc_transfers, flushes, busrdx, memtraffic;

   cacheLine **cache;
   ulong calcTag(ulong addr)     { return (addr >> (log2Blk) );}
   ulong calcIndex(ulong addr)  { return ((addr >> log2Blk) & tagMask);}
   ulong calcAddr4Tag(ulong tag)   { return (tag << (log2Blk));}

   void cat_padded(std::string *str, uint_fast32_t n);
   void cat_padded(std::string *str, double n);
public:
    ulong currentCycle;  
     
    Cache(uint_fast32_t cache_size, uint_fast32_t cache_assoc, uint_fast32_t blocksize, uint_fast8_t proc, uint_fast8_t coh_type );
   ~Cache() {}
   
   cacheLine *findLineToReplace(ulong addr);
   cacheLine *fillLine(ulong addr);
   cacheLine * findLine(ulong addr);
   cacheLine * getLRU(ulong);
   
   ulong getRM(){return read_misses;} ulong getWM(){return write_misses;}
   ulong getReads(){return reads;}ulong getWrites(){return writes;}
   ulong getWB(){return write_backs;}
   
   void writeBack(ulong)   {write_backs++;++memtraffic;}
   void Access(ulong,uchar);
   void invalidate(uint_fast32_t addr);
   void intervention();
   void intervention_mesi();

   void printStats();
   void updateLRU(cacheLine *);

   void flush() {++flushes;}
   void bus_rdx() {++busrdx;}
   void transfer() {++cc_transfers;}

};

#endif
