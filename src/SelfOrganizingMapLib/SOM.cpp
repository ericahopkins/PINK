/**
 * @file   SelfOrganizingMapLib/SOM.cpp
 * @brief  Self organizing Kohonen-map.
 * @date   Nov 25, 2014
 * @author Bernd Doser, HITS gGmbH
 */

#include <cmath>
#include <fstream>
#include <iostream>
#include <iomanip>

#include "SOM.h"
#include "UtilitiesLib/Error.h"
#include "UtilitiesLib/Filler.h"

namespace pink {

template <typename SOMType>
SOM<SOMType>::SOM(SOMType const& som_type)
 : som()
{}

template <typename SOMType>
SOM<SOMType>::~SOM()
{
    if (verbose) {
        std::cout << "  Time for image rotations = " << std::chrono::duration_cast<std::chrono::milliseconds>(timer[0]).count() << " ms" << std::endl;
        std::cout << "  Time for euclidean distance = " << std::chrono::duration_cast<std::chrono::milliseconds>(timer[1]).count() << " ms" << std::endl;
        std::cout << "  Time for SOM update = " << std::chrono::duration_cast<std::chrono::milliseconds>(timer[2]).count() << " ms" << std::endl;
    }
}

template <typename SOMType>
void SOM<SOMType>::write(std::string const& filename) const
{
    std::ofstream os(filename);
    if (!os) throw std::runtime_error("Error opening " + filename);

    os.write((char*)&number_of_channels, sizeof(int));
    os.write((char*)&width, sizeof(int));
    os.write((char*)&height, sizeof(int));
    os.write((char*)&depth, sizeof(int));
    os.write((char*)&neuron_dim, sizeof(int));
    os.write((char*)&neuron_dim, sizeof(int));
    os.write((char*)&som[0], number_of_channels * som_size
        * neuron_dim * neuron_dim * sizeof(float));
}

template <typename SOMType>
template <typename DistributionFunctionType>
void SOM<SOMType>::train(Image<T> const& image, DistributionFunctionType const& distribution_functor)
{
	auto&& rotated_images = generate_rotated_images(image);
	generate_euclidean_distance_matrix();

	auto&& best_match = find_best_match();

	update_counter(best_match);
	update_neurons(best_match);
}

template <typename SOMType>
std::vector<Image<T>> SOM<SOMType>::generate_rotated_images(Image<T> const& image) const
{
    TimeAccumulator localTimeAccumulator(timer[0]);

}

template <typename SOMType>
void SOM<SOMType>::generate_euclidean_distance_matrix() const
{
    TimeAccumulator localTimeAccumulator(timer[1]);

}

template <typename SOMType>
int SOM<SOMType>::find_best_match() const
{
    TimeAccumulator localTimeAccumulator(timer[2]);

    int best_match = 0;
    float min_euclidean_distance = euclideanDistanceMatrix[0];
    for (size_t i = 1; i < som_size; ++i) {
        if (euclideanDistanceMatrix[i] < minDistance) {
            minDistance = euclideanDistanceMatrix[i];
            best_match = i;
        }
    }
    return best_match;
}

template <typename SOMType>
void SOM<SOMType>::update_neurons(int best_match, std::vector<int> const& best_rotation_matrix)
{
    float distance, factor;
    float *current_neuron = &som[0];

    for (int i = 0; i < som_size; ++i) {
        distance = (*ptr_distance_functor)(best_match, i);
        if (max_update_distance <= 0.0 or distance < max_update_distance) {
            factor = (*ptr_distribution_functor)(distance) * damping;
            update_single_neuron(current_neuron, rotatedImages + best_rotation_matrix[i]
                * number_of_channels * neuron_size, factor);
        }
        current_neuron += number_of_channels * neuron_size;
    }
}

template <typename SOMType>
void SOM<SOMType>::print_update_counter() const
{
	std::cout << "\n  Number of updates of each neuron:\n" << std::endl;
	if (layout == Layout::HEXAGONAL) {
		int radius = (width - 1)/2;
		for (int pos = 0, x = -radius; x <= radius; ++x) {
			for (int y = -radius - std::min(0,x); y <= radius - std::max(0,x); ++y, ++pos) {
				std::cout << std::setw(6) << update_counter_matrix[pos] << " ";
			}
			std::cout << std::endl;
		}
	} else {
		for (int pos = 0, d = 0; d != depth; ++d) {
			for (int h = 0; h != height; ++h) {
				for (int w = 0; w != width; ++w, ++pos) {
					std::cout << std::setw(6) << update_counter_matrix[pos] << " ";
				}
				std::cout << std::endl;
			}
			std::cout << std::endl;
		}
	}
}

/// template instantiation
template class SOM<Cartesian<2, Cartesian<2, float>>>;

} // namespace pink
