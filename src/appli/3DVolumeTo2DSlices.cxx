// 
// This program creates a 2D slices from a 3D volume. The slices are created
// for a given direction i.e., x, y, or z. 
// A direction of the axis is traduced as:
//   x = sagittal
//   y = coronal
//   z = axial
//
// Author: Alfredo Morales Pinzon, amoralespinzon@bwh.harvard.edu
// Date: Aug. 30, 2017

// ----------------------------------------------------------------------------
// How to run this program
//
// 3DVolumeto2DSlices <input image> <axis> <output directory>
// 
// <input image> is the filepath to the input 3D volume
// <axis> is the axis used to extract the images, i.e., x, y, or z
//        For instance axis=z implies the extraction of axial images
// <output directory> is the the folderpath to the output directory. 
//
// The output images will be saved using sequetial numbers for the names:
// 0.nii.gz, 2.nii.gz, ... , MAX.nii.gz, where MAX is the number of slices in 
// the selected direction.

// ----------------------------------------------------------------------------
// Restrictions
// 
// Input images are required to be unsigned short and saved as .nii.gz 

// ----------------------------------------------------------------------------
// Libraries

//C++
#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <iostream>
#include <fstream>

// ITK
#include "itkImageFileReader.h"
#include "itkCastImageFilter.h"
#include "itkImageFileWriter.h"
#include "itkExtractImageFilter.h"

// ----------------------------------------------------------------------------
// Constants and type defitions

const std::string AXIAL = "z";
const std::string SAGITTAL = "x";
const std::string CORONAL = "y";

const unsigned int Dims = 3;
typedef unsigned short TPixel;
typedef itk::Image<TPixel,Dims> TInputImage;

// ----------------------------------------------------------------------------
// Functions' headers

template <class ImageType>
void process( std::string filpat_image, std::string axis, std::string folpat_output);

// ----------------------------------------------------------------------------
// Main function

int main(int argc, char * argv[]) 
{
	//Variables	
	std::string filpat_image; 	// File-path containing the input image
	std::string axis; 			// Axis to extract the 2D slices
	std::string folpat_output; 	// Folder-path for the outputs
	

	if(argc<3)
	{		
		std::cout << "Missing parameters, you should use:" << std::endl;
		std::cout<< argv[0] << " <input image> <axis> <output directory>" << std::endl;
		return EXIT_FAILURE;
	}
	else
	{
		filpat_image=argv[1];
		axis=argv[2];
		folpat_output=argv[3];

		//std::cout << "Input images:" << filpat_image << std::endl;
		//std::cout << "Axis:" << axis << std::endl;
		//std::cout << "Folder output:" << folpat_output << std::endl;
	}
	
	process<TInputImage>(filpat_image, axis, folpat_output);
	
	return EXIT_SUCCESS;
}

// ----------------------------------------------------------------------------
// Functions

/**
 * Method that reads the input image, extract the 2D slices, given an axis, and
 * saves the ouput images in the output folder.
 * @param filpat_image is the filename of the input image
 * @param axis is the axis used to extract the 2D slices
 * @oaram folpat_output is the output folder
 * */ 
template <class ImageType>
void process( std::string filpat_image, std::string axis, std::string folpat_output)
{
	// Macro-algorithm
	//  1. Read the input image
	//  2. For each slice in the selected axis
	//	  2.1 Extract the slice
	//    2.2 Save the slice
	// 3. Print the number of processed slices or ERROR in case of error

	// ........................................................................
	// Variables

	TInputImage::Pointer imagePointer;

	TInputImage::DirectionType direc;
	int size_x=0, size_y=0, size_z=0;

	// Reader
	typedef itk::ImageFileReader<TInputImage> ReaderType;
	ReaderType::Pointer imageReader = ReaderType::New();

	// Writer
	typedef itk::ImageFileWriter<TInputImage> Writer;
	Writer::Pointer imageWriter = Writer::New();

	// Filter to extract 2D slices from a 3D volume
	typedef itk::ExtractImageFilter< TInputImage, TInputImage > ExtractFilterType;
	 
	// ........................................................................
	//  1. Read the input image

	imageReader->SetFileName(filpat_image);
	try
	{
		imageReader->Update();
	}
	catch( itk::ExceptionObject & excp)
    {
        std::cerr << excp << std::endl;
        return;
    }

    // Get the size of the image
    TInputImage::RegionType inputRegion =
   			imageReader->GetOutput()->GetLargestPossibleRegion();
    
    TInputImage::SizeType size = inputRegion.GetSize();
    size_x=size[0];
    size_y=size[1];
    size_z=size[2];

    // Define the number of iterations
    int its = 0;
    if(axis.compare(AXIAL) == 0)
		its = size_z;
	else if(axis.compare(SAGITTAL) == 0)
		its = size_x;
	else if(axis.compare(CORONAL) == 0)
		its = size_y;

	// ........................................................................
	//  2. For each slice in the selected axis

	// Get the size and set to 1 the required direction
	TInputImage::SizeType size_slice = inputRegion.GetSize();
	if(axis.compare(AXIAL) == 0)
		size_slice[2] = 1;
	else if(axis.compare(SAGITTAL) == 0)
		size_slice[0] = 1;
	else if(axis.compare(CORONAL) == 0)
		size_slice[1] = 1;

	for(int slice=0; slice < its; ++slice)
	{
		// ....................................................................
		// 2.1 Extract the slice

    	// Get start index and set starting slice in the defined direction
    	TInputImage::IndexType start = inputRegion.GetIndex();
    	if(axis.compare(AXIAL) == 0)
			start[2] = slice;
		else if(axis.compare(SAGITTAL) == 0)
			start[0] = slice;
		else if(axis.compare(CORONAL) == 0)
			start[1] = slice;

		// Create the region that contains the selected slice
		TInputImage::RegionType actualRegion;
		actualRegion.SetIndex( start );
		actualRegion.SetSize(  size_slice  );

		// Create the extract filter
		ExtractFilterType::Pointer extractFilter = ExtractFilterType::New();
		extractFilter->SetDirectionCollapseToSubmatrix();

		// Extract the slice
		extractFilter->SetExtractionRegion( actualRegion );
		extractFilter->SetInput(imageReader->GetOutput());
		extractFilter->Update();

		// ....................................................................
		// 2.2 Save the slice
		
		imageWriter->SetInput ( extractFilter->GetOutput() );
		std::stringstream filpat_image;
		filpat_image << folpat_output << "/" << slice << ".nii.gz";
		//std::string filpat_image = folpat_output + "/" + slice + ".nii.gz";
		imageWriter->SetFileName( filpat_image.str() );
		imageWriter->Update();
	}
	std::cout << its << std::endl;
}
