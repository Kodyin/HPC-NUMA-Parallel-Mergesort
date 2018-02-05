/**
 *  \file parallel-mergesort.cc
 *
 *  \brief Implement your parallel mergesort in this file.
 */

#include <assert.h>
#include <omp.h>
#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <algorithm>
#include <string.h>
using namespace std;

#include "sort.hh"
void
parallelSort (int N, keytype* A);
void
mergeSort (keytype* A, int l, int r, keytype* B, int s, int lvl);
void
Pmerge (keytype* A, int l1, int r1, int l2, int r2, keytype* B, int l3, int lvl);
int
binaryS (int tar, keytype* A, int l, int r);
void 
MSort(keytype* A,keytype* T,int l,int r);
void
mergeSequential (keytype* A, int l1, int r1, int l2, int r2, keytype* B, int l3);

void
parallelSort (int N, keytype* A)
{
    int depth = 0;
    #pragma omp parallel
    {
         #pragma omp single nowait
        {
            depth = omp_get_num_threads();
            mergeSort (A, 0, N-1, A, 0, depth);
        }
              
    }
  /* Lucky you, you get to start from scratch */
}

void
mergeSort (keytype* A, int l, int r, keytype* B, int s, int lvl){
        int n = r - l + 1;
        if(n<1) 
            return;
        else if(n==1) 
            B[s] = A[l];
        else {
            int mid = (l+r)>>1, len = mid - l + 1;
            if(lvl>1)/* Spawning */
            {
                /* Assign more memory for NUMA architecture for this thread and child thread*/
                keytype* T = newKeys(n);    
                #pragma omp task
                mergeSort(A, l, mid, T, 0, lvl/2);
                mergeSort(A, mid+1, r, T, len, lvl/2);
                #pragma omp taskwait
                Pmerge(T, 0, len-1, len, n-1, B, s, lvl);
                free(T);
            }
            /* Sequential merge*/
            else {
                std::sort(A + l, A + mid + 1);
                std::sort(A + mid + 1, A + r + 1);
                std::merge(A + l, A + mid + 1, A + mid + 1, A + r + 1, B+s);
            }
        }
    
}
int
binaryS (int tar, keytype* A, int l, int r){
    int low = l, high = max(l, r+1);
    while(low<high){
        int mid = (low+high)>>1;
        if(tar<=A[mid]) high = mid;
        else low = mid + 1;
    }
    return high;
}

void
Pmerge (keytype* A, int l1, int r1, int l2, int r2, keytype* B, int l3, int lvl){
        int n1 = r1 - l1 + 1, n2 = r2 - l2 + 1;
        if(lvl>1) /* Spawning */
        {
            if(n1<n2){
                swap(l1,l2);
                swap(r1,r2);
                swap(n1,n2);
            }
            if(n1<=0) return;
            int mid1 = (l1+r1)>>1;
            int mid2 = binaryS(A[mid1], A, l2, r2);
            int mid3 = l3 + (mid1 - l1) + (mid2 - l2);
            B[mid3] =  A[mid1];

            #pragma omp task 
            Pmerge(A, l1, mid1-1, l2, mid2-1, B, l3, lvl/2);
            Pmerge(A, mid1+1, r1, mid2, r2, B, mid3+1, lvl/2);
            #pragma omp taskwait   
        }
        else {
            std::merge(A + l1, A + r1 + 1, A + l2, A + r2 + 1, B+l3);
        }                               
}

 /* Sequential */
void 
MSort(keytype* A,keytype* T,int l,int r)
{
    int mid;
    if(l<r)
    {
        mid=(l+r)/2;
        MSort(A,T,l,mid);
        MSort(A,T,mid+1,r);
        mergeSequential(A,l,mid,mid+1,r, T, l);
        memcpy(A+l, T+l, (r-l+1) * sizeof(keytype));
    }
}

void
mergeSequential (keytype* A, int l1, int r1, int l2, int r2, keytype* B, int l3){
    int a = l1, b = l2;
    for(int i = 0; i<r1-l1+1+r2-l2+1; i++){
        if(a>r1) {
            memcpy(B + l3 + i, A + b, (r2 - b + 1) * sizeof(keytype));
            break;
        }
        else if(b>r2) {
            memcpy(B + l3 + i, A + a, (r1 - a + 1) * sizeof(keytype));
            break;
        }
        else if(A[a] < A[b]) B[l3+i] = A[a++];
        else B[l3+i] = A[b++];
    }
}


/* eof */
