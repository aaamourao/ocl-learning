#include <stdio.h>
#include <stdlib.h>

#ifdef __APPLE__
#include <OpenCL/cl.h>
#else
#include <CL/cl.h>
#endif

/*
 * OpenCL kernel function:
 * Implements the computation that
 * are going to be executed by the
 * device: C = A + B
 */
const char *vecadd_kernel =
    "__kernel                        \n"
    "void vecadd(__global int *A,    \n"
    "               __global int *B, \n"
    "               __global int *C) \n"
    "{                               \n"
    "    int idx = get_global_id(0); \n"
    "    C[idx] = A[idx] + B[idx];   \n"
    "}                               \n";

int main(void)
{
    // Constants
    // ------------------------------------------------------------------------
    // Elements in each array
    const int elements = 2048;
    // Dimensions of the work group
    const int workGroupDim = 1;
    // ------------------------------------------------------------------------

    // Declare host arrays and allocate space for them
    size_t datasize = sizeof(int)*elements;
    int *A = (int *)malloc(datasize);
    int *B = (int *)malloc(datasize);
    int *C = (int *)malloc(datasize);

    // Set their values
    // Obs: it's C code: i will be used again, and there is no ++i
    unsigned int i;
    for (i = 0; i < elements; i++) {
        A[i] = i;
        B[i] = i;
    }

    // Get OpenCL platforms available
    cl_platform_id *platforms = NULL;
    cl_uint num_platforms = 0;

    cl_int clStatus = clGetPlatformIDs(0, NULL, &num_platforms);
    platforms = (cl_platform_id *)
                malloc(sizeof(cl_platform_id)*num_platforms);
    clStatus = clGetPlatformIDs(num_platforms, platforms, NULL);

    // Get devices list
    cl_device_id *devices = NULL;
    cl_uint num_devices = 0;

    clStatus = clGetDeviceIDs(platforms[0], CL_DEVICE_TYPE_ALL, 0,
                              NULL, &num_devices);
    devices = (cl_device_id *)
              malloc(sizeof(cl_device_id)*num_devices);
    clStatus = clGetDeviceIDs(platforms[0], CL_DEVICE_TYPE_ALL, num_devices,
                              devices, NULL);

    // Create a context and associate it with the devices
    cl_context context;
    context = clCreateContext(NULL, num_devices, devices, NULL, NULL, &clStatus);

    // Command queue
    cl_command_queue cmdQueue;
    cmdQueue = clCreateCommandQueue(context, devices[0], 0, &clStatus);

    // Allocate space on the device
    cl_mem bufA;
    bufA = clCreateBuffer(context, CL_MEM_READ_ONLY, datasize, NULL, &clStatus);

    cl_mem bufB;
    bufB = clCreateBuffer(context, CL_MEM_READ_ONLY, datasize, NULL, &clStatus);

    // Output buffer
    cl_mem bufC;
    bufC = clCreateBuffer(context, CL_MEM_WRITE_ONLY, datasize,
                          NULL, &clStatus);

    // Write input arrays to the device buffers
    clStatus = clEnqueueWriteBuffer(cmdQueue, bufA, CL_FALSE, 0, datasize,
                                    A, 0, NULL, NULL);
    clStatus = clEnqueueWriteBuffer(cmdQueue, bufB, CL_FALSE, 0, datasize,
                                    B, 0, NULL, NULL);

    // Set program from OpenCL kernel source
    cl_program program = clCreateProgramWithSource(context, 1,
                         (const char **)&vecadd_kernel, NULL, &clStatus);

    // Build it
    clStatus = clBuildProgram(program, num_devices, devices, NULL, NULL, NULL);

    // Create kernel
    cl_kernel kernel = clCreateKernel(program, "vecadd", &clStatus);

    // Link input and output buffers with the kernel
    clStatus = clSetKernelArg(kernel, 0, sizeof(cl_mem), &bufA);
    clStatus = clSetKernelArg(kernel, 1, sizeof(cl_mem), &bufB);
    clStatus = clSetKernelArg(kernel, 2, sizeof(cl_mem), &bufC);

    // Host code defining size of Workgroups
    size_t workGroupSize[workGroupDim];
    workGroupSize[0] = elements;

    // Execute the kernel on the device
    clStatus = clEnqueueNDRangeKernel(cmdQueue, kernel, 1, NULL,
                                      workGroupSize, NULL, 0, NULL, NULL);

    // Get the result from the device
    clEnqueueReadBuffer(cmdQueue, bufC, CL_TRUE, 0, datasize, C, 0, NULL, NULL);

    /*
     * Verify the result by computing the sum for each array element
     * again on the host platform. This is just an example, so,
     * doing it on real applications doesn't make any sense.
     *
     */
    unsigned int result = 1;
    for (i = 0; i < elements; i++) {
        if (C[i] != i+i) {
            result = 0;
            break;
        }
    }
    if (result) {
        printf("Output is correct\n");
    } else {
        printf("Something got wrong...\n");
    }

    // Free resources
    clReleaseKernel(kernel);
    clReleaseProgram(program);
    clReleaseCommandQueue(cmdQueue);
    clReleaseMemObject(bufA);
    clReleaseMemObject(bufB);
    clReleaseMemObject(bufC);
    clReleaseContext(context);

    // Free host resources
    free(A);
    free(B);
    free(C);
    free(platforms);
    free(devices);

    return 0;
}
