#include <stdio.h>
#include <opencv2/opencv.hpp>

//using namespace cv;

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

bool depth_image_to_point_cloud(){
    //Process Depth To PCL Cloud
    uint pixel_count = depth.rows * depth.cols;
    dst->resize(pixel_count);
    dst->height = depth.rows;
    dst->width = depth.cols;

    float x = 0.0f, y = 0.0f;

    float* ptr = (float*) (depth.data);
    for (uint i = 0; i < pixel_count; ++i)
    {
        cv::Vec2f xy = xycords.at<cv::Vec2f>(0, i);
        x = xy[1]; y = xy[0];

        PointT point;
        point.z = (static_cast<float>(*ptr)) / (1000.0f); // Convert from mm to meters
        point.x = (x - config.camera_params.ir.cx) * point.z / config.camera_params.ir.fx;
        point.y = (y - config.camera_params.ir.cy) * point.z / config.camera_params.ir.fy;
        point.r = 128; point.g = 128; point.b = 128;
        dst->at(i) = point;

        ++ptr;
    }
}

int main(int argc, char** argv )
{
    if ( argc != 2 )
    {
        printf("usage: DisplayImage.out <Image_Path>\n");
        return -1;
    }

    cv::Mat image;
    image = cv::imread( argv[1], 1 );

    if ( !image.data )
    {
        printf("No image data \n");
        return -1;
    }
    cv::namedWindow("Display Image", cv::WINDOW_AUTOSIZE );
    cv::imshow("Display Image", image);

    cv::waitKey(0);

    return 0;
}
