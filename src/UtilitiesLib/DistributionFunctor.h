/**
 * @file   UtilitiesLib/DistributionFunctors.h
 * @brief  Virtual functors for distribution functions used by updating SOM.
 * @date   Dec 4, 2014
 * @author Bernd Doser, HITS gGmbH
 */

#pragma once

#include <cmath>
#include <stdexcept>

namespace pink {

/**
 * @brief Abstract base for distribution functor.
 */
struct DistributionFunctorBase
{
    virtual float operator () (float distance) const = 0;
    virtual ~DistributionFunctorBase() {}
};

/**
 * @brief Gaussian functor
 *
 * 1.0 / (sigma * math.sqrt(2.0 * math.pi)) * math.exp(-1.0/2.0 * (x / sigma)**2 )
 */
struct GaussianFunctor : public DistributionFunctorBase
{
    GaussianFunctor(float sigma) : sigma(sigma) {}

    float operator () (float distance) const
    {
    	static float f1 = 1.0 / (sigma * sqrt(2.0 * M_PI));
        return f1 * exp(-0.5 * pow((distance/sigma),2));
    }

private:

    float sigma;

};

/**
 * @brief Mexican hat functor
 *
 * 2.0 / ( math.sqrt(3.0 * sigma) * math.pow(math.pi, 0.25)) * (1- x**2.0 / sigma**2.0) * math.exp(-x**2.0/(2.0 * sigma**2))
 */
struct MexicanHatFunctor : public DistributionFunctorBase
{
    MexicanHatFunctor(float sigma)
     : sigma(sigma)
    {
        assert(sigma > 0.0);
    }

    float operator () (float distance) const
    {
    	static float sigma2 = sigma * sigma;
    	static float f1 = 2.0 / (sqrt(3.0 * sigma) * pow(M_PI, 0.25));
        float distance2 = distance * distance;
        return f1 * (1.0 - distance2/sigma2) * exp(-distance2 / (2.0 * sigma2));
    }

private:

    float sigma;

};

} // namespace pink
