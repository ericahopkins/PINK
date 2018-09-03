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
    {
        TimeAccumulator localTimeAccumulator(timer[0]);
		auto&& rotated_images = generate_rotated_images(image, number_of_rotations,
			inputData_.image_dim, inputData_.neuron_dim, inputData_.useFlip, inputData_.interpolation,
			inputData_.numberOfChannels);
    }
    {
        TimeAccumulator localTimeAccumulator(timer[1]);
		auto&& [euclidean_distance_matrix, best_rotation_matrix] = generate_euclidean_distance_matrix(rotated_images,
			inputData_.som_size, &som_[0], inputData_.numberOfChannels * inputData_.neuron_size,
			inputData_.numberOfRotationsAndFlip, &rotatedImages[0]);
    }
    {
        TimeAccumulator localTimeAccumulator(timer[2]);
		auto&& best_match = find_best_matching_neuron(euclidean_distance_matrix, inputData_.som_size);

		update_counter(best_match);
		update_neurons(rotated_images, best_match, best_rotation_matrix);
    }
}

template <typename T>
void SOM<T>::generate_rotated_images(Image<T> const& image) const
{

}

template <typename T>
void SOM<T>::generate_euclidean_distance_matrix() const
{

}

template <typename T>
int SOM<T>::find_best_match(float *euclideanDistanceMatrix, int som_size) const
{

}

/// template instantiation
template class SOM<float>;

} // namespace pink
