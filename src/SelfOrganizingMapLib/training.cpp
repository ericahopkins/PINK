/**
 * @file   SelfOrganizingMapLib/trainSelfOrganizingMap.cpp
 * @date   Nov 3, 2014
 * @author Bernd Doser, HITS gGmbH
 */

#include "ImageProcessingLib/Image.h"
#include "ImageProcessingLib/ImageIterator.h"
#include "ImageProcessingLib/ImageProcessing.h"
#include "SelfOrganizingMap.h"
#include "SOM.h"
#include "UtilitiesLib/CheckArrays.h"
#include "UtilitiesLib/Error.h"
#include "UtilitiesLib/Filler.h"
#include "UtilitiesLib/InputData.h"
#include <chrono>
#include <iostream>
#include <iomanip>
#include <vector>

using namespace std;
using namespace PINK;
using namespace chrono;

void SOM::training()
{
    cout << "  Starting C version of training.\n" << endl;

	// Memory allocation
	int rotatedImagesSize = inputData_.numberOfChannels * inputData_.numberOfRotations * inputData_.neuron_size;
	if (inputData_.useFlip) rotatedImagesSize *= 2;
	if (inputData_.verbose) cout << "  Size of rotated images = " << rotatedImagesSize * sizeof(float) << " bytes" << endl;
	vector<float> rotatedImages(rotatedImagesSize);

	if (inputData_.verbose) cout << "  Size of euclidean distance matrix = " << inputData_.som_size * sizeof(float) << " bytes" << endl;
	vector<float> euclideanDistanceMatrix(inputData_.som_size);

	if (inputData_.verbose) cout << "  Size of best rotation matrix = " << inputData_.som_size * sizeof(int) << " bytes" << endl;
	vector<int> bestRotationMatrix(inputData_.som_size);

	if (inputData_.verbose) cout << "  Size of SOM = " << getSizeInBytes() << " bytes\n" << endl;

	float progress = 0.0;
	float progressStep = 1.0 / inputData_.numIter / inputData_.numberOfImages;
	float nextProgressPrint = inputData_.progressFactor;

	// Start timer
	auto startTime = steady_clock::now();

	for (int iter = 0; iter != inputData_.numIter; ++iter)
	{
		for (ImageIterator<float> iterImage(inputData_.imagesFilename), iterEnd; iterImage != iterEnd; ++iterImage)
		{
            if (progress > nextProgressPrint)
            {
                const auto stopTime = steady_clock::now();
                const auto duration = stopTime - startTime;

                cout << "  Progress: " << fixed << setprecision(0) << progress*100 << " % ("
                     << duration_cast<seconds>(steady_clock::now() - startTime).count() << " s)" << endl;

                if (inputData_.intermediate_storage) {
                    if (inputData_.verbose) cout << "  Write intermediate SOM to " << inputData_.resultFilename << " ... " << flush;
                    write(inputData_.resultFilename);
                    if (inputData_.verbose) cout << "done." << endl;
                }

                nextProgressPrint += inputData_.progressFactor;
                startTime = steady_clock::now();
            }
            progress += progressStep;

            generateRotatedImages(&rotatedImages[0], iterImage->getPointerOfFirstPixel(), inputData_.numberOfRotations,
                inputData_.image_dim, inputData_.neuron_dim, inputData_.useFlip, inputData_.interpolation,
                inputData_.numberOfChannels);

            generateEuclideanDistanceMatrix(&euclideanDistanceMatrix[0], &bestRotationMatrix[0],
                inputData_.som_size, &som_[0], inputData_.numberOfChannels * inputData_.neuron_size,
                inputData_.numberOfRotationsAndFlip, &rotatedImages[0]);

            int bestMatch = findBestMatchingNeuron(&euclideanDistanceMatrix[0], inputData_.som_size);
            updateCounter(bestMatch);
            updateNeurons(&rotatedImages[0], bestMatch, &bestRotationMatrix[0]);
		}
	}

	cout << "  Progress: 100 % ("
		 << duration_cast<seconds>(steady_clock::now() - startTime).count() << " s)" << endl;

	if (inputData_.verbose) cout << "  Write final SOM to " << inputData_.resultFilename << " ... " << flush;
	write(inputData_.resultFilename);
	if (inputData_.verbose) cout << "done." << endl;

    printUpdateCounter();
}
