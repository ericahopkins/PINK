/**
 * @file   ImageProcessingLib/ImageProcessing.cpp
 * @brief  Plain-C functions for image processing.
 * @date   Oct 7, 2014
 * @author Bernd Doser, HITS gGmbH
 */

#include "ImageProcessing.h"
#include "UtilitiesLib/Error.h"
#include <fstream>
#include <iostream>
#include <iomanip>
#include <math.h>
#include <omp.h>
#include <stdlib.h>
#include <stdexcept>
#include <vector>

#if PINK_USE_PYTHON
    #include "Python.h"
#endif

using namespace std;

std::ostream& operator << (std::ostream& os, Interpolation interpolation)
{
	if (interpolation == NEAREST_NEIGHBOR) os << "nearest_neighbor";
	else if (interpolation == BILINEAR) os << "bilinear";
	else os << "undefined";
	return os;
}

void rotate_nearest_neighbor(int height, int width, float *source, float *dest, float alpha)
{
    const float cosAlpha = cos(alpha);
    const float sinAlpha = sin(alpha);

    float x0 = (width-1) * 0.5;
    float y0 = (height-1) * 0.5;
    float x1, y1;

    for (int x2 = 0; x2 < width; ++x2) {
        for (int y2 = 0; y2 < height; ++y2) {

        	x1 = ((float)x2 - x0) * cosAlpha + ((float)y2 - y0) * sinAlpha + x0 + 0.1;
            if (x1 < 0 or x1 >= width) {
            	dest[x2*height + y2] = 0.0;
            	continue;
            }
        	y1 = ((float)y2 - y0) * cosAlpha - ((float)x2 - x0) * sinAlpha + y0 + 0.1;
            if (y1 < 0 or y1 >= height) {
            	dest[x2*height + y2] = 0.0;
            	continue;
            }
            dest[x2*height + y2] = source[(int)x1*height + (int)y1];
        }
    }
}

void rotate_bilinear(int height, int width, float *source, float *dest, float alpha)
{
    const float cosAlpha = cos(alpha);
    const float sinAlpha = sin(alpha);

    float x0 = (width-1) * 0.5;
    float y0 = (height-1) * 0.5;
    float x1, y1, rx1, ry1, cx1, cy1;
    int ix1, iy1, ix1b, iy1b;

    for (int x2 = 0; x2 < width; ++x2) {
        for (int y2 = 0; y2 < height; ++y2) {

        	x1 = ((float)x2 - x0) * cosAlpha + ((float)y2 - y0) * sinAlpha + x0;
//            if (x1 < 0 or x1 >= width) {
//            	dest[x2*height + y2] = 0.0;
//            	continue;
//            }
        	y1 = ((float)y2 - y0) * cosAlpha - ((float)x2 - x0) * sinAlpha + y0;
//            if (y1 < 0 or y1 >= height) {
//            	dest[x2*height + y2] = 0.0;
//            	continue;
//            }
            ix1 = x1;
            iy1 = y1;
            ix1b = ix1 + 1;
            iy1b = iy1 + 1;
            rx1 = x1 - ix1;
            ry1 = y1 - iy1;
            cx1 = 1.0f - rx1;
            cy1 = 1.0f - ry1;
            dest[x2*height + y2] = cx1 * cy1 * source[ix1  * height + iy1 ]
                                 + cx1 * ry1 * source[ix1  * height + iy1b]
                                 + rx1 * cy1 * source[ix1b * height + iy1 ]
                                 + rx1 * ry1 * source[ix1b * height + iy1b];
        }
    }
}

void rotate_90degrees(int height, int width, float *source, float *dest)
{
    for (int x = 0; x < width; ++x) {
        for (int y = 0; y < height; ++y) {
            dest[(height-y-1)*width + x] = source[x*height + y];
        }
    }
}

void rotate(int height, int width, float *source, float *dest, float alpha, Interpolation interpolation)
{
	if (interpolation == NEAREST_NEIGHBOR)
		rotate_nearest_neighbor(height, width, source, dest, alpha);
	else if (interpolation == BILINEAR)
		rotate_bilinear(height, width, source, dest, alpha);
	else {
		fatalError("rotateAndCrop: unknown interpolation\n");
	}
}

void flip(int height, int width, float *source, float *dest)
{
	float *pdest = dest + (height-1) * width;
	float *psource = source;

	for (int i = 0; i < height; ++i) {
		for (int j = 0; j < width; ++j) {
		    pdest[j] = psource[j];
		}
		pdest -= width;
		psource += width;
	}
}

void crop(int height, int width, int height_new, int width_new, float * source, float *dest)
{
	int width_margin = (width - width_new) / 2;
	int height_margin = (height - height_new) / 2;

	for (int i = 0; i < height_new; ++i) {
		for (int j = 0; j < width_new; ++j) {
		    dest[i*width_new+j] = source[(i+height_margin)*width + (j+width_margin)];
		}
	}
}

void flipAndCrop(int height, int width, int height_new, int width_new, float *source, float *dest)
{
	int width_margin = (width - width_new) / 2;
	int height_margin = (height - height_new) / 2;

	for (int i = 0, ri = height_new-1; i < height_new; ++i, --ri) {
		for (int j = 0; j < width_new; ++j) {
		    dest[ri*width_new + j] = source[(i+height_margin)*width + (j+width_margin)];
		}
	}
}

void rotateAndCrop_nearest_neighbor(int height, int width, int height_new, int width_new, float *source, float *dest, float alpha)
{
	const int width_margin = (width - width_new) * 0.5;
	const int height_margin = (height - height_new) * 0.5;

    const float cosAlpha = cos(alpha);
    const float sinAlpha = sin(alpha);

    const float x0 = (width-1) * 0.5;
    const float y0 = (height-1) * 0.5;
    float x1, y1;

    for (int x2 = 0; x2 < width_new; ++x2) {
        for (int y2 = 0; y2 < height_new; ++y2) {
        	x1 = ((float)x2 + width_margin - x0) * cosAlpha + ((float)y2 + height_margin - y0) * sinAlpha + x0 + 0.1;
            if (x1 < 0 or x1 >= width) {
            	dest[x2*height_new + y2] = 0.0f;
            	continue;
            }
        	y1 = ((float)y2 + height_margin - y0) * cosAlpha - ((float)x2 + width_margin - x0) * sinAlpha + y0 + 0.1;
            if (y1 < 0 or y1 >= height) {
            	dest[x2*height_new + y2] = 0.0f;
            	continue;
            }
            dest[x2*height_new + y2] = source[(int)x1*height + (int)y1];
        }
    }
}

void rotateAndCrop_bilinear(int height, int width, int height_new, int width_new, float *source, float *dest, float alpha)
{
	const int width_margin = (width - width_new) * 0.5;
	const int height_margin = (height - height_new) * 0.5;

    const float cosAlpha = cos(alpha);
    const float sinAlpha = sin(alpha);

    const float x0 = (width-1) * 0.5;
    const float y0 = (height-1) * 0.5;
    float x1, y1, rx1, ry1, cx1, cy1;
    int ix1, iy1, ix1b, iy1b;

    for (int x2 = 0; x2 < width_new; ++x2) {
        for (int y2 = 0; y2 < height_new; ++y2) {
        	x1 = ((float)x2 + width_margin - x0) * cosAlpha + ((float)y2 + height_margin - y0) * sinAlpha + x0;
//            if (x1 < 0 or x1 >= width) {
//            	dest[x2*height_new + y2] = 0.0f;
//            	continue;
//            }
        	y1 = ((float)y2 + height_margin - y0) * cosAlpha - ((float)x2 + width_margin - x0) * sinAlpha + y0;
//            if (y1 < 0 or y1 >= height) {
//            	dest[x2*height_new + y2] = 0.0f;
//            	continue;
//            }
            ix1 = x1;
            iy1 = y1;
            ix1b = ix1 + 1;
            iy1b = iy1 + 1;
            rx1 = x1 - ix1;
            ry1 = y1 - iy1;
            cx1 = 1.0f - rx1;
            cy1 = 1.0f - ry1;
            dest[x2*height_new + y2] = cx1 * cy1 * source[ix1  * height + iy1 ]
                                     + cx1 * ry1 * source[ix1  * height + iy1b]
                                     + rx1 * cy1 * source[ix1b * height + iy1 ]
                                     + rx1 * ry1 * source[ix1b * height + iy1b];
        }
    }
}

void rotateAndCrop(int height, int width, int height_new, int width_new, float *source, float *dest, float alpha, Interpolation interpolation)
{
	if (interpolation == NEAREST_NEIGHBOR)
		rotateAndCrop_nearest_neighbor(height, width, height_new, width_new, source, dest, alpha);
	else if (interpolation == BILINEAR)
		rotateAndCrop_bilinear(height, width, height_new, width_new, source, dest, alpha);
	else {
		fatalError("rotateAndCrop: unknown interpolation");
	}
}

float calculateEuclideanDistance(float *a, float *b, int length)
{
    return sqrt(calculateEuclideanDistanceWithoutSquareRoot(a,b,length));
}

float calculateEuclideanDistanceWithoutSquareRoot(float *a, float *b, int length)
{
	float *pa = a;
	float *pb = b;
	float c = 0.0;
	float tmp;
    for (int i = 0; i < length; ++i, ++pa, ++pb) {
    	tmp = *pa - *pb;
        c += tmp * tmp;
    }
    return c;
}

void normalize(float *a, int length)
{
	float maxValue;
    for (int i = 0; i < length; ++i) {
        maxValue = fmax(maxValue, a[i]);
    }

    float maxValueInv;
    for (int i = 0; i < length; ++i) {
        a[i] *= maxValueInv;
    }
}

float mean(float *a, int length)
{
	int i;
	float sum = 0.0;
    for (i = 0; i < length; ++i) {
        sum += a[0];
    }
    return sum / length;
}

float stdDeviation(float *a, int length)
{
	int i;
	float sum = 0.0;
	float meanValue = mean(a,length);

    for (i = 0; i < length; ++i) {
    	sum += pow((a[i], meanValue),2);
    }

	return sqrt(sum/length);
}

void zeroValuesSmallerThanStdDeviation(float *a, int length, float safety)
{
	int i;
	float threshold = stdDeviation(a,length) * safety;

    for (i = 0; i < length; ++i) {
    	if (a[i] < threshold) a[i] = 0.0;
    }
}

void printImage(float *image, int height, int width)
{
    for (int j = 0; j < height; ++j) {
        for (int i = 0; i < width; ++i) {
            cout << setw(10) << fixed << setprecision(3) << image[i*height+j] << " ";
    	}
        cout << endl;
    }
    cout << endl;
}

void writeImagesToBinaryFile(std::vector<float> const& images, int numberOfImages, int numberOfChannels,
    int height, int width, std::string const& filename)
{
    std::ofstream os(filename);
    if (!os) throw std::runtime_error("Error opening " + filename);

    os.write((char*)&numberOfImages, sizeof(int));
    os.write((char*)&numberOfChannels, sizeof(int));
    os.write((char*)&height, sizeof(int));
    os.write((char*)&width, sizeof(int));
    os.write((char*)&images[0], numberOfImages * numberOfChannels * height * width * sizeof(float));
}

void readImagesFromBinaryFile(std::vector<float> &images, int &numberOfImages, int &numberOfChannels,
    int &height, int &width, std::string const& filename)
{
    std::ifstream is(filename);
    if (!is) throw std::runtime_error("Error opening " + filename);

    is.read((char*)&numberOfImages, sizeof(int));
    is.read((char*)&numberOfChannels, sizeof(int));
    is.read((char*)&height, sizeof(int));
    is.read((char*)&width, sizeof(int));

    int size = numberOfImages * numberOfChannels * height * width;
    images.resize(size);
    is.read((char*)&images[0], size * sizeof(float));
}

void showImage(float *image, int height, int width)
{
    #if PINK_USE_PYTHON
		std::string filename("ImageTmp.bin");
		writeImageToBinaryFile(image, height, width, filename);

		Py_Initialize();
		PyRun_SimpleString("import numpy");
		PyRun_SimpleString("import matplotlib.pylab as plt");
		PyRun_SimpleString("import struct");

		std::string line = "inFile = open(\"" + filename + "\", 'rb')";
		PyRun_SimpleString(line.c_str());
		PyRun_SimpleString("size = struct.unpack('iii', inFile.read(12))");
		PyRun_SimpleString("array = numpy.array(struct.unpack('f'*size[1]*size[2], inFile.read(size[1]*size[2]*4)))");
		PyRun_SimpleString("data = numpy.ndarray([size[1],size[2]], 'float', array)");
		PyRun_SimpleString("inFile.close()");
		PyRun_SimpleString("fig = plt.figure()");
		PyRun_SimpleString("ax = fig.add_subplot(1,1,1)");
		PyRun_SimpleString("ax.set_aspect('equal')");
		PyRun_SimpleString("plt.imshow(data, interpolation='nearest', cmap=plt.cm.ocean)");
		PyRun_SimpleString("plt.colorbar()");
		PyRun_SimpleString("plt.show()");
    #else
		std::cout << "=== WARNING === Pink must be compiled with python support to show images." << std::endl;
    #endif
}

void convertSOMToImage(float* image, const float* som, int som_dim, int image_dim)
{
	int total_image_dim = som_dim * image_dim;
    float *pimage = image;
    const float *psom = som;

    for (int i = 0; i < som_dim; ++i) {
        for (int j = 0; j < som_dim; ++j) {
            for (int k = 0; k < image_dim; ++k) {
                for (int l = 0; l < image_dim; ++l) {
        	        pimage[i*image_dim*som_dim*image_dim + k*image_dim*som_dim + j*image_dim + l] = *psom++;
            	}
            }
    	}
    }
}

void convertImageToSOM(float* image, const float* som, int som_dim, int image_dim)
{
    int total_image_dim = som_dim * image_dim;
    float *pimage = image;
    const float *psom = som;

    for (int i = 0; i < som_dim; ++i) {
        for (int j = 0; j < som_dim; ++j) {
            for (int k = 0; k < image_dim; ++k) {
                for (int l = 0; l < image_dim; ++l) {
                    pimage[i*image_dim*som_dim*image_dim + k*image_dim*som_dim + j*image_dim + l] = *psom++;
                }
            }
        }
    }
}

void writeSOM(const float* som, int numberOfChannels, int som_dim, int neuron_dim, std::string const& filename)
{
    std::ofstream os(filename);
    if (!os) throw std::runtime_error("Error opening " + filename);

    os.write((char*)&numberOfChannels, sizeof(int));
    os.write((char*)&som_dim, sizeof(int));
    os.write((char*)&neuron_dim, sizeof(int));
    os.write((char*)som, numberOfChannels * som_dim * som_dim * neuron_dim * neuron_dim * sizeof(float));
}

void readSOM(float *som, int &numberOfChannels, int &som_dim, int &neuron_dim, std::string const& filename)
{
    std::ifstream is(filename);
    if (!is) throw std::runtime_error("Error opening " + filename);

    is.read((char*)&numberOfChannels, sizeof(int));
    is.read((char*)&som_dim, sizeof(int));
    is.read((char*)&neuron_dim, sizeof(int));
    is.read((char*)som, numberOfChannels * som_dim * som_dim * neuron_dim * neuron_dim * sizeof(float));
}

void writeRotatedImages(float* images, int image_dim, int numberOfImages, std::string const& filename)
{
	int heigth = numberOfImages * image_dim;
	int width = image_dim;
	int image_size = image_dim * image_dim;
    vector<float> image(heigth * width);

    for (int i = 0; i < numberOfImages; ++i) {
        for (int j = 0; j < image_size; ++j) image[j + i*image_size] = images[j + i*image_size];
    }

    writeImagesToBinaryFile(image, 1, 1, heigth, width, filename);
}

void showRotatedImages(float* images, int image_dim, int numberOfRotations)
{
	int heigth = 2 * numberOfRotations * image_dim;
	int width = image_dim;
	int image_size = image_dim * image_dim;
    float *image = (float *)malloc(heigth * width * sizeof(float));

    for (int i = 0; i < 2 * numberOfRotations; ++i) {
        for (int j = 0; j < image_size; ++j) image[j + i*image_size] = images[j + i*image_size];
    }

    showImage(image, heigth, width);
    free(image);
}

void showRotatedImagesSingle(float* images, int image_dim, int numberOfRotations)
{
	int image_size = image_dim * image_dim;
    for (int i = 0; i < 2 * numberOfRotations; ++i) {
        showImage(images + i*image_size, image_dim, image_dim);
    }
}
