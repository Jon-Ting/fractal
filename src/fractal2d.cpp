// 2D fractal analysis code

// System includes: 
#include <iostream>
#include <fstream>
#include <string>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>

// OpenCV includes: 
#include <opencv2/core/core.hpp>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/highgui/highgui.hpp>
#include "opencv2/imgproc.hpp"

// Eigen: 
#include <Eigen/Dense>

using namespace std; 
using namespace cv; 
using namespace Eigen; 

// Initializations: 
string help_flag = "-h"; 
string img_name; 

// Function prototypes
void show_help(); 

int main(int argc, char** argv)
{
	if (argc != 2)
	{
		cout << "Check usage again." << endl; 
		return -1; 
	}
	else 
	{
		if (!strcmp(argv[1], help_flag.c_str()))
		{
			show_help(); 
			return -2; 
		}
		else
		{
			img_name = argv[1]; 
		}
	}

	string fullpath = "../images/"+img_name; 

	// Read input image into grayscale 
	//Mat img = imread(fullpath.c_str(), CV_LOAD_IMAGE_GRAYSCALE); 
	Mat img = imread(fullpath.c_str(), IMREAD_GRAYSCALE); 
	if (img.empty())
	{
		cout << "Error reading image." << endl; 
		return -3; 
	}
	namedWindow("Input image", WINDOW_AUTOSIZE); 
	imshow("Input image", img);
	printf("\nShown in the display window is the input image. Click the image and press spacebar to continue. Press Ctrl+C to exit.\n\n");  
	waitKey(0); 

	// Need to remap the image to 512x512 so box counting can be used
	Mat img_bc = Mat::zeros(512, 512, CV_8UC1); 
	resize(img, img_bc, img_bc.size(), 0, 0, INTER_LINEAR); 

	// Create binary image that will serve as input to box counting algorithm
	Mat img_bin; 
	double thresh = 250.0; 
	double maxval = 255.0; 
	threshold(img_bc, img_bin, thresh, maxval, THRESH_BINARY_INV); 

	// Implement box counting here: 
	double width = 512.0; 
	double p = log(width)/log(double(2.0)); 

	// Init N: 
	VectorXf N = VectorXf::Zero(int(p)+1); 

	double sumImg = cv::sum(img_bin)[0]; 

	N(int(p)) = sumImg; 

	// Init other stuff: 
	double siz; 
	double siz2; 
	float running_sum; 

    for (int g = int(p)-1; g > 0; g--)
    {
    	siz = pow(2.0, double(p-g)); 
    	siz2 = round(siz/2.0); 

    	running_sum = 0; 

    	for (int i = 0; i < int(width-siz+1); i = i+int(siz))
    	{
    		for (int j = 0; j < int(width-siz+1); j = j+int(siz))
    		{
    			img_bin.at<uchar>(i,j) = (bool(img_bin.at<uchar>(i,j)) || bool(img_bin.at<uchar>(i+siz2,j)) 
    				|| bool(img_bin.at<uchar>(i,j+siz2)) || bool(img_bin.at<uchar>(i+siz2,j+siz2))); 
    			running_sum = running_sum+float(img_bin.at<uchar>(i,j)); 
    		}	
    	}
    	
    	N(g) = running_sum; 

    }

    // Flip N
    N = N.colwise().reverse().eval(); 

    VectorXf R = VectorXf::Zero(int(p)+1); 
    R(0) = 1.0; 
    for (int k = 1; k < R.size(); k++)
    {
    	R(k) = pow(2.0, double(k)); 
    }	

    FILE* fp; 
    int position = int(img_name.rfind('.')); 
    string newName = img_name.substr(0, position); 
    string results_file = newName+"_output.txt"; 
    fp = fopen(results_file.c_str(), "w"); 

    for (int k = 0; k < R.size(); k++)
    {
    	fprintf(fp, "%f %f\n", R(k), N(k)); 
    }

    fclose(fp); 

    printf("\n"); 
    printf("Analysis complete. Saved results to %s.\n", results_file.c_str()); 
    printf("Use either plot_fractal.py or plot_fractal.m to post-process and generate a plot.\n"); 
    printf("\n"); 

    printf("\n"); 
    printf("Python:\n"); 
    printf("python plot_fractal.py %s %s\n", results_file.c_str(), img_name.c_str()); 
    printf("\n"); 

    printf("\n"); 
    printf("MATLAB (in the MATLAB command window in this directory):\n"); 
    printf("plot_fractal %s %s\n", results_file.c_str(), img_name.c_str()); 
    printf("\n"); 

    string user_ans; 

    printf("Press y or n for 'yes' or 'no' to answer the following question.\n"); 
    cout << "Would you like to post-process the results now in Python? (Y/n) "; 
    getline(cin, user_ans); 

    if (!strcmp(user_ans.c_str(), "y"))
    {
    	string python_command = "python plot_fractal.py "+results_file+" "+img_name; 
    	printf("Post-processing in Python.\n"); 
    	system(python_command.c_str()); 
    }
    else
    {
    	printf("Exiting program.\n"); 
    }


	return 0; 
}

// Help menu function
void show_help()
{
	printf("\n"); 
	printf("Welcome to the help menu for fractal2d.\n"); 
	printf("\n"); 
	printf("Usage:\n"); 
	printf("1. Run fractal2d on a specified image.\n");
	printf("   ./fractal2d image.png\n"); 
	printf("\n"); 
	printf("2. Display this help menu again.\n"); 
	printf("   ./fractal2d -h\n"); 
	printf("\n"); 
	printf("Below is a list of image files in the image folder that are available for analysis.\n"); 
	system("ls ../images"); 
	printf("\n"); 
	return; 
} 
