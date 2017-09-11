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
	CvArr* img_dest;
	int mask[image->width][image->height];
	memset(mask, 0, sizeof(mask[0][0]) * image->width * image->height);
	CvArr* mask_img = mask;

	printf("%d\n", image->width);
	printf("%d\n", image->height);

	// np.array([[(60,image.shape[0]),(430, 330), (530, 330), (image.shape[1]-60,image.shape[0])]], dtype=np.int32)

	CvPoint  curve[]={cvPoint(60,10),  cvPoint(10,100),  cvPoint(100,100),  cvPoint(100,10)};
	CvPoint* curveArr[1]={curve};
	int      nCurvePts[1]={4};
	int      nCurves=1;

	cvFillPoly(mask_img, curveArr, nCurvePts, nCurves, CV_RGB(255,255,255), 8, 0);

	printf("desenha\n");

	// masked_image = cv2.bitwise_and(img, mask)
	cvAnd(img, mask_img, img_dest, NULL);
	cvShowImage("region_of_interest", img_dest);
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