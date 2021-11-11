/**
 * CacheController.h encapsulates header functionality for the Cache Controller class which handles all cases of
 * cache coherency across multiple protocols.
 *
 * Created on: October 22nd, 2021
 * Author: Stevan Dupor
 * Copyright (C) 2021 Stevan Dupor - All Rights Reserved
 * Unauthorized copying of this file, via any medium is strictly prohibited.
 */

#ifndef MULTIPROCCACHE_CACHECONTROLLER_H
#define MULTIPROCCACHE_CACHECONTROLLER_H

#include <iostream>
#include <vector>
#include <algorithm>

#include "cache.h"



class CacheController {
private:
   //std::vector<Cache> caches;
   Cache * caches;

   uint_fast8_t type,qty;
   void msi_access(uint_fast32_t addr, uint_fast8_t proc, bool write, std::vector<cacheLine *> local);
   void mesi_access(uint_fast32_t addr, uint_fast8_t proc, bool write, std::vector<cacheLine *> local);
   void dragon_access(uint_fast32_t addr, uint_fast8_t proc, bool write, std::vector<cacheLine *> local);
   CacheController(uint_fast32_t size, uint_fast32_t assoc, uint_fast32_t blocksize, uint_fast8_t num_caches, uint_fast8_t coherence_type);


public:

   void access(uint_fast32_t addr, uint_fast8_t proc, bool write);
   void report();
   static CacheController* instance(uint_fast32_t size, uint_fast32_t assoc, uint_fast32_t blocksize, uint_fast8_t num_caches, uint_fast8_t coherence_type);
   ~CacheController();
   void csv_dump(std::string *specifics);

};


#endif //MULTIPROCCACHE_CACHECONTROLLER_H
