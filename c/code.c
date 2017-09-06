#include <stdio.h>
#include <cv.h>
#include <highgui.h> 

IplImage* image = 0;
double low = 5, ratio = 3;

void canny();

int main(int argc, char**argv){
	image = cvLoadImage(argv[1], CV_LOAD_IMAGE_GRAYSCALE);
	canny();

	cvShowImage("rihanna", image);
	cvWaitKey(0);

	cvReleaseImage(&image);
}

void canny(){
	const CvArr* img = image;
	cvCanny(img, image, low, low*ratio, 3);
}