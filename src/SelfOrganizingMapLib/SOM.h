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

//! Self organizing matrix
template <class SOMLayoutType = Cartesian<2>, class NeuronLayoutType = Cartesian<2>>
class SOM
{
public:

	typedef std::vector<T> NeuronType;

	//! Default and parameter constructor
    SOM(int width, int height, int depth, Layout layout, bool periodic_boundary_conditions,
        int neuron_dim, int number_of_channels, DistributionFunction distribution_function,
		float sigma, float damping, int max_update_distance,
        SOMInitialization init, int seed, std::string const& init_filename);

	//! Destructor
    ~SOM();

    void write(std::string const& filename) const;

    int getSize() const { return som.size(); }

    int getSizeInBytes() const { return som.size() * sizeof(T); }

    std::vector<float> getData() { return som; }

    const std::vector<float> getData() const { return som; }

    float* getDataPointer() { return &som[0]; }

    float const* getDataPointer() const { return &som[0]; }

    //! Main routine for SOM training.
    void train(Image<T> const& image);

    //! Main routine for SOM projection.
    //! Returns the best matching neuron and the spatial transformation (rotation, flipping)
    void project(Image<T> const& image) const;

    //! Main routine for SOM mapping.
    //! Returns a heatmap in the dimension of SOM, with the euclidean norm
    void map(Image<T> const& image) const;

    //! Print matrix of SOM updates.
    void print_update_counter() const;

private:

    std::vector<Image<T>> generate_rotated_images(Image<T> const& image) const;

    std::tuple<std::vector<T>, std::vector<int>> generate_euclidean_distance_matrix(std::vector<Image<T>> const& rotated_images) const;

    //! Returns the position in SOM of the best matching neuron (lowest euclidean distance).
    int find_best_match() const;

    //! Updating self organizing map.
    void update_neurons(int best_match, std::vector<int> const& best_rotation_matrix);

    //! Updating one single neuron.
    void update_single_neuron(float *neuron, float *image, float factor);

    //! Save position of current SOM update.
    void update_counter(int bestMatch) { ++update_counter_matrix[bestMatch]; }

    //! SOM dimensions
    int width;
    int height;
    int depth;

    Layout layout;
    bool periodic_boundary_conditions;

    //! Neuron dimensions
    int neuron_dim;
    int number_of_channels;

    DistributionFunction distribution_function;
    float sigma;
    float damping;
    int max_update_distance;

    //! The real self organizing matrix: A matrix of neurons
    std::vector<NeuronType> som;

    std::shared_ptr<DistributionFunctorBase> ptr_distribution_functor;

    std::shared_ptr<DistanceFunctorBase> ptr_distance_functor;

    //! Counting updates of each neuron
    std::vector<int> update_counter_matrix;

    //! Array for detailed timings
	std::array<std::chrono::high_resolution_clock::duration, 3> timer;
};

} // namespace pink
