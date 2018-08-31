/**
 * @file   SelfOrganizingMapLib/SOM.h
 * @brief  Self organizing Kohonen-map.
 * @date   Nov 25, 2014
 * @author Bernd Doser, HITS gGmbH
 */

#pragma once

#include <array>
#include <chrono>
#include <memory>
#include <string>
#include <vector>

#include "ImageProcessingLib/Image.h"
#include "UtilitiesLib/DistanceFunctor.h"
#include "UtilitiesLib/DistributionFunction.h"
#include "UtilitiesLib/DistributionFunctor.h"
#include "UtilitiesLib/Layout.h"
#include "UtilitiesLib/SOMInitializationType.h"

using myclock = std::chrono::steady_clock;

namespace pink {

//! Main class for self organizing matrix.
template <typename T>
class SOM
{
public:

	//! Default and parameter constructor
    SOM(int width, int height, int depth, Layout layout, bool periodic_boundary_conditions,
        int neuron_dim, int number_of_channels, DistributionFunction distribution_function,
		float sigma, float damping, int max_update_distance,
        SOMInitialization init, int seed, std::string const& init_filename);

    void write(std::string const& filename) const;

    int getSize() const { return som.size(); }

    int getSizeInBytes() const { return som.size() * sizeof(T); }

    std::vector<float> getData() { return som; }

    const std::vector<float> getData() const { return som; }

    float* getDataPointer() { return &som[0]; }

    float const* getDataPointer() const { return &som[0]; }

    //! Main routine for SOM training.
    std::array<std::chrono::high_resolution_clock::duration, 3> training(Image<T> const& image);

    //! Main routine for SOM mapping.
    void mapping(Image<T> const& image);

    //! Print matrix of SOM updates.
    void print_update_counter() const;

private:

    //! Updating self organizing map.
    void update_neurons(float *rotatedImages, int bestMatch, int *bestRotationMatrix);

    //! Save position of current SOM update.
    void update_counter(int bestMatch) { ++update_counter_matrix[bestMatch]; }

    //! Updating one single neuron.
    void update_single_neuron(float *neuron, float *image, float factor);

    //! SOM dimensions
    int width;
    int height;
    int depth;

    Layout layout;
    bool periodic_boundary_conditions;

    //! Neuron dimensions
    int neuron_dim;
    int number_of_channels;

    //! Internal value of total size for SOM tensor
    int som_size;

    //! Internal value of total size for neuron tensor
    int neuron_size;

    DistributionFunction distribution_function;
    float sigma;
    float damping;
    int max_update_distance;

    //! The real self organizing matrix.
    std::vector<float> som;

    std::shared_ptr<DistributionFunctorBase> ptr_distribution_functor;

    std::shared_ptr<DistanceFunctorBase> ptr_distance_functor;

    // Counting updates of each neuron
    std::vector<int> update_counter_matrix;

};

} // namespace pink
