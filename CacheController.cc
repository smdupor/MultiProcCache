//
// Created by smdupor on 10/23/21.
//

#include "CacheController.h"

CacheController::CacheController(uint_fast32_t size, uint_fast32_t assoc, uint_fast32_t blocksize,
                                 uint_fast8_t num_caches, uint_fast8_t coherence_type) {
type = coherence_type;
caches = (Cache *) malloc(num_caches*sizeof(Cache));
qty = num_caches;

for(size_t i=0;i<num_caches;++i) {
   caches[i] = *new Cache(size, assoc, blocksize, i, coherence_type);
}

for (size_t i=0;i<30;++i)
   count[i]=0;

   // DO MSI

   //DO MESI

   // DO DRAGON

}

void CacheController::access(uint_fast32_t addr, uint_fast8_t proc, bool write) {
   std::vector<cacheLine *> local;
   for(int i =0; i<qty;++i)
      local.emplace_back(caches[i].findLine(addr)); ////////////////////// SWAPPPED from for Cache in caches
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
      }

         // Case Write and (other) proc is in M
      else {
         caches[Mloc->get_proc()].invalidate(addr);
         Mloc->invalidate();
         caches[Mloc->get_proc()].flush();
         local[proc]=caches[proc].findLineToReplace(addr);
         local[proc]->set_state(M);
         caches[proc].transfer();
         caches[proc].bus_rdx();
         caches[proc].Access(addr, 'w');
      }
   }
   else if(write && (has_S || has_E)) {
      // Case write and This proc is the ONLY E
      if (local[proc] && has_E) {
         caches[proc].Access(addr, 'w');
         local[proc]->set_state(M);
      }
         // Case Write and multiple procs in S including this
      else if(local[proc]) {
         for(cacheLine *l : local){
            if(l && l->get_proc() != proc) {
               caches[l->get_proc()].invalidate(addr);
               l->invalidate();
            }
         }
         local[proc]->set_state(M);
         caches[proc].Access(addr, 'w');
         count[3]++;
      }
         // Case write, other procs in S or E, this proc invalid
      else{
         for(cacheLine *l : local){
            if(l && l->get_proc() != proc) {
               caches[l->get_proc()].invalidate(addr);
               l->invalidate();
            }
         }
         local[proc]=caches[proc].findLineToReplace(addr);
         local[proc]->set_state(M);
         caches[proc].transfer();
         caches[proc].bus_rdx();
         caches[proc].Access(addr, 'w');
      }
   }
   else if (write && !has_M && !has_S && !has_E){ // case write, all invalid
      local[proc]=caches[proc].findLineToReplace(addr);
      local[proc]->set_state(M);
      caches[proc].bus_rdx();
      caches[proc].Access(addr, 'w');
   }
   else if(!write && (has_S || has_E)){
      if(local[proc]) { // case read and this proc in S or E
         caches[proc].Access(addr, 'r');
      }
      else{ // case read and other procs in S or e, this proc invalid
         local[proc]=caches[proc].findLineToReplace(addr);
         local[proc]->set_state(S);
         if(Eloc) {
            Eloc->set_state(S);
            caches[Eloc->get_proc()].intervention_mesi();
         }
         caches[proc].transfer();
         caches[proc].Access(addr, 'r');
      }
   }
   else if (!write && has_M) {
      // Case rd and (this) proc is in M
      if (local[proc] && local[proc]->get_state() == M) {
         caches[proc].Access(addr, 'r');
      }
         // Case rd and (other) proc is in M
      else {
         Mloc->set_state(S);
         Mloc->setFlags(VALID);
         caches[Mloc->get_proc()].intervention_mesi();
         caches[Mloc->get_proc()].flush_mesi();
         local[proc]=caches[proc].findLineToReplace(addr);
         local[proc]->set_state(S);
         caches[proc].transfer();
         caches[proc].Access(addr, 'r');
      }
   }
   else{ // case rd and all invalid
      local[proc]=caches[proc].findLineToReplace(addr);
      local[proc]->set_state(E);
      caches[proc].Access(addr, 'r');
   }
}

void CacheController::dragon_access(uint_fast32_t addr, uint_fast8_t proc, bool write, std::vector<cacheLine *> local) {
   bool has_SM=false, has_SC=false, has_M=false, has_E=false;
   cacheLine *Mloc, *Eloc, *SMloc;
   count[7]++;
   for(cacheLine *l : local) {
      if (l && l->get_state() == SM) {
         has_SM = true;
         SMloc = l;
      }
      else if (l && l->get_state() == M) {
         has_M = true;
         Mloc=l;
      }
      else if(l && l->get_state() == E) {
         has_E = true;
         Eloc=l;
      }
      else if(l && l->get_state() == SC) {
         has_SC = true;
      }
   }

   if(has_M && has_SM) {
      std::cout<< "M and SM Collision on " << addr << "where count 6 is"<< count[7] << std::endl;
   }
   if(has_E && has_M) {
      std::cout<< "E and M Collision on " << addr << "where count 6 is"<< count[7] << std::endl;
   }
   if(has_E && has_SM) {
      std::cout<< "E and SM Collision on " << addr << "where count 6 is"<< count[7] << std::endl;
   }
   if(has_E && has_SC) {
      std::cout<< "E and SC Collision on " << addr << "where count 6 is"<< count[7] << std::endl;
   }
   if(has_M && has_SC) {
      std::cout<< "E and SC Collision on " << addr << "where count 6 is"<< count[7] << std::endl;
   }



   if(write) { //ALL WRITES
      // Case write and (this) proc is in E or M or Sm
      if ((has_E && Eloc->get_proc() == proc) || (has_M && Mloc->get_proc() == proc) ||
          (has_SM && SMloc->get_proc() == proc)) {
         caches[proc].Access(addr, 'w');
         if(has_E) {
            local[proc]->set_state(M);
            count[18]++;
         }
         /*if(has_SM && !has_SC) {
            local[proc]->set_state(M);
         }*/
      }
      else if (has_SC && !has_SM){
         if(!local[proc]) {
            local[proc]=caches[proc].findLineToReplace(addr);
          //  local[proc]->setFlags(VALID);
            count[17]++;
         }

         local[proc]->set_state(SM);
         caches[proc].Access(addr, 'w');

      }

      // Case write and Other proc in E
      else if(has_E && Eloc->get_proc() != proc) {
         if(!local[proc]){
            local[proc]=caches[proc].findLineToReplace(addr);
            count[16]++;
          //  local[proc]->setFlags(VALID);
            if(local[proc]->get_state()==SC){
               count [17]++;
            }
         }
         local[Eloc->get_proc()]->set_state(SC);
         caches[Eloc->get_proc()].intervention_mesi();
         caches[proc].Access(addr, 'w');
         local[proc]->set_state(SM);
         count[0]++;
      }
      // Case write and Other proc in M
      else if(has_M && Mloc->get_proc() != proc) {
         if(!local[proc]) {
            local[proc]=caches[proc].findLineToReplace(addr);
            if(local[proc]->get_state()==SC){
               count [17]++;
            //   local[proc]->setFlags(VALID);
            }
            count[15]++;
         }
         local[Mloc->get_proc()]->set_state(SC);
         caches[Mloc->get_proc()].intervention_mesi();
         caches[Mloc->get_proc()].flush();
         Mloc->setFlags(VALID);
         caches[proc].Access(addr, 'w');
         local[proc]->set_state(SM);

         count[1]++;
      }
      // Case write and Other proc in SM
      else if(has_SM && SMloc->get_proc() != proc) {
         if(!local[proc]) {
            local[proc]=caches[proc].findLineToReplace(addr);
            //if(local[proc]->get_state() == SC)
             //  local[proc]->setFlags(VALID);
            caches[SMloc->get_proc()].flush();
            SMloc->setFlags(VALID);
            count[14]++;

         }

         local[SMloc->get_proc()]->set_state(SC);

         caches[proc].Access(addr, 'w');
         local[proc]->set_state(SM);
         count[2]++;
      }
      // Case write and no caches hold valid line
      else {
         count[13]++;
         local[proc]=caches[proc].findLineToReplace(addr);
         //if(local[proc]->get_state() == SC)
          //  local[proc]->setFlags(VALID);

         caches[proc].Access(addr, 'w');
         local[proc]->set_state(M);

      }

   }
   else { // ALL READS
      // Case read and (this) proc is in E or M or Sm
      if ((has_E && Eloc->get_proc() == proc) || (has_M && Mloc->get_proc() == proc) ||
          (has_SM && SMloc->get_proc() == proc)||(has_SC && local[proc])) {
         caches[proc].Access(addr, 'r');
      }
         // Case read and Other proc in E
      else if(has_E && Eloc->get_proc() != proc) {
         if(!local[proc]) {
            local[proc]=caches[proc].findLineToReplace(addr);
            count[8]++;
            //if(local[proc]->get_state() == SC)
           //   local[proc]->setFlags(VALID);
         }
         caches[proc].Access(addr, 'r');
         local[Eloc->get_proc()]->set_state(SC);
         caches[Eloc->get_proc()].intervention_mesi();
         local[proc]->set_state(SC);
         count[3]++;

      }
         // Case read and Other proc in M
      else if(has_M && Mloc->get_proc() != proc) {
         if(!local[proc]){
            local[proc]=caches[proc].findLineToReplace(addr);
            //if(local[proc]->get_state() == SC)
             //  local[proc]->setFlags(VALID);
            count[9]++;
         }

         local[Mloc->get_proc()]->set_state(SM);
         caches[Mloc->get_proc()].intervention_mesi();
         caches[Mloc->get_proc()].flush();
         Mloc->setFlags(VALID);
         caches[proc].Access(addr, 'r');
         local[proc]->set_state(SC);
         count[4]++;
      }
         // Case read and Other proc in SM and this proc invalid
      else if(has_SM && SMloc->get_proc() != proc && !local[proc]) {
         local[proc]=caches[proc].findLineToReplace(addr);
         caches[SMloc->get_proc()].flush();
         //SMloc->setFlags(VALID);
         caches[proc].Access(addr, 'r');
         local[proc]->set_state(SC);
         count[5]++;
      }
         // Case read and Other proc in SC
      else if(has_SC ) {
         if(!local[proc]) {
            local[proc] = caches[proc].findLineToReplace(addr);
            count[11]++;
         }
         caches[proc].Access(addr, 'r');
         local[proc]->set_state(SC);
         count[6]++;
      }

         // Case read and no caches hold valid line
      else {
         local[proc]=caches[proc].findLineToReplace(addr);
         caches[proc].Access(addr, 'r');
         local[proc]->set_state(E);
         count[12]++;
      }

   }
}

void CacheController::report() {
   uint_fast32_t diff = 0;
   for(int i =0; i<qty;++i) {
                                     ////////////////////// SWAPPPED from for Cache in caches
      caches[i].printStats();
      diff += caches[i].getFU();
   }

   //std::cout<< "Found = " << diff << " To Go: "<< 7580-diff << std::endl;

   for(size_t i=0;i<18;++i){
      std::cout << "Slot " << i<< " = " << count[i]<<std::endl;

   }
}

CacheController *
CacheController::instance(uint_fast32_t size, uint_fast32_t assoc, uint_fast32_t blocksize, uint_fast8_t num_caches,
                          uint_fast8_t coherence_type) {
   static CacheController instance(size, assoc, blocksize, num_caches, coherence_type);
   return &instance;
}

CacheController::~CacheController() {
   free(caches);

}
