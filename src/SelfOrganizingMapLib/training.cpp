/**
 * @file   SelfOrganizingMapLib/training.cpp
 * @date   Nov 3, 2014
 * @author Bernd Doser, HITS gGmbH
 */

#include <iostream>
#include <iomanip>

#include "ImageProcessingLib/Image.h"
#include "ImageProcessingLib/ImageIterator.h"
#include "ImageProcessingLib/ImageProcessing.h"
#include "SelfOrganizingMap.h"
#include "SOM.h"
#include "UtilitiesLib/CheckArrays.h"
#include "UtilitiesLib/Error.h"
#include "UtilitiesLib/Filler.h"
#include "UtilitiesLib/InputData.h"
#include "UtilitiesLib/TimeAccumulator.h"

namespace pink {

template <typename T>
void SOM<T>::training(Image<T> const& image)
{
    auto&& rotated_images = generate_rotated_images(image, number_of_rotations,
		inputData_.image_dim, inputData_.neuron_dim, inputData_.useFlip, inputData_.interpolation,
		inputData_.numberOfChannels);

    auto&& [euclidean_distance_matrix, best_rotation_matrix] = generate_euclidean_distance_matrix(rotated_images,
		inputData_.som_size, &som_[0], inputData_.numberOfChannels * inputData_.neuron_size,
		inputData_.numberOfRotationsAndFlip, &rotatedImages[0]);

	int best_match = findbest_matchingNeuron(&euclideanDistanceMatrix[0], inputData_.som_size);

	update_counter(best_match);
	update_neurons(rotated_images, best_match, best_rotation_matrix);
}

/// template instantiation
template class SOM<float>;

} // namespace pink
