//
// Created by smdupor on 10/23/21.
//

#include "CacheController.h"

CacheController::CacheController(uint_fast32_t size, uint_fast32_t assoc, uint_fast32_t blocksize,
                                 uint_fast8_t num_caches, uint_fast8_t coherence_type) {
type = coherence_type;
for(size_t i=0;i<num_caches;++i) {
   caches.emplace_back(Cache(size, assoc, blocksize, i));
}

   // DO MSI

   //DO MESI

   // DO DRAGON

}

void CacheController::access(uint_fast32_t addr, uint_fast8_t proc, bool write) {
   std::vector<cacheLine *> local;
   for(Cache &c : caches)
      local.emplace_back(c.findLine(addr));
   switch (type) {
     case MSI:
         msi_access(addr, proc, write, local);
         break;
     case MESI:
         mesi_access(addr, proc, write, local);
         break;
     case DRAGON:
         dragon_access(addr, proc, write, local);
         break;
   }
}

void CacheController::msi_access(uint_fast32_t addr, uint_fast8_t proc, bool write, std::vector<cacheLine *> local) {
   bool has_S=false, has_M=false;
   uint_fast8_t num_s=0;
   cacheLine *Mloc;

   for(cacheLine *l : local) {
      if (l && l->get_state() == S) {
         has_S = true;
         ++num_s;
      }
      else if (l && l->get_state() == M) {
         has_M = true;
         Mloc=l;
      }
   }

   if(write && has_M) {
      // Case Write and (this) proc is in M
      if (local[proc] && local[proc]->get_state() == M)
         caches[proc].Access(addr, 'w');

      // Case Write and (other) proc is in M
      else {
         caches[Mloc->get_proc()].writeBack(addr);
         caches[Mloc->get_proc()].invalidate();
         Mloc->invalidate();
         local[proc]=caches[proc].findLineToReplace(addr);
         local[proc]->set_state(M);
         caches[proc].Access(addr, 'w');
      }
   }
   else if(write && has_S) {
      // Case write and This proc is the ONLY S
      if (local[proc] && local[proc]->get_state() == S && num_s == 1) {
         caches[proc].Access(addr, 'w');
         local[proc]->set_state(M);    //BusUpgr ... intervention?
      }
      // Case Write and multiple procs in S inclusive of this proc
      else if(local[proc]) {
         for(cacheLine *l : local){
            if(l && l->get_proc() != proc) {
               caches[l->get_proc()].invalidate();
               l->invalidate();
            }
         }
         local[proc]->set_state(M);
         caches[proc].Access(addr, 'w');
      }
      // Case write, other procs in S, this proc invalid
      else{
         for(cacheLine *l : local){
            if(l && l->get_proc() != proc) {
               caches[l->get_proc()].invalidate();
               l->invalidate();
            }
         }
         local[proc]=caches[proc].findLineToReplace(addr);
         local[proc]->set_state(M);
         caches[proc].Access(addr, 'w');
      }
   }
   else if (write && !has_M && !has_S){
      local[proc]=caches[proc].findLineToReplace(addr);
      local[proc]->set_state(M);
      caches[proc].Access(addr, 'w');
   }
   else if(!write && has_S){
      if(local[proc]) {
         caches[proc].Access(addr, 'r');
      }
      else{
         local[proc]=caches[proc].findLineToReplace(addr);
         local[proc]->set_state(S);
         caches[proc].Access(addr, 'r');
      }
   }
   else if (!write && has_M) {
      // Case rd and (this) proc is in M
      if (local[proc] && local[proc]->get_state() == M)
         caches[proc].Access(addr, 'r');

         // Case rd and (other) proc is in M
      else {
         caches[Mloc->get_proc()].writeBack(addr);
         caches[Mloc->get_proc()].invalidate();
         Mloc->invalidate();
         local[proc]=caches[proc].findLineToReplace(addr);
         local[proc]->set_state(S);
         caches[proc].Access(addr, 'r');
      }
   }
   else{
      local[proc]=caches[proc].findLineToReplace(addr);
      local[proc]->set_state(S);
      caches[proc].Access(addr, 'r');
   }
}

void CacheController::mesi_access(uint_fast32_t addr, uint_fast8_t proc, bool write, std::vector<cacheLine *> local) {

}

void CacheController::dragon_access(uint_fast32_t addr, uint_fast8_t proc, bool write, std::vector<cacheLine *> local) {


}

void CacheController::report() {
   for(Cache &c:caches)
      c.printStats();
}

CacheController *
CacheController::instance(uint_fast32_t size, uint_fast32_t assoc, uint_fast32_t blocksize, uint_fast8_t num_caches,
                          uint_fast8_t coherence_type) {
   static CacheController instance(size, assoc, blocksize, num_caches, coherence_type);
   return &instance;
}