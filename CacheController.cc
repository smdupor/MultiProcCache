//
// Created by smdupor on 10/23/21.
//

#include "CacheController.h"

CacheController::CacheController(uint_fast32_t size, uint_fast32_t assoc, uint_fast32_t blocksize,
                                 uint_fast8_t num_caches, uint_fast8_t coherence_type) {
type = coherence_type;
for(size_t i=0;i<num_caches;++i) {
   caches.emplace_back(Cache(size, assoc, blocksize, i, coherence_type));
}

for (size_t i=0;i<30;++i)
   count[i]=0;

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
         //caches[Mloc->get_proc()].writeBack(addr);
         caches[Mloc->get_proc()].invalidate(addr);
         Mloc->invalidate();
         caches[Mloc->get_proc()].flush();
         local[proc]=caches[proc].findLineToReplace(addr);
         local[proc]->set_state(M);
         caches[proc].bus_rdx();
         caches[proc].Access(addr, 'w');

      }
   }
   else if(write && has_S) {
      // Case write and This proc is the ONLY S
      if (local[proc] && local[proc]->get_state() == S && num_s == 1) {
         caches[proc].Access(addr, 'w');
         caches[proc].bus_rdx();
         local[proc]->set_state(M);    //BusUpgr ... intervention?
      }
      // Case Write and multiple procs in S inclusive of this proc
      else if(local[proc]) {
         for(cacheLine *l : local){
            if(l && l->get_proc() != proc) {
               caches[l->get_proc()].invalidate(addr);
               l->invalidate();
            }
         }
         local[proc]->set_state(M);
         caches[proc].bus_rdx();
         caches[proc].Access(addr, 'w');
      }
      // Case write, other procs in S, this proc invalid
      else{
         for(cacheLine *l : local){
            if(l && l->get_proc() != proc) {
               caches[l->get_proc()].invalidate(addr);
               l->invalidate();
            }
         }
         local[proc]=caches[proc].findLineToReplace(addr);
         local[proc]->set_state(M);
         caches[proc].bus_rdx();
         caches[proc].Access(addr, 'w');
      }
   }
   else if (write && !has_M && !has_S){ // case write, all invalid
      local[proc]=caches[proc].findLineToReplace(addr);
      local[proc]->set_state(M);
      caches[proc].bus_rdx();
      caches[proc].Access(addr, 'w');
   }
   else if(!write && has_S){
      if(local[proc]) { // case read and this proc in S
         caches[proc].Access(addr, 'r');
      }
      else{ // case read and other procs in S, this proc invalid
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
         //caches[Mloc->get_proc()].writeBack(addr);
         //caches[Mloc->get_proc()].invalidate(addr);
         Mloc->set_state(S);
         Mloc->setFlags(VALID);
         caches[Mloc->get_proc()].intervention();
         caches[Mloc->get_proc()].flush();
//         Mloc->invalidate();
         local[proc]=caches[proc].findLineToReplace(addr);
         local[proc]->set_state(S);
         caches[proc].Access(addr, 'r');
      }
   }
   else{ // case rd and all invalid
      local[proc]=caches[proc].findLineToReplace(addr);
      local[proc]->set_state(S);
      caches[proc].Access(addr, 'r');
   }
}

void CacheController::mesi_access(uint_fast32_t addr, uint_fast8_t proc, bool write, std::vector<cacheLine *> local) {


   bool has_S=false, has_M=false, has_E=false;
   uint_fast8_t num_s=0;
   cacheLine *Mloc, *Eloc;

   for(cacheLine *l : local) {
      if (l && l->get_state() == S) {
         has_S = true;
         ++num_s;
      }
      else if (l && l->get_state() == M) {
         has_M = true;
         Mloc=l;
      }
      else if(l && l->get_state() == E) {
         has_E = true;
         Eloc=l;
      }
   }

   if(write && has_M) {
      // Case Write and (this) proc is in M
      if (local[proc] && local[proc]->get_state() == M) {
         caches[proc].Access(addr, 'w');
         count[0]++;
      }

         // Case Write and (other) proc is in M
      else {
         //caches[Mloc->get_proc()].writeBack(addr);
         caches[Mloc->get_proc()].invalidate(addr);
         Mloc->invalidate();
         caches[Mloc->get_proc()].flush();
         local[proc]=caches[proc].findLineToReplace(addr);
         local[proc]->set_state(M);
         caches[proc].transfer();
         caches[proc].bus_rdx();
         caches[proc].intervention_mesi();
         caches[proc].Access(addr, 'w');
         count[1]++;
      }
   }
   else if(write && (has_S || has_E)) {
      // Case write and This proc is the ONLY E
      if (local[proc] && has_E) {
         caches[proc].Access(addr, 'w');
         local[proc]->set_state(M);
         //caches[proc].intervention_mesi();
         count[2]++;
      }
         // Case Write and multiple procs in S including this
      else if(local[proc]) {

         for(cacheLine *l : local){
            if(l && l->get_proc() != proc) {
               caches[l->get_proc()].invalidate(addr);
               count[11]++;
               l->invalidate();
            }
         }
         local[proc]->set_state(M);
        caches[proc].intervention_mesi();

         caches[proc].Access(addr, 'w');
         count[3]++;
      }
         // Case write, other procs in S or E, this proc invalid
      else{
         for(cacheLine *l : local){
            if(l && l->get_proc() != proc) {
               caches[l->get_proc()].invalidate(addr);
               l->invalidate();
               count[12]++;
            }
         }
         local[proc]=caches[proc].findLineToReplace(addr);
         local[proc]->set_state(M);
         caches[proc].transfer();
         caches[proc].intervention_mesi();
         caches[proc].bus_rdx();
         caches[proc].Access(addr, 'w');
         count[4]++;
      }
   }
   else if (write && !has_M && !has_S && !has_E){ // case write, all invalid
      local[proc]=caches[proc].findLineToReplace(addr);
      local[proc]->set_state(M);
      caches[proc].bus_rdx();
      //caches[proc].intervention_mesi(); // has 3
      caches[proc].Access(addr, 'w');
      count[5]++;
   }
   else if(!write && (has_S || has_E)){
      if(local[proc]) { // case read and this proc in S or E
         caches[proc].Access(addr, 'r');
         count[6]++;
      }
      else{ // case read and other procs in S or e, this proc invalid
         local[proc]=caches[proc].findLineToReplace(addr);
         local[proc]->set_state(S);
         if(Eloc) {
            Eloc->set_state(S);
            count[13]++;
         }
         //caches[proc].intervention_mesi();
         caches[proc].transfer();
         caches[proc].Access(addr, 'r');
         count[7]++;
      }
   }
   else if (!write && has_M) {
      // Case rd and (this) proc is in M
      if (local[proc] && local[proc]->get_state() == M) {
         caches[proc].Access(addr, 'r');
         count[8]++;
      }
         // Case rd and (other) proc is in M
      else {
         Mloc->set_state(S);
         Mloc->setFlags(VALID);
         //caches[proc].intervention_mesi();
         caches[Mloc->get_proc()].flush_mesi();
         local[proc]=caches[proc].findLineToReplace(addr);
         local[proc]->set_state(S);
         caches[proc].transfer();
         caches[proc].Access(addr, 'r');
         count[9]++;
      }
   }
   else{ // case rd and all invalid
      local[proc]=caches[proc].findLineToReplace(addr);
      local[proc]->set_state(E);
      caches[proc].Access(addr, 'r');
      count[10]++;
   }
}

void CacheController::dragon_access(uint_fast32_t addr, uint_fast8_t proc, bool write, std::vector<cacheLine *> local) {


}

void CacheController::report() {
   uint_fast32_t diff = 0;
   for(Cache &c:caches) {
      c.printStats();
      diff += c.getFU();
   }

   std::cout<< "Found = " << diff << " To Go: "<< 7580-diff << std::endl;

   for(size_t i=0;i<14;++i){
      std::cout << "Slot " << i<< " = " << count[i]<<std::endl;

   }
}

CacheController *
CacheController::instance(uint_fast32_t size, uint_fast32_t assoc, uint_fast32_t blocksize, uint_fast8_t num_caches,
                          uint_fast8_t coherence_type) {
   static CacheController instance(size, assoc, blocksize, num_caches, coherence_type);
   return &instance;
}