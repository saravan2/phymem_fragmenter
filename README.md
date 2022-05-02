# phymem_fragmenter
Kernel Loadable Module to fragment Physical Memory; exhausts contiguous physical memory (from a particular order)

Makes higher order page allocation request and splits the chunk into individual pages. The KLM fragments the
segment by holding onto the first page and freeing the rest. It denies memory compaction and reclaim to
happen on the just freed segment of memory for subsequent requests of same size.

On some scenarios, compaction and reclaim might create higher order pages even after phymem_fragmenter run.
We could then use sidekick module to catch those newly created higher order pages. Sidekick module can be
rmmode and insmod multiple times to exhaust all relevant higher order pages.

KLMs free the fragmented pages held in custody upon module exit.

Usage :

KVERSION=<version> make  {all|debug|clean}

cat /proc/buddyinfo
Node 0, zone      DMA      0      0      0      0      0      0      0      0      1      1      3
Node 0, zone    DMA32    353    353    352    352    353    353    354    321    304    319     63
Node 0, zone   Normal  21040  26612  21485  10084   4682   1935    824    411    158    150   3643

insmod ./phymem_fragmenter.ko

dmesg | tail -1f
[  478.008842] Starting fragmenter

cat /proc/buddyinfo
Node 0, zone      DMA      0      0      0      0      0      0      0      0      1      1      3
Node 0, zone    DMA32   6125   6125   6124   6124   6125   6125      0      0      0      0      0
Node 0, zone   Normal  93366 100711  95660  79977  70940  69029     28      1      0      0      0

insmod ./phymem_fragmenter_sidekick.ko

cat /proc/buddyinfo
Node 0, zone      DMA      0      0      0      0      0      0      0      0      1      1      3
Node 0, zone    DMA32   6125   6125   6124   6124   6125   6125      0      0      0      0      0
Node 0, zone   Normal 100108 100641  95673  80278  71061  69141      0      0      0      0      0

rmmod phymem_fragmentor_sidekick
rmmod phymem_fragmentor

dmesg | tail -2f
[  529.882863] Releasing all fragments
[  529.882867] Released all fragments
