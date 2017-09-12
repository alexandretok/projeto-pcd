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
CvPoint** find_lanes();

int main(int argc, char**argv){
	image = cvLoadImage(argv[1], CV_LOAD_IMAGE_GRAYSCALE);
	gauss(15);
	canny();
	region_of_interest();
	hough(1, CV_PI/180, 69, 10, 10);

	find_lanes();
	// printf(" %d \n", CV_IMAGE_ELEM(image,uchar,0,0));
	
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

	IplImage* image2 = cvCreateImage( cvGetSize(image), 8, 1);
	cvSetZero(image2);

	for( int i = 0; i < lines->total; i++ ) {
        CvPoint* line = (CvPoint*)cvGetSeqElem(lines,i);
        cvLine(image2, line[0], line[1], CV_RGB(255,255,255), 2, 8, 0);
    }

    image = image2;
}

CvPoint** find_lanes(){

	cvShowImage("imagem2", image);

	// Finds the first (most to left and top) red pixel on the left side of the image
	int leftLine[4] = {0, 0, 0, 0};
	for (int i = 330; i < image->height; ++i){
		if(leftLine[0] != 0){
			i = image->height;
		}
		else{
			for (int j = 0; j < image->width/2; ++j){
				if(CV_IMAGE_ELEM(image,int,i,j) > 0){
					leftLine[0] = j;
                	leftLine[1] = i;
				}
			}
		}
	}

    // Finds the last (most to right and bottom) red pixel on the left side of the image
    for (int i = image->height - 1; i > 330; --i){
    	if(leftLine[2] != 0){
    		i = 330;
    	}
    	else{
    		for (int j = 0; j < image->width/2; ++j){
    			if(CV_IMAGE_ELEM(image,int,i,j) > 0){
    				leftLine[2] = j;
                	leftLine[3] = i;
    			}
    		}
    	}
    }

    // Finds the first (most to left and top) red pixel on the right side of the image
    int rightLine[4] = {0, 0, 0, 0};
	for (int i = 330; i < image->height; ++i){
		if(rightLine[0] != 0){
			i = image->height;
		}
		else{
			for (int j = image->width/2; j < image->width; ++j){
				if(CV_IMAGE_ELEM(image,int,i,j) > 0){
					rightLine[0] = j;
                	rightLine[1] = i;
				}
			}
		}
	}

    // Finds the last (most to right and bottom) red pixel on the right side of the image
    for (int i = image->height - 1; i > 330; --i){
    	if(rightLine[2] != 0){
    		i = 330;
    	}
    	else{
    		for (int j = image->width/2; j < image->width; ++j){
    			if(CV_IMAGE_ELEM(image,int,i,j) > 0){
    				rightLine[2] = j;
                	rightLine[3] = i;
    			}
    		}
    	}
    }

    // Calculates the slopes of each lane line to extrapolate
    float leftSlope = ((float) leftLine[1] - leftLine[3]) / ((float) leftLine[0] - leftLine[2]);
    float left_b = ((float) leftLine[1]) - ((float) (leftSlope * leftLine[0]));
    float left_x = (image->height - left_b) / leftSlope;
    leftLine[2] = (int) left_x;
    leftLine[3] = image->height;

    float rightSlope = ((float) rightLine[1] - rightLine[3]) / ((float) rightLine[0] - rightLine[2]);
    float right_b = ((float) rightLine[1]) - ((float) (rightSlope * rightLine[0]));
    float right_x = (image->height - right_b) / rightSlope;
    rightLine[2] = (int) right_x;
    rightLine[3] = image->height;

    CvPoint leftPointLines[] = {cvPoint(leftLine[0],leftLine[1]), cvPoint(leftLine[2],leftLine[3])};
    CvPoint rightPointLines[] = {cvPoint(rightLine[0],rightLine[1]), cvPoint(rightLine[2],rightLine[3])};

    CvPoint** lines = {leftPointLines, rightPointLines};

    printf("left: %d %d %d %d\n", leftLine[0], leftLine[1], leftLine[2], leftLine[3]);
    printf("right: %d %d %d %d\n", rightLine[0], rightLine[1], rightLine[2], rightLine[3]);

    return lines;
}