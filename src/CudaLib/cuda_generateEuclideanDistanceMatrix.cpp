/**
 * @file   CudaLib/cuda_generateEuclideanDistanceMatrix.cpp
 * @date   Oct 30, 2014
 * @author Bernd Doser, HITS gGmbH
 */

#include "CudaLib.h"
#include <stdio.h>
#include <vector>

namespace pink {

/**
 * Host function that prepares data array and passes it to the CUDA kernel.
 */
void cuda_generateEuclideanDistanceMatrix(float *d_euclideanDistanceMatrix, int *d_bestRotationMatrix,
    int som_size, float* d_som, int image_size, int num_rot, float* d_rotatedImages, int block_size_1,
    bool useMultipleGPUs)
{
    float *d_firstStep = cuda_alloc_float(som_size * num_rot);
    //cuda_fill_zero(d_firstStep, som_size * num_rot);

    // First step ...
    if (useMultipleGPUs and cuda_getNumberOfGPUs() > 1) {
        cuda_generateEuclideanDistanceMatrix_firstStep_multiGPU(d_som, d_rotatedImages,
            d_firstStep, som_size, num_rot, image_size, block_size_1);
    } else {
        cuda_generateEuclideanDistanceMatrix_firstStep(d_som, d_rotatedImages,
            d_firstStep, som_size, num_rot, image_size, block_size_1);
    }

//    std::vector<float> firstStep(som_size * num_rot);
//    cuda_copyDeviceToHost_float(&firstStep[0], d_firstStep, som_size * num_rot);
//    for (int i = 0; i < som_size * num_rot; ++i)
//        printf("firstStep = %f\n", firstStep[i]);

    // Second step ...
    cuda_generateEuclideanDistanceMatrix_secondStep(d_euclideanDistanceMatrix,
        d_bestRotationMatrix, d_firstStep, som_size, num_rot);

    cuda_free(d_firstStep);
}

} // namespace pink
