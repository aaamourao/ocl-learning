# Vector Addition
OpenCL implementation of a simple **vector addition**. It is
even simpler than  *SAXPY*.

The code was strongly based on the first code sample of
a great OpenCL book (the best one IMHO):
[Heterogeneous Computing with
OpenCL](http://www.heterogeneouscompute.org/?page_id=7).

Just small changes were added, adapting it to **OpenCL 2.0**,
since I just have access to the old **1.2** version of the book.
Both **C++** and **C** samples were added.

## Algorithm
A simple C implementation of a vector addition:
```C
    void vecadd(int n, float *a, float *b, float *c) {
        for(int i = 0, i < n; ++i) {
            c[i] = a[i] + b[i];
        }
    }
```
