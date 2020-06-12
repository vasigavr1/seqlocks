# Seqlocks
An implementation of sequence locks (seqlocks) for lock-free reads in C using atomics

*The header file contains the implementation.
*The source file contains usage examples of reading and writing

## Note
The implementation assumes x86 architecture (i.e. the TSO memory model) where the memory ordering read --> acquire is enforced (and only needs a compiler barrier)
For more information for a portable implementation, I recommend Boehms's [paper]{https://www.hpl.hp.com/techreports/2012/HPL-2012-68.pdf}
