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
