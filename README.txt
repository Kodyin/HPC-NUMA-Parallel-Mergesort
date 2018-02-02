
To run, 
	make mergesort-omp
	./mergesort-omp 10000000

This is a NUMA oriented algorithm implementation with some auxiliary memory allocation.

We are stuck for a while about the NUMA architecture. However, by allocate some more auxiliary memory due to the first touch property, we finally get a 5 time speed up on the 8-core cluster.

On a shared memory 4-core laptop, the same code get 3 time speed up, though the non-NUMA version code might get higher number on a shared memory machine. 

However, I compared the two version of code on cluster with same algorithm and slightly different memory allocation. Though NUMA version spends more memory and time to do allocation, initialization and deallocation, it still beats the non NUMA version code on the cluster, which proves that for NUMA architecture and memory affinity.
