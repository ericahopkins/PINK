/**
 * @file   SelfOrganizingMapTest/training.cpp
 * @brief  Unit tests for image processing.
 * @date   Oct 6, 2014
 * @author Bernd Doser, HITS gGmbH
 */

#include "gtest/gtest.h"

#include "SelfOrganizingMapLib/train.h"

using namespace pink;

TEST(SelfOrganizingMapTest, 2_dim_cartesian)
{
	Cartesian<2, Cartesian<2, float>> som({3, 3});
	Cartesian<2, float> image({100, 100});

	Trainer trainer;
	trainer(som, image);
}
