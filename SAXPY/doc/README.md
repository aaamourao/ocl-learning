# SAXPY
OpenCL implementation of its "Hello-World": **saxpy** - Single precision real *Alpha X plus Y*.

## Algorithm
A simple **C** implementation of **saxpy**:
```C
    void saxpy(int n, float a, float *x, float *y, float *z){
        for(int i = 0, i < n; ++i){
            z[i] = a*x[i] + y[i];
        }
    }
```
