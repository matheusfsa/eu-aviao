#include "Bmp.h"
#include <iostream>
#include <GL/freeglut.h>
//#include <opencv2/core/core.hpp>
//#include <opencv2/highgui/highgui.hpp>
//using namespace cv;
using namespace std;
int main(int argc, char** argv)
{
    Image::Bmp bmp;
    if(!bmp.read("textures/mars/normal.bmp")){
        cout << "error" << endl;
        return 0;     // exit if failed load image

    }
    // get bmp info
    cout << "height " << endl;
    int width = bmp.getWidth();
    int height = bmp.getHeight();
    const unsigned char* data = bmp.getDataRGB();
    cout << "height " << height << endl;
    cout << "width " << width << endl;
    cout << "size " << bmp.getDataSize() << endl;
    cout << "n_pixels " << height*width << endl;
    cout << "n_channels " << bmp.getBitCount()/8 << endl;
    //cout << "n_pixels " <<  bmp.getDataSize()/(bmp.getBitCount()/8) << endl;
    //cout << ((height*bmp.getBitCount())) << endl;
    cout << (int)data[3 * (0 * width + 0) + 0]<< endl;
    cout << (int)data[3 * (0 * width + 0) + 1]<< endl;
    cout << (int)data[3 * (0 * width + 0) + 2]<< endl;
    //cout << getPixel(bmp, 0, 0, 0) << endl;
}