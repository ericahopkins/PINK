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

//! Self organizing map
template <typename SOMType>
class SOM
{
public:

    typedef typename SOMType::value_type NeuronType;
    typedef typename NeuronType::value_type value_type;

	//! Default and parameter constructor
    SOM(SOMType const& som_type);

	//! Destructor
    ~SOM();

    void write(std::string const& filename) const;

    //! Main routine for SOM training.
    template <typename NeuronType, typename DistributionFunctionType>
    void train(Image<NeuronType> const& image, DistributionFunctionType const& distribution_functor);

    //! Main routine for SOM projection.
    //! Returns the best matching neuron and the spatial transformation (rotation, flipping)
    // TODO: void project(Image<T> const& image) const;

    //! Main routine for SOM mapping.
    //! Returns a heatmap in the dimension of SOM, with the euclidean norm
    // TODO: void map(Image<T> const& image) const;

    //! Print matrix of SOM updates.
    void print_update_counter() const;

private:

    template <typename NeuronType, typename DistributionFunctionType>
    std::vector<Image<NeuronType>> generate_rotated_images(Image<T> const& image) const;

    // TODO: std::tuple<std::vector<T>, std::vector<int>> generate_euclidean_distance_matrix(std::vector<Image<T>> const& rotated_images) const;

    //! Returns the position in SOM of the best matching neuron (lowest euclidean distance).
    int find_best_match() const;

    //! Updating self organizing map.
    void update_neurons(int best_match, std::vector<int> const& best_rotation_matrix);

    //! Save position of current SOM update.
    void update_counter(int bestMatch) { ++update_counter_matrix[bestMatch]; }

    //! The real self organizing map
    SOMType som;

    //! Counting updates of each neuron
    std::vector<int> update_counter_matrix;

    //! Array for detailed timings
	std::array<std::chrono::high_resolution_clock::duration, 3> timer;
};

} // namespace pink
