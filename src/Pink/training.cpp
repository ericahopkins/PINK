/**
 * @file   SelfOrganizingMapLib/training.cpp
 * @date   Nov 3, 2014
 * @author Bernd Doser, HITS gGmbH
 */

#include <iostream>
#include <iomanip>

#include "ImageProcessingLib/Image.h"
#include "ImageProcessingLib/ImageIterator.h"
#include "ImageProcessingLib/ImageProcessing.h"
#include "SelfOrganizingMap.h"
#include "SOM.h"
#include "UtilitiesLib/CheckArrays.h"
#include "UtilitiesLib/Error.h"
#include "UtilitiesLib/Filler.h"
#include "UtilitiesLib/InputData.h"
#include "UtilitiesLib/TimeAccumulator.h"

namespace pink {

void training(InputData const& input_data)
{
    std::cout << "  Starting C version of training.\n" << std::endl;

    SOM<float> som(inputData);

    // Memory allocation
    int rotatedImagesSize = input_data.numberOfChannels * input_data.numberOfRotations * input_data.neuron_size;
    if (input_data.useFlip) rotatedImagesSize *= 2;
    if (input_data.verbose) std::cout << "  Size of rotated images = " << rotatedImagesSize * sizeof(float) << " bytes" << std::endl;
    std::vector<float> rotatedImages(rotatedImagesSize);

    if (input_data.verbose) std::cout << "  Size of euclidean distance matrix = " << input_data.som_size * sizeof(float) << " bytes" << std::endl;
    std::vector<float> euclideanDistanceMatrix(input_data.som_size);

    if (input_data.verbose) std::cout << "  Size of best rotation matrix = " << input_data.som_size * sizeof(int) << " bytes" << std::endl;
    std::vector<int> bestRotationMatrix(input_data.som_size);

    if (input_data.verbose) std::cout << "  Size of SOM = " << som.getSizeInBytes() << " bytes\n" << std::endl;

    float progress = 0.0;
    float progressStep = 1.0 / input_data.numIter / input_data.numberOfImages;
    float nextProgressPrint = input_data.progressFactor;
    int progressPrecision = rint(log10(1.0 / input_data.progressFactor)) - 2;
    if (progressPrecision < 0) progressPrecision = 0;

    // Start timer
    auto startTime = myclock::now();
    const int maxTimer = 3;
    std::chrono::high_resolution_clock::duration timer[maxTimer] = {std::chrono::high_resolution_clock::duration::zero()};

    int interStoreCount = 0;
    int updateCount = 0;

    for (int iter = 0; iter != input_data.numIter; ++iter)
    {
        for (ImageIterator<float> iterImage(input_data.imagesFilename), iterEnd; iterImage != iterEnd; ++iterImage, ++updateCount)
        {
            if ((input_data.progressFactor < 1.0 and progress > nextProgressPrint) or
                (input_data.progressFactor >= 1.0 and updateCount != 0 and !(updateCount % static_cast<int>(input_data.progressFactor))))
            {
                std::cout << "  Progress: " << std::setw(12) << updateCount << " updates, "
                     << std::fixed << std::setprecision(progressPrecision) << std::setw(3) << progress*100 << " % ("
                     << std::chrono::duration_cast<std::chrono::seconds>(myclock::now() - startTime).count() << " s)" << std::endl;
                if (input_data.verbose) {
                    std::cout << "  Time for image rotations = " << std::chrono::duration_cast<std::chrono::milliseconds>(timer[0]).count() << " ms" << std::endl;
                    std::cout << "  Time for euclidean distance = " << std::chrono::duration_cast<std::chrono::milliseconds>(timer[1]).count() << " ms" << std::endl;
                    std::cout << "  Time for SOM update = " << std::chrono::duration_cast<std::chrono::milliseconds>(timer[2]).count() << " ms" << std::endl;
                }

                if (input_data.intermediate_storage != OFF) {
                    std::string interStoreFilename = input_data.resultFilename;
                    if (input_data.intermediate_storage == KEEP) {
                        interStoreFilename.insert(interStoreFilename.find_last_of("."), "_" + std::to_string(interStoreCount));
                        ++interStoreCount;
                    }
                    if (input_data.verbose) std::cout << "  Write intermediate SOM to " << interStoreFilename << " ... " << std::flush;
                    som.write(interStoreFilename);
                    if (input_data.verbose) std::cout << "done." << std::endl;
                }

                nextProgressPrint += input_data.progressFactor;
                startTime = myclock::now();
                for (int i(0); i < maxTimer; ++i) timer[i] = std::chrono::high_resolution_clock::duration::zero();
            }
            progress += progressStep;

            {
                TimeAccumulator localTimeAccumulator(timer[0]);
                generateRotatedImages(&rotatedImages[0], iterImage->getPointerOfFirstPixel(), input_data.numberOfRotations,
                    input_data.image_dim, input_data.neuron_dim, input_data.useFlip, input_data.interpolation,
                    input_data.numberOfChannels);
            }

            {
                TimeAccumulator localTimeAccumulator(timer[1]);
                generateEuclideanDistanceMatrix(&euclideanDistanceMatrix[0], &bestRotationMatrix[0],
                    input_data.som_size, &som_[0], input_data.numberOfChannels * input_data.neuron_size,
                    input_data.numberOfRotationsAndFlip, &rotatedImages[0]);
            }

            {
                TimeAccumulator localTimeAccumulator(timer[2]);
                int bestMatch = findBestMatchingNeuron(&euclideanDistanceMatrix[0], input_data.som_size);
                updateCounter(bestMatch);
                updateNeurons(&rotatedImages[0], bestMatch, &bestRotationMatrix[0]);
            }
        }
    }

    std::cout << "  Progress: " << std::setw(12) << updateCount << " updates, 100 % ("
         << std::chrono::duration_cast<std::chrono::seconds>(myclock::now() - startTime).count() << " s)" << std::endl;
    if (input_data.verbose) {
        std::cout << "  Time for image rotations = " << std::chrono::duration_cast<std::chrono::milliseconds>(timer[0]).count() << " ms" << std::endl;
        std::cout << "  Time for euclidean distance = " << std::chrono::duration_cast<std::chrono::milliseconds>(timer[1]).count() << " ms" << std::endl;
        std::cout << "  Time for SOM update = " << std::chrono::duration_cast<std::chrono::milliseconds>(timer[2]).count() << " ms" << std::endl;
    }

    if (input_data.verbose) std::cout << "  Write final SOM to " << input_data.resultFilename << " ... " << std::flush;
    som.write(input_data.resultFilename);
    if (input_data.verbose) std::cout << "done." << std::endl;

    if (inputData.verbose) som.print_update_counter();
}

/// template instantiation
template class SOM<float>;

} // namespace pink
