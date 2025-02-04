#include <stdio.h>
#include <opencv2/opencv.hpp>
#include <fstream>
#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <sstream>			            // Hold concatenated string
#include <cstdlib>			            // atoi, system etc
#include <iomanip>			            // Set fixed width and fill them with 0s
#include "PlyFunctions.h"               // PLY read/write functions
#include "Transformation.h"             // Rotation/Translation functions

#define DEBUG 0

//IR Camera parameters as given by libfreenect2
struct camera_params_t
{
    struct ir_t{
//        float cx = 254.878f;
//        float cy = 205.395f;
//        float fx = 365.456f;
//        float fy = 365.456f;
//        float k1 = 0.0905474;
//        float k2 = -0.26819;
//        float k3 = 0.0950862;
//        float p1 = 0.0;
//        float p2 = 0.0;
//        float cx = 255.16569542;
//        float cy = 211.82460040;
//        float fx = 367.28674900;
//        float fy = 367.28675187;
//        float k1 = 0.0914235179439778;
//        float k2 = -0.269355436246;
//        float k3 = 0.092570290246685;
        float p1 = 0.0;
        float p2 = 0.0;
        // My calibration values.
        float cx = 246.827688819385173;
        float cy = 208.082873938749344;
        float fx = 365.630641419457390;
        float fy = 365.514101119250824;
        float k1 = 0.091923194251939 ;
        float k2 = -0.237043183681760;
        float k3 = 0.001359406123628;
    }ir;
}camera_params;

// Show a help message.
static void showUsage(){
    std::cerr << "usage: Depth2XYZ "
                 << " Image_Dir_Path "
                 << " Image_Name without any number or extension "
                 << " Maximum_Depth "
                 << " Start_Num "
                 << " Stop_Num "
                 << " [-rx rotation_around_X_axis] "
                 << " [-ry rotation_around_Y_axis] "
                 << " [-rz rotation_around_Z_axis] "
                 << " [-tx translation_along_X_axis] "
                 << " [-ty translation_along_Y_axis] "
                 << " [-tz translation_along_Z_axis] "
                 << std::endl;
}

// Read an image which is 16bit/pixel and create a 3D model
int main(int argc, char** argv )
{
    // Check the input arguments.
    if ( argc < 6 )
    {
        showUsage();
        return 1;
    }

    // Try to read the optional arguments.
    long rot_x(0), rot_y(0), rot_z(0), trans_x(0), trans_y(0), trans_z(0);
    int rot_axis_order[]={0, 0, 0};
    // If we want to rotate the point cloud arround any axis or translate along any axis then use
    // the following input arguments or just use identity for rotation and 0 for translation.
    for (int argI=6; argI<argc; ++argI){
        const std::string arg(argv[argI]);                  // Read the given argument.
        if (arg == "-rx"){
            ++argI;
            rot_x = std::strtol(argv[argI], NULL, 0);
        }else if (arg == "-rx"){
            ++argI;
            rot_x = std::strtol(argv[argI], NULL, 0);
        }else if (arg == "-rx"){
            ++argI;
            rot_x = std::strtol(argv[argI], NULL, 0);
        }else if (arg == "-rx"){
            ++argI;
            rot_x = std::strtol(argv[argI], NULL, 0);
        }else if (arg == "-rx"){
            ++argI;
            rot_x = std::strtol(argv[argI], NULL, 0);
        }else if (arg == "-rx"){
            ++argI;
            rot_x = std::strtol(argv[argI], NULL, 0);
        }else{
            showUsage();
            return 1;
        }
    }

    // First, create a directory to store all ply files. The newly created directory name is
    // "PlyFiles" which will be created under the given directory. IF THE FILES ALREADY EXIST, THEN
    // IT WILL OVER WRITE IT. NEEDS TO BE FIXED.
    std::ostringstream dirName;
    dirName << argv[1] <<"/PlyFiles/";
    std::ostringstream cmdName;							// Command to create a directory
    cmdName << "mkdir " << dirName.str();
    std::system(cmdName.str().c_str());

    // Now run a forloop to go through all the files starting with "start number" and ending with
    // "end number" given by the user.
    int startNum = std::atoi(argv[4]);
    int endNum = std::atoi(argv[5]);
    for (int iFN=startNum; iFN<endNum; iFN++){	// FOR EACH DEPTH IMAGE
        // Get the image name. The image name will include both the path and the file name including
        // the file extension. The file extension we are going to use here is "png".
        std::ostringstream imgFileName;
        imgFileName << argv[1] << "/" << argv[2] << "_" << std::setfill('0') << std::setw(4)\
                               << iFN << ".png";
        std::cout << "The image name is: " << imgFileName.str() << std::endl;

        // Read the image and if the format is unrecognizable or no data is available then exit
        // the prgram. As the images obtained from the libfreenect2 are filpped vertically, so flip
        // them again.
        cv::Mat imageMatFlip, imageMat;
        imageMat = cv::imread( imgFileName.str(), CV_LOAD_IMAGE_ANYDEPTH | CV_LOAD_IMAGE_ANYCOLOR);

        if ( !imageMat.data )
        {
            printf("No image data \n");
            return -1;
        }

        // Make a vertical flip.
//        cv::flip(imageMatFlip, imageMat, 0);

        int r, c;

        if (DEBUG){
            //Print the image depth before convertion
            std::cout << imageMat.depth() << std::endl;
            std::cout << imageMat.type() << std::endl;
            std::cout << imageMat.channels() << std::endl;
        }

        // Convert the 16 bit image to 32 bit to display on the screen.
        imageMat.convertTo( imageMat, CV_32FC1);
        if (DEBUG){
            cv::namedWindow("Display Image", cv::WINDOW_AUTOSIZE );
            cv::imshow("Display Image", imageMat);
            cv::waitKey(0);
            // Print the image depth after the convertion
            std::cout << imageMat.depth() << std::endl;
            std::cout << imageMat.type() << std::endl;
            std::cout << imageMat.channels() << std::endl;
        }

        // Convert the image pixels into 3D point clouds and crate a ply file in the end.
        std::vector<std::vector<float>> gridMat;
        for (r=0; r<imageMat.rows; r++){	// FOR EACH ROW ELEMENT
            for (c=0; c<imageMat.cols; c++){	// FOR EACH COLUMN ELEMENT
                float Zw = imageMat.at<float>(r,c)/(1000.0f);				//Depth
                // If the depth is beyond the required value the ignore the pixel.
                if (Zw > 0.5 && Zw < atof(argv[3])){
                    float Xw = (c-camera_params.ir.cx)*Zw/camera_params.ir.fx;
                    float Yw = (r-camera_params.ir.cy)*Zw/camera_params.ir.fy;

                    // Save the point in a matrix.
                    std::vector<float> tmpRow;
                    tmpRow.push_back(Xw);
                    tmpRow.push_back(Yw);
                    tmpRow.push_back(Zw);

                    // Store the vector
                    gridMat.push_back(tmpRow);
                }
            }	// END COLUMN-FOR
        }	// END ROW-FOR

        if (DEBUG){
            // Sanity check for the image.
            float *tmpData = reinterpret_cast<float*>(imageMat.data);
            std::cout << "Been here 1" << std::endl;
            // Display few pixels for sanity check.
            for (r=0; r<10; r++){
                for (c=0; c<14; c++){
                    std::cout <<tmpData[r*14+c] << "\t";
                    //            std::cout<< imageMat.at<float>(r,c) << "\t";
                }
                std::cout << std::endl;
            }

            // Sanity check for the evaluation of matrix values.
            for (r=0; r<2; r++){
                for (c=0; c<7; c++){
                    std::cout << gridMat[r*7+c][0] << "\t" << gridMat[r*7+c][1] << "\t" \
                                                                                << gridMat[r*7+c][2] << std::endl;
                }
            }
            std::cout << "Been here 3" << std::endl;
        }

        // Get the image name by discarding the extension. Then create a folder, which will hold
        // the ply files, inside the main folder by getting rid of the depth image file name with
        // extension.
        std::ostringstream conStr; 			// Now write the points into a ply file.
        conStr << argv[2] << "_" << std::setfill('0') << std::setw(4)<< iFN;
        std::string plyFileName;
        plyFileName = dirName.str()+"/"+conStr.str()+".ply";
        int num_pts = xyz2ply(plyFileName, gridMat);
        if (num_pts > 0){
            std::cout << "Total number of points in " << conStr.str() << " are " << num_pts << std::endl;
        }
    }	// END IMAGE-FOR
    return 0;
}
