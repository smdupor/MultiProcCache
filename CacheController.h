//
// Created by smdupor on 10/23/21.
//

#ifndef MULTIPROCCACHE_CACHECONTROLLER_H
#define MULTIPROCCACHE_CACHECONTROLLER_H

#include <iostream>
#include <vector>
#include <algorithm>

#include "cache.h"



class CacheController {
private:
   std::vector<Cache> caches;
   uint_fast8_t type;
   void msi_access(uint_fast32_t addr, uint_fast8_t proc, bool write, std::vector<cacheLine *> local);
   void mesi_access(uint_fast32_t addr, uint_fast8_t proc, bool write, std::vector<cacheLine *> local);
   void dragon_access(uint_fast32_t addr, uint_fast8_t proc, bool write, std::vector<cacheLine *> local);
   CacheController(uint_fast32_t size, uint_fast32_t assoc, uint_fast32_t blocksize, uint_fast8_t num_caches, uint_fast8_t coherence_type);
   int count[30];

public:

   void access(uint_fast32_t addr, uint_fast8_t proc, bool write);
   void report();
   static CacheController* instance(uint_fast32_t size, uint_fast32_t assoc, uint_fast32_t blocksize, uint_fast8_t num_caches, uint_fast8_t coherence_type);
};


#endif //MULTIPROCCACHE_CACHECONTROLLER_H
