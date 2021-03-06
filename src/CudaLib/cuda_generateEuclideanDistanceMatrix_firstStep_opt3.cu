/**
 * @file   CudaLib/cuda_generateEuclideanDistanceMatrix_firstStep_opt3.cu
 * @date   Oct 30, 2014
 * @author Bernd Doser, HITS gGmbH
 */

#include "CudaLib.h"
#include "cuda_generateEuclideanDistanceMatrix_firstStep_opt3.cu.h"
#include <stdio.h>

namespace pink {

template
void cuda_generateEuclideanDistanceMatrix_firstStep_opt3<512>(float *d_som, float *d_rotatedImages,
   int som_size, int num_rot, int image_size);

template
void cuda_generateEuclideanDistanceMatrix_firstStep_opt3<256>(float *d_som, float *d_rotatedImages,
   int som_size, int num_rot, int image_size);

template
void cuda_generateEuclideanDistanceMatrix_firstStep_opt3<128>(float *d_som, float *d_rotatedImages,
   int som_size, int num_rot, int image_size);

template
void cuda_generateEuclideanDistanceMatrix_firstStep_opt3<64>(float *d_som, float *d_rotatedImages,
   int som_size, int num_rot, int image_size);

template
void cuda_generateEuclideanDistanceMatrix_firstStep_opt3<32>(float *d_som, float *d_rotatedImages,
   int som_size, int num_rot, int image_size);

} // namespace pink
