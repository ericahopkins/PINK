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

    void generate_rotated_images(Image<T> const& image) const;

    void generate_euclidean_distance_matrix() const;

    //! Returns the position of the best matching neuron (lowest euclidean distance).
    int find_best_match(float *euclideanDistanceMatrix, int som_size) const;

    //! Updating self organizing map.
    void update_neurons(float *rotatedImages, int bestMatch, int *bestRotationMatrix);

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

    //! Counting updates of each neuron
    std::vector<int> update_counter_matrix;

    //! Array for detailed timings
	std::array<std::chrono::high_resolution_clock::duration, 3> timer;

    //!
	std::vector<T> rotated_images;
	std::vector<T> euclidean_distance_matrix;
	std::vector<int> best_rotation_matrix;
};

} // namespace pink
