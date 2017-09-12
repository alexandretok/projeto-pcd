#include <stdio.h>
#include <cv.h>
#include <highgui.h>
// #include <graphics.h>

IplImage* image = 0;
double low = 5, ratio = 3;

void canny();
void region_of_interest();
void gauss(int);
void hough(float, float, int, float, float);

int main(int argc, char**argv){
	image = cvLoadImage(argv[1], CV_LOAD_IMAGE_GRAYSCALE);
	gauss(15);
	canny();
	region_of_interest();
	hough(1, CV_PI/180, 50, 30, 10);
	
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

void region_of_interest(){
	const CvArr* img = image;
	IplImage* mask = cvCreateImage(cvGetSize(image), 8, 1);

	CvPoint  curve[]={cvPoint(140,530),  cvPoint(430,330),  cvPoint(530,330),  cvPoint(920,530)};
	CvPoint* curveArr[]={curve};
	int      nCurvePts[]={4};
	int      nCurves=1;

	cvFillPoly(mask, curveArr, nCurvePts, nCurves, CV_RGB(255,255,255), 8, 0);
	cvAnd(image, mask, image, NULL);
}

void hough(float rho, float theta, int threshold, float min_line_len, float max_line_gap){
	CvMemStorage* storage = cvCreateMemStorage(0);
	CvSeq* lines = 0;
	lines = cvHoughLines2(image, storage, CV_HOUGH_PROBABILISTIC, rho, theta, threshold, min_line_len, max_line_gap);

	IplImage* image2 = cvCreateImage( cvGetSize(image), 8, 3 );
	cvCvtColor( image, image2, CV_GRAY2BGR );
	for( int i = 0; i < lines->total; i++ ) {
        CvPoint* line = (CvPoint*)cvGetSeqElem(lines,i);
        cvLine(image2, line[0], line[1], CV_RGB(255,0,0), 2, 8, 0);
    }

    image = image2;
}