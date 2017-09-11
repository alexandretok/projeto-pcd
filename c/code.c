#include <stdio.h>
#include <cv.h>
#include <highgui.h> 

IplImage* image = 0;
double low = 5, ratio = 3;

void canny();
void gauss(int);

int main(int argc, char**argv){
	image = cvLoadImage(argv[1], CV_LOAD_IMAGE_GRAYSCALE);
	gauss(15);
	canny();
	
	cvShowImage("imagem", image);
	cvWaitKey(0);

	cvReleaseImage(&image);
}

void gauss(int weight){
	if(weight % 2 == 0)
		printf("\n\nERROR IN: gauss(weight) -> weight should be an ODD number (ex: 1, 3, 5...)\n\n");

	cvSmooth(image, image, CV_GAUSSIAN, weight, weight, 0, 0);
}

void canny(){
	const CvArr* img = image;
	cvCanny(img, image, low, low*ratio, 3);
}