# SAXPY
OpenCL implementation of its "Hello-World": *saxpy* - Single precision real *Alpha X plus Y*.

## Algorithm
A simple C implementation of saxpy:
```C
    void saxpy(int n, float a, float *b, float *c){
        for(int i = 0, i < n; ++i){
            y[i] = a*x[i] + y[i];
        }
    }
```
