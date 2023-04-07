# SPM Course Assignments

Every Directory contains the solution of the Assignment.

## Assignment 1:
Write a program that transforms a vector<int> into another vector<int> where position i is the result of the computation of function f (given) on the corresponding position of the first vector (w[i] = f(v[i])). Try to use first a sequential solution then a solution where portions of the vector are given to a number of threads given by command line. <br />  

## Assignment 2:
Prepare a divide and conquer implementation of the vector transformation used in the first assignment. Namely: <br />

a) if the vector is short (use a threshold m, that may be changed), just apply f (parameter) over all the vector items and return; <br />
b) if the vector length is longer than m split the vector into 2 chunks (half and half) and compute the two halves in parallel, then "conquer" the results joining them in a single result vector. <br />

"Compute in parallel" means either using threads or asyncs, whatever you like. <br />
Please consider the amount of overall threads/asyncs you'll generate. Also consider the possibility to split the vector of len > m in more than 2 subvectors, e.g. in k (param) sub vectors. <br />

## Assignment 3:
Parallel implementation of a simplified version of google map reduce pattern, on shared memory multicore, using c++ threads. <br />

## Assignment 4:
 consider a matrix NxN. In a cycle,  turn the i,j value of the matrix into the average of the i,j value and of its N, S, E, W neighbours (if present. N value is the i,(j-1), E value is the (i+1),j, etc.), for any i,j up to either maxIter iterations have been computed or to the point the sum of the absolute value of the differences in between old and new i,j values runs below some epsilon (epsilon and maxiter being parameters from the command line).
