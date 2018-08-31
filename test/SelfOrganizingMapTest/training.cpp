/**
 * @file   SelfOrganizingMapTest/training.cpp
 * @brief  Unit tests for image processing.
 * @date   Oct 6, 2014
 * @author Bernd Doser, HITS gGmbH
 */

#include "gtest/gtest.h"

#include "ImageProcessingLib/Image.h"
#include "SelfOrganizingMapLib/SOM.h"

using namespace pink;

TEST(SelfOrganizingMapTest, quadratic)
{
	SOM<float> som(3, 3, 1, Layout::QUADRATIC, 60, 1);
	Image<float> image(100, 100);

	som.training(image);
}
