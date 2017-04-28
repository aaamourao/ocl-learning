#define __CL_ENABLE_EXCEPTIONS

#ifdef __APPLE__
#include <OpenCL/cl.h>
#else
#include <CL/cl.hpp>
#endif

#include <iostream>
#include <fstream>
#include <string>

int main()
{
    // Constants
    // ------------------------------------------------------------------------
    const int N_ELEMENTS = 1024;
    const int WORK_GROUP_DIM = 1;
    // ------------------------------------------------------------------------

    int* A = new int[N_ELEMENTS];
    int* B = new int[N_ELEMENTS];
    int* C = new int[N_ELEMENTS];

    for (unsigned int i = 0; i < N_ELEMENTS; ++i) {
        A[i] = i;
        B[i] = i;
    }
    try {
        // Get available platforms
        std::vector<cl::Platform> platforms;
        cl::Platform::get(&platforms);

        // Get a list of devices on the current platform
        std::vector<cl::Device> devices;
        platforms[0].getDevices(CL_DEVICE_TYPE_GPU, &devices);

        // Create context
        cl::Context context(devices);

        // Create a command queue
        // Obs.: Using only the first device
        //       Ignoring the others, if exist
        cl::CommandQueue queue = cl::CommandQueue(context, devices[0]);

        // Create buffers
        cl::Buffer bufferA = cl::Buffer(context, CL_MEM_READ_ONLY,
                                        N_ELEMENTS*sizeof(int));
        cl::Buffer bufferB = cl::Buffer(context, CL_MEM_READ_ONLY,
                                        N_ELEMENTS*sizeof(int));
        cl::Buffer bufferC = cl::Buffer(context, CL_MEM_WRITE_ONLY,
                                        N_ELEMENTS*sizeof(int));

        // Copy input data to the input buffers
        queue.enqueueWriteBuffer(bufferA, CL_TRUE, 0,
                                 N_ELEMENTS*sizeof(int), A);
        queue.enqueueWriteBuffer(bufferB, CL_TRUE, 0,
                                 N_ELEMENTS*sizeof(int), B);

        // Get the program source
        std::ifstream sourceFile("vector_add_kernel.cl");
        std::string sourceCode(
            std::istreambuf_iterator<char>(sourceFile),
            (std::istreambuf_iterator<char>())
        );
        cl::Program::Sources source(1,
                                    std::make_pair(sourceCode.c_str(),
                                            sourceCode.length()+1));

        // Create program from the target source code
        cl::Program program = cl::Program(context, source);

        // Build
        program.build(devices);

        // Create the kernel
        cl::Kernel vecadd_kernel(program, "vecadd");

        // Link kernel arguments
        vecadd_kernel.setArg(0, bufferA);
        vecadd_kernel.setArg(1, bufferB);
        vecadd_kernel.setArg(2, bufferC);

        // Execute
        cl::NDRange global(N_ELEMENTS);
        cl::NDRange local(256);
        queue.enqueueNDRangeKernel(vecadd_kernel, cl::NullRange, global, local);

        // Copy the result to the host
        queue.enqueueReadBuffer(bufferC, CL_TRUE, 0, N_ELEMENTS*sizeof(int), C);

        // Verify the result
        bool result = true;
        for (int i = 0; i < N_ELEMENTS; ++i) {
            if (C[i] != A[i] + B[i]) {
                result = false;
                break;
            }
        }
        if (result)
            std::cout << "Success!" << std::endl;
        else
            std::cout << "Failed!" << std::endl;
    } catch(cl::Error error) {
        std::cout << error.what() << "(" <<
                  error.err() << ")" << std::endl;
    }
    return 0;
}
