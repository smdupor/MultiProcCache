/*******************************************************
                          cache.cc
********************************************************/

#include <stdlib.h>
#include <assert.h>
#include "cache.h"
using namespace std;

Cache::Cache(uint_fast32_t cache_size, uint_fast32_t cache_assoc, uint_fast32_t blocksize, uint_fast8_t proc )
{
   this->proc = proc;
   ulong i, j;
   reads = read_misses = writes = invalidations = interventions = flushes= busrdx= memtraffic=cc_transfers= 0;
   write_misses = write_backs = currentCycle = 0;

   size       = (ulong)(cache_size);
   lineSize   = (ulong)(blocksize);
   assoc      = (ulong)(cache_assoc);
   sets       = (ulong)((cache_size / blocksize) / assoc);
   numLines   = (ulong)(cache_size / blocksize);
   log2Sets   = (ulong)(log2(sets));   
   log2Blk    = (ulong)(log2(blocksize));
  
   //*******************//
   //initialize your counters here//
   //*******************//
 
   tagMask =0;
   for(i=0;i<log2Sets;i++)
   {
		tagMask <<= 1;
        tagMask |= 1;
   }
   
   /**create assoc two dimentional cache, sized as cache[sets][assoc]**/
   cache = new cacheLine*[sets];
   for(i=0; i<sets; i++)
   {
      cache[i] = new cacheLine[assoc];
      for(j=0; j<assoc; j++) 
      {
	   cache[i][j].invalidate();
      cache[i][j].set_proc(proc);
      }
   }      
   
}

/**you might add other parameters to Access()
since this function is an entry point 
to the memory hierarchy (i.e. caches)**/
void Cache::Access(ulong addr,uchar op)
{
	currentCycle++;
        	
	if(op == 'w') writes++;
	else          reads++;
	
	cacheLine * line = findLine(addr);
	if(line == NULL)/*miss*/
	{
		if(op == 'w') write_misses++;
		else read_misses++;

      ++memtraffic;
		cacheLine *newline = fillLine(addr);
   		if(op == 'w') newline->setFlags(DIRTY);    
		
	}
	else
	{
		/**since it's a hit, update LRU and update dirty flag**/
		updateLRU(line);
		if(op == 'w') line->setFlags(DIRTY);
	}
}

/*look up line*/
cacheLine * Cache::findLine(ulong addr)
{
   ulong i, j, tag, pos;
   
   pos = assoc;
   tag = calcTag(addr);
   i   = calcIndex(addr);
  
   for(j=0; j<assoc; j++)
	if(cache[i][j].isValid())
	        if(cache[i][j].getTag() == tag)
		{
		     pos = j; break; 
		}
   if(pos == assoc)
	return NULL;
   else
	return &(cache[i][pos]); 
}

/*upgrade LRU line to be MRU line*/
void Cache::updateLRU(cacheLine *line)
{
  line->setSeq(currentCycle);  
}

/*return an invalid line as LRU, if any, otherwise return LRU line*/
cacheLine * Cache::getLRU(ulong addr)
{
   ulong i, j, victim, min;

   victim = assoc;
   min    = currentCycle;
   i      = calcIndex(addr);
   
   for(j=0;j<assoc;j++)
   {
      if(cache[i][j].isValid() == 0) return &(cache[i][j]);     
   }   
   for(j=0;j<assoc;j++)
   {
	 if(cache[i][j].getSeq() <= min) { victim = j; min = cache[i][j].getSeq();}
   } 
   assert(victim != assoc);
   
   return &(cache[i][victim]);
}

/*find a victim, move it to MRU position*/
cacheLine *Cache::findLineToReplace(ulong addr)
{
   cacheLine * victim = getLRU(addr);
   //updateLRU(victim);
  
   return (victim);
}

/*allocate a new line*/
cacheLine *Cache::fillLine(ulong addr)
{ 
   ulong tag;
  
   cacheLine *victim = findLineToReplace(addr);
   updateLRU(victim);
   assert(victim != 0);
   if(victim->getFlags() == DIRTY) writeBack(addr);
    	
   tag = calcTag(addr);   
   victim->setTag(tag);
   victim->setFlags(VALID);    

   return victim;
}

void Cache::printStats()
{ 
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
   cat_padded(&output, (double)std::round(10000*((double)(this->read_misses+this->write_misses))/((double)(this->reads+this->writes)))/10000); // TODO l1+vc miss rate
   output += "06. number of writebacks:                       ";
   cat_padded(&output, this->write_backs);
   output += "07. number of cache-to-cache transfers:         ";
   cat_padded(&output, this->cc_transfers);
   output += "08. number of memory transactions:              ";
   cat_padded(&output, this->memtraffic);
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

// Attach a numeric value to the end of a string with space padding
void Cache::cat_padded(std::string *str, uint_fast32_t n) {
   std::string value = std::to_string(n);
   while (value.length() < 12)
      value = " " + value;
   value += "\n";
   *str += value;
}

void Cache::invalidate(uint_fast32_t addr) {
   ++this->invalidations;
   cacheLine *l = findLine(addr);
   if(l->getFlags()==DIRTY)
      writeBack(addr);
   l->setFlags(INVALID);
   l->set_state(INVALID);
}

void Cache::intervention() {
   ++this->interventions;
}