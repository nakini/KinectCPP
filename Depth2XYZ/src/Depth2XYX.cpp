#include <stdio.h>
#include <opencv2/opencv.hpp>
#include <fstream>
#include <iostream>

//IR Camera parameters as given by libfreenect2
struct camera_params_t
{
    struct ir_t{
        float cx = 254.878f;
        float cy = 205.395f;
        float fx = 365.456f;
        float fy = 365.456f;
        float k1 = 0.0905474;
        float k2 = -0.26819;
        float k3 = 0.0950862;
        float p1 = 0.0;
        float p2 = 0.0;
    }ir;
}camera_params;

// Read an image which is 16bit/pixel and create a 3D model
int main(int argc, char** argv )
{
    // Check the input arguments. The user should provide the image path and the maximum distance.
    if ( argc != 3 )
    {
        printf("usage: Depth2XYZ <Image_Path> <Maximum_Depth>\n");
        return -1;
    }

    // Read the image and if the format is unrecognizable or no data is available then exit the prgram.
    cv::Mat imageMat;
    imageMat = cv::imread( argv[1], CV_LOAD_IMAGE_ANYDEPTH | CV_LOAD_IMAGE_ANYCOLOR);

    if ( !imageMat.data )
    {
        printf("No image data \n");
        return -1;
    }

    int r, c;

//    //Print the image depth before convertion
//    std::cout << imageMat.depth() << std::endl;
//    std::cout << imageMat.type() << std::endl;
//    std::cout << imageMat.channels() << std::endl;

    // Convert the 16 bit image to 32 bit to display on the screen.
    imageMat.convertTo( imageMat, CV_32FC1);
    cv::namedWindow("Display Image", cv::WINDOW_AUTOSIZE );
    cv::imshow("Display Image", imageMat);
    cv::waitKey(0);

//    // Print the image depth after the convertion
//    std::cout << imageMat.depth() << std::endl;
//    std::cout << imageMat.type() << std::endl;
//    std::cout << imageMat.channels() << std::endl;

    // Convert the image pixels into 3D point clouds and crate a ply file in the end.
    std::vector<std::vector<float>> gridMat;
    for (r=0; r<imageMat.rows; r++){
        for (c=0; c<imageMat.cols; c++){
            float Zw = imageMat.at<float>(r,c)/(1000.0f);				//Depth
            // If the depth is beyond the required value the ignore the pixel.
            if (Zw > 0.5 && Zw < atoi(argv[2])){
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
        }
    }

    // Sanity check for the image.
//    float *tmpData = reinterpret_cast<float*>(imageMat.data);
//    // Display few pixels for sanity check.
//    for (r=0; r<10; r++){
//        for (c=0; c<14; c++){
//            std::cout <<tmpData[r*14+c] << "\t";
////            std::cout<< imageMat.at<float>(r,c) << "\t";
//        }
//        std::cout << std::endl;
//    }

//    // Sanity check for the evaluation of matrix values.
//    for (r=0; r<2; r++){
//        for (c=0; c<7; c++){
//            std::cout << gridMat[r*7+c][0] << "\t" << gridMat[r*7+c][1] << "\t" \
//                                           << gridMat[r*7+c][2] << std::endl;
//        }
//    }

    // Now write the points into a ply file.
    std::ofstream plyFile;
    plyFile.open("xyz.ply");

    // Insert the header
    plyFile << "ply\n";
    plyFile << "format  binary 1.0\n";
    plyFile << "comment made by Tushar Nakini, ViGIR\n";
    plyFile << "element vertex " << gridMat.size() <<"\n";
    plyFile << "property float32 x\n";
    plyFile << "property float32 y\n";
    plyFile << "property float32 z\n";
    plyFile << "property uchar red\n";
    plyFile << "property uchar green\n";
    plyFile << "property uchar blue\n";
    plyFile << "end_header\n";

    // Write the values into the ply file.
    for (int iGM=0; iGM<gridMat.size(); iGM++){
        plyFile << gridMat[iGM][0] << " " << gridMat[iGM][1] << " " << gridMat[iGM][2] << " "\
                                   << 255 << " " << 255 << " " << 255 << "\n";
    }

    plyFile.close();
    return 0;
}
