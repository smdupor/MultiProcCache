#!/bin/bash

#./smp_cache 8192 8 64 4 0 trace/canneal.04t.debug > output
#diff -iw output val.v2/MSI_debug.val
#rm output
#echo "MSI Debug trace"
#
#./smp_cache 8192 8 64 4 0 trace/canneal.04t.longTrace > output
#diff -iw output val.v2/MSI_long.val
#rm output
#echo "MSI Long trace"

./smp_cache 8192 8 64 4 1 trace/canneal.04t.debug > output
diff -iw output val.v2/MESI_debug.val
rm output
echo "MESI Debug trace"

./smp_cache 8192 8 64 4 1 trace/canneal.04t.longTrace > output
diff -iw output val.v2/MESI_long.val
rm output
echo "MESI Long trace"
#
#./smp_cache 8192 8 64 4 2 trace/canneal.04t.debug > output
#diff -iw output val.v2/Dragon_debug.val
#rm output
#echo "DRAGON Debug trace"
#
#./smp_cache 8192 8 64 4 2 trace/canneal.04t.longTrace > output
#diff -iw output val.v2/Dragon_long.val
##rm output
#echo "DRAGON Long trace"

#for i in 1 2 3 4 5 6 7
#do
#
#  echo "trace$i"
#  diff -iw gcc.$i z_Resources/gcc.output$i.txt
#done