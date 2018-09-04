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
	generate_rotated_images(image);
	generate_euclidean_distance_matrix();

	auto&& best_match = find_best_match();

	update_counter(best_match);
	update_neurons(best_match);
}

template <typename T>
void SOM<T>::generate_rotated_images(Image<T> const& image) const
{
    TimeAccumulator localTimeAccumulator(timer[0]);

}

template <typename T>
void SOM<T>::generate_euclidean_distance_matrix() const
{
    TimeAccumulator localTimeAccumulator(timer[1]);

}

template <typename T>
int SOM<T>::find_best_match(float *euclideanDistanceMatrix, int som_size) const
{
    TimeAccumulator localTimeAccumulator(timer[2]);

}

/// template instantiation
template class SOM<float>;

} // namespace pink
