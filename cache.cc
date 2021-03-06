/*******************************************************
                          cache.cc

This file is originally created by NC State University/Solihin.
 Minimal modifications are made locally; Most implementation features
 are in the Cache Controller Class.
********************************************************/

#include <stdlib.h>
#include <assert.h>
#include <sstream>
#include <iomanip>

#include "cache.h"

using namespace std;

Cache::Cache(uint_fast32_t cache_size, uint_fast32_t cache_assoc, uint_fast32_t blocksize, uint_fast8_t proc,
             uint_fast8_t coh_type) {
   this->proc = proc;
   this->type = coh_type;
   ulong i, j;
   reads = read_misses = writes = invalidations = interventions = flushes = busrdx = memtraffic = cc_transfers = 0;
   write_misses = write_backs = currentCycle = 0;

   size = (ulong) (cache_size);
   lineSize = (ulong) (blocksize);
   assoc = (ulong) (cache_assoc);
   sets = (ulong) ((cache_size / blocksize) / assoc);
   numLines = (ulong) (cache_size / blocksize);
   log2Sets = (ulong) (log2(sets));
   log2Blk = (ulong) (log2(blocksize));

   //*******************//
   //initialize your counters here//
   //*******************//

   tagMask = 0;
   for (i = 0; i < log2Sets; i++) {
      tagMask <<= 1;
      tagMask |= 1;
   }

   /**create assoc two dimentional cache, sized as cache[sets][assoc]**/
   cache = new cacheLine *[sets];
   for (i = 0; i < sets; i++) {
      cache[i] = new cacheLine[assoc];
      for (j = 0; j < assoc; j++) {
         cache[i][j].invalidate();
         cache[i][j].set_proc(proc);
      }
   }

}

/**you might add other parameters to Access()
since this function is an entry point 
to the memory hierarchy (i.e. caches)**/
void Cache::Access(ulong addr, uchar op) {


   if (op == 'w') writes++;
   else reads++;

   cacheLine *line = findLine(addr);
   if (line == NULL)/*miss*/
   {
      if (op == 'w') write_misses++;
      else read_misses++;

      ++memtraffic;
      cacheLine *newline = fillLine(addr);
      if (op == 'w') newline->setFlags(DIRTY);

   } else {
      /**since it's a hit, update LRU and update dirty flag**/
      updateLRU(line);
      if (op == 'w') line->setFlags(DIRTY);
   }
}

/*look up line*/
cacheLine *Cache::findLine(ulong addr) {
   ulong i, j, tag, pos;

   pos = assoc;
   tag = calcTag(addr);
   i = calcIndex(addr);

   for (j = 0; j < assoc; j++)
      if (cache[i][j].isValid())
         if (cache[i][j].getTag() == tag) {
            pos = j;
            break;
         }
   if (pos == assoc)
      return NULL;
   else
      return &(cache[i][pos]);
}

/*upgrade LRU line to be MRU line*/
void Cache::updateLRU(cacheLine *line) {
   line->setSeq(currentCycle);
}

/*return an invalid line as LRU, if any, otherwise return LRU line*/
cacheLine *Cache::getLRU(ulong addr) {
   ulong i, j, victim, min;

   victim = assoc;
   min = currentCycle;
   i = calcIndex(addr);

   for (j = 0; j < assoc; j++) {
      if (cache[i][j].isValid() == 0) return &(cache[i][j]);
   }
   for (j = 0; j < assoc; j++) {
      cacheLine temp = cache[i][j];
      temp.getSeq();
      if (cache[i][j].getSeq() <= min) {
         victim = j;
         min = cache[i][j].getSeq();
      }
   }
   assert(victim != assoc);

   return &(cache[i][victim]);
}

/*find a victim, move it to MRU position*/
cacheLine *Cache::findLineToReplace(ulong addr) {
   cacheLine *victim = getLRU(addr);
   //updateLRU(victim);       ////////////////////////////////////////// This is the only functional change;

   return (victim);
}

/*allocate a new line*/
cacheLine *Cache::fillLine(ulong addr) {
   ulong tag;

   cacheLine *victim = findLineToReplace(addr);
   updateLRU(victim);
   assert(victim != 0);

   if (type != DRAGON && victim->getFlags() == DIRTY) {
      writeBack(addr);
   } else if (type == DRAGON && victim->getFlags() == DIRTY && victim->get_state() != SC) {
      writeBack(addr);

   }

   tag = calcTag(addr);
   victim->setTag(tag);
   victim->setFlags(VALID);

   return victim;
}

void Cache::printStats() {

   std::string output = "============ Simulation results (Cache ";
   output += std::to_string(this->proc);
   output += ") ============\n";
   output += "01. number of reads:                           ";
   cat_padded(&output, this->reads);
   output += "02. number of read misses:                     ";
   cat_padded(&output, this->read_misses);
   output += "03. number of writes:                          ";
   cat_padded(&output, this->writes);
   output += "04. number of write misses:                    ";
   cat_padded(&output, this->write_misses);
   output += "05. total miss rate:                           ";
   cat_padded(&output, (double) std::round(
           10000 * ((double) (this->read_misses + this->write_misses)) / ((double) (this->reads + this->writes))) /
                       100);
   output += "06. number of writebacks:                       ";
   cat_padded(&output, this->write_backs);
   output += "07. number of cache-to-cache transfers:         ";
   cat_padded(&output, this->cc_transfers);
   output += "08. number of memory transactions:              ";
   uint_fast32_t mem2;
   if (this->type == MSI) { mem2 = read_misses + write_misses + (busrdx - write_misses) + write_backs; }
   else if (this->type == MESI) {
      mem2 = read_misses + write_misses + (busrdx - write_misses) + write_backs - cc_transfers;
   }
   else { mem2 = read_misses + write_misses + busrdx + write_backs; }
   cat_padded(&output, mem2);
   output += "09. number of interventions:                    ";
   cat_padded(&output, this->interventions);
   output += "10. number of invalidations:                    ";
   cat_padded(&output, this->invalidations);
   output += "11. number of flushes:                          ";
   cat_padded(&output, this->flushes);
   output += "12. number of BusRdX:                           ";
   cat_padded(&output, this->busrdx);
   std::cout << output;

}

void Cache::csv_dump(double *stats, std::string *s) {

   *s += "  ";
   *s += std::to_string(this->proc);
   *s += "  ,";
   *s += std::to_string(this->reads);
   stats[0] += this->reads;
   *s += ",";
   *s += std::to_string(this->read_misses);
   stats[1] += this->read_misses;
   *s += ",";
   *s += std::to_string(this->writes);
   stats[2] += this->writes;
   *s += ",";
   *s += std::to_string(this->write_misses);
   stats[3] += this->write_misses;
   *s += ",";
   *s += std::to_string((double) std::round(
           10000 * ((double) (this->read_misses + this->write_misses)) / ((double) (this->reads + this->writes))) /
                        100);
   stats[4] += (double) std::round(
           10000 * ((double) (this->read_misses + this->write_misses)) / ((double) (this->reads + this->writes))) / 100;
   *s += ",";
   *s += std::to_string(this->write_backs);
   stats[5] += this->write_backs;
   *s += ",";
   *s += std::to_string(this->cc_transfers);
   stats[6] += this->cc_transfers;
   uint_fast32_t mem2;
   if (this->type == MSI) { mem2 = read_misses + write_misses + (busrdx - write_misses) + write_backs; }
   else if (this->type == MESI) {
      mem2 = read_misses + write_misses + (busrdx - write_misses) + write_backs - cc_transfers;
   }
   else { mem2 = read_misses + write_misses + busrdx + write_backs; }
   *s += ",";
   *s += std::to_string(mem2);
   stats[7] += mem2;
   *s += ",";
   *s += std::to_string(this->interventions);
   stats[8] += this->interventions;
   *s += ",";
   *s += std::to_string(this->invalidations);
   stats[9] += this->invalidations;
   *s += ",";
   *s += std::to_string(this->flushes);
   stats[10] += this->flushes;
   *s += ",";
   *s += std::to_string(this->busrdx);
   stats[11] += this->busrdx;
   *s += ",";

}


// Attach a numeric value to the end of a string with space padding
void Cache::cat_padded(std::string *str, uint_fast32_t n) {
   std::string value = std::to_string(n);
   while (value.length() < 12)
      value = " " + value;
   value += "\n";
   *str += value;
}

// Attach a numeric value to the end of a string with space padding
void Cache::cat_padded(std::string *str, double n) {
   std::stringstream stream;
   stream << std::fixed << std::setprecision(2) << n;
   std::string r = stream.str();

   std::string value = r;
   while (value.length() < 12)
      value = " " + value;
   value += "%\n";
   *str += value;
}

void Cache::invalidate(uint_fast32_t addr) {
   ++this->invalidations;
   cacheLine *l = findLine(addr);
   if (l->getFlags() == DIRTY)
      writeBack(addr);
   l->setFlags(INVALID);
   l->set_state(INVALID);
}

void Cache::intervention() {
   ++this->interventions;
   ++this->memtraffic;
   ++this->write_backs;
}

void Cache::intervention_mesi() {
   ++this->interventions;

}