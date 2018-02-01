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
#include <string.h>
using namespace std;

#include "sort.hh"
void
parallelSort (int N, keytype* A);
void
mergeSort (keytype* A, int l, int r, keytype* B, int s, int lvl);
// void
// mergeSort (keytype* A, int l, int r, int lvl);
void
merge (keytype* A, int l1, int r1, int l2, int r2, keytype* B, int l3, int lvl);
int
binaryS (int tar, keytype* A, int l, int r);
void
mergeSequential (keytype* A, int l1, int r1, int l2, int r2, keytype* B, int l3);


static int compare (const void* a, const void* b)
{
  keytype ka = *(const keytype *)a;
  keytype kb = *(const keytype *)b;
  if (ka < kb)
    return -1;
  else if (ka == kb)
    return 0;
  else
    return 1;
}
void sequentialQSort (int N, keytype* A, keytype* B, int s)
{
  qsort (A, N, sizeof (keytype), compare);
  memcpy (B+s, A, (N) * sizeof (keytype));
}


void
parallelSort (int N, keytype* A)
{
    //omp_set_nested(1);
    //omp_set_num_threads (4);
    int depth = 0;
    //for(int i = 0; i<N; i++) printf("%d\n",A[i]);
    //keytype* ans = newKeys(N);
    #pragma omp parallel
    {
         #pragma omp single nowait
        {
            depth = omp_get_num_threads();
            mergeSort (A, 0, N-1, A, 0, depth);
        }
              
    }
    //for(int i = 0; i<N; i++) printf("%d\n",A[i]);
  /* Lucky you, you get to start from scratch */
}

void
mergeSort (keytype* A, int l, int r, keytype* B, int s, int lvl){
        int n = r - l + 1;
        //printf("%d\n",n);
        if(n<1) 
            return;
        else if(n==1){
            B[s] = A[l];
        }
        else {
            keytype* T = newKeys(n);
            int mid = (l+r)>>1, len = mid - l + 1;
            if(lvl>1)
            {
                #pragma omp task
                mergeSort(A, l, mid, T, 0, lvl/2);
                mergeSort(A, mid+1, r, T, len, lvl/2);
                #pragma omp taskwait

            }
            else {
                sequentialQSort (mid-l+1, A+l, T, 0);
                sequentialQSort (r-mid, A+mid+1, T, len);
                //mergeSort(A, l, mid, T, 0, lvl);
                //mergeSort(A, mid+1, r, T, len, lvl);
            }
            merge(T, 0, len-1, len, n-1, B, s, lvl);
            free(T);
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
merge (keytype* A, int l1, int r1, int l2, int r2, keytype* B, int l3, int lvl){
        //if(lvl<4) printf("Thread %d is merging %d ~ %d with %d ~ %d \n", omp_get_thread_num(), l1, r1, l2, r2); 
        int n1 = r1 - l1 + 1, n2 = r2 - l2 + 1;
        if(lvl>1){
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
            merge(A, l1, mid1-1, l2, mid2-1, B, l3, lvl/2);
            merge(A, mid1+1, r1, mid2, r2, B, mid3+1, lvl/2);
            #pragma omp taskwait   
        }
        else {
            mergeSequential(A, l1, r1, l2, r2, B, l3);
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


//if(lvl<4) printf("Thread %d is sorting %d through %d\n", omp_get_thread_num(), l, r);  
            // printf("%d\n",omp_get_max_threads());
            // printf("Num Threads:%d ",omp_get_num_threads());
            // printf("id:%d\n",omp_get_thread_num());
                //keytype* T = newKeys(n);

/* eof */