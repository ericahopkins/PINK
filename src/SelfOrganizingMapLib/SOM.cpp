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

template <typename T>
SOM<T>::SOM(int width, int height, int depth, Layout layout, bool periodic_boundary_conditions,
    int neuron_dim, int number_of_channels, DistributionFunction distribution_function,
	float sigma, float damping, int max_update_distance,
    SOMInitialization init, int seed, std::string const& init_filename)
 : width(width),
   height(height),
   depth(depth),
   layout(layout),
   periodic_boundary_conditions(periodic_boundary_conditions),
   neuron_dim(neuron_dim),
   number_of_channels(number_of_channels),
   som_size(width * height * depth),
   neuron_size(neuron_dim * neuron_dim),
   distribution_function(distribution_function),
   sigma(sigma),
   damping(damping),
   max_update_distance(max_update_distance),
   som(som_size * neuron_size * number_of_channels),
   update_counter_matrix(som_size, 0),
   timer(std::chrono::high_resolution_clock::duration::zero())
{
    // Initialize SOM
    if (init == SOMInitialization::ZERO)
        fillWithValue(&som[0], som.size());
    else if (init == SOMInitialization::RANDOM)
        fillWithRandomNumbers(&som[0], som.size(), seed);
    else if (init == SOMInitialization::RANDOM_WITH_PREFERRED_DIRECTION) {
        fillWithRandomNumbers(&som[0], som.size(), seed);
        for (int n = 0; n < som_size; ++n)
            for (int c = 0; c < number_of_channels; ++c)
                for (int i = 0; i < neuron_dim; ++i)
                    som[(n*number_of_channels + c)*neuron_size + i*neuron_dim + i] = 1.0f;
    }
    else if (init == SOMInitialization::FILEINIT) {
        std::ifstream is(init_filename);
        if (!is) throw std::runtime_error("Error opening " + init_filename);

        int tmp;
        is.read((char*)&tmp, sizeof(int));
        if (tmp != number_of_channels) throw std::runtime_error("readSOM: wrong number_of_channels.");
        is.read((char*)&tmp, sizeof(int));
        if (tmp != width) throw std::runtime_error("readSOM: wrong width.");
        is.read((char*)&tmp, sizeof(int));
        if (tmp != height) throw std::runtime_error("readSOM: wrong height.");
        is.read((char*)&tmp, sizeof(int));
        if (tmp != depth) throw std::runtime_error("readSOM: wrong depth.");
        is.read((char*)&tmp, sizeof(int));
        if (tmp != neuron_dim) throw std::runtime_error("readSOM: wrong neuron_dim.");
        is.read((char*)&tmp, sizeof(int));
        if (tmp != neuron_dim) throw std::runtime_error("readSOM: wrong neuron_dim.");
        is.read((char*)&som[0], number_of_channels * som_size * neuron_dim
            * neuron_dim * sizeof(float));
    } else
        fatalError("Unknown init type.");

    // Set distribution function
    if (distribution_function == DistributionFunction::GAUSSIAN)
        ptr_distribution_functor = std::make_shared<GaussianFunctor>(sigma);
    else if (distribution_function == DistributionFunction::MEXICANHAT)
        ptr_distribution_functor = std::make_shared<MexicanHatFunctor>(sigma);
    else
        fatalError("Unknown distribution function.");

    int dimensionality = 1;
    if (height > 1) ++dimensionality;
    if (depth > 1) ++dimensionality;

    // Set distance function
    if (layout == Layout::QUADRATIC) {
        if (periodic_boundary_conditions) {
            if (dimensionality == 1) {
                ptr_distance_functor = std::make_shared<CartesianDistanceFunctor<1, true>>(width);
            } else if (dimensionality == 2) {
                ptr_distance_functor = std::make_shared<CartesianDistanceFunctor<2, true>>(width, height);
            } else if (dimensionality == 3) {
                ptr_distance_functor = std::make_shared<CartesianDistanceFunctor<3, true>>(width, height, depth);
            }
        } else {
            if (dimensionality == 1) {
                ptr_distance_functor = std::make_shared<CartesianDistanceFunctor<1>>(width);
            } else if (dimensionality == 2) {
                ptr_distance_functor = std::make_shared<CartesianDistanceFunctor<2>>(width, height);
            } else if (dimensionality == 3) {
                ptr_distance_functor = std::make_shared<CartesianDistanceFunctor<3>>(width, height, depth);
            }
        }
    } else if (layout == Layout::HEXAGONAL) {
        ptr_distance_functor = std::make_shared<HexagonalDistanceFunctor>(width);
    } else {
        fatalError("Unknown layout.");
    }

    // Memory allocation
    int rotated_images_size = number_of_channels * number_of_rotations * neuron_size;
    if (use_flip) rotated_images_size *= 2;

    if (verbose) {
    	std::cout << "  Size of rotated images = " << rotated_images_size * sizeof(T) << " bytes" << std::endl;
        std::cout << "  Size of euclidean distance matrix = " << som_size * sizeof(T) << " bytes" << std::endl;
        std::cout << "  Size of best rotation matrix = " << som_size * sizeof(int) << " bytes" << std::endl;
        std::cout << "  Size of SOM = " << getSizeInBytes() << " bytes\n" << std::endl;
    }

    if (cpu) {
		rotated_images.resize(rotated_images_size);
		euclidean_distance_matrix.resize(som_size);
		best_rotation_matrix.resize(som_size);
    } else {

    }
}

template <typename T>
SOM<T>::~SOM()
{
    if (verbose) {
        std::cout << "  Time for image rotations = " << std::chrono::duration_cast<std::chrono::milliseconds>(timer[0]).count() << " ms" << std::endl;
        std::cout << "  Time for euclidean distance = " << std::chrono::duration_cast<std::chrono::milliseconds>(timer[1]).count() << " ms" << std::endl;
        std::cout << "  Time for SOM update = " << std::chrono::duration_cast<std::chrono::milliseconds>(timer[2]).count() << " ms" << std::endl;
    }
}

template <typename T>
void SOM<T>::write(std::string const& filename) const
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

template <typename T>
void SOM<T>::update_neurons(float *rotatedImages, int bestMatch, int *bestRotationMatrix)
{
    float distance, factor;
    float *current_neuron = &som[0];

    for (int i = 0; i < som_size; ++i) {
        distance = (*ptr_distance_functor)(bestMatch, i);
        if (max_update_distance <= 0.0 or distance < max_update_distance) {
            factor = (*ptr_distribution_functor)(distance) * damping;
            updateSingleNeuron(current_neuron, rotatedImages + bestRotationMatrix[i]
                * number_of_channels * neuron_size, factor);
        }
        current_neuron += number_of_channels * neuron_size;
    }
}

template <typename T>
void SOM<T>::print_update_counter() const
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

template <typename T>
void SOM<T>::updateSingleNeuron(float *neuron, float *image, float factor)
{
    for (int i = 0; i < number_of_channels * neuron_size; ++i) {
        neuron[i] -= (neuron[i] - image[i]) * factor;
    }
}

/// template instantiation
template class SOM<float>;

} // namespace pink
