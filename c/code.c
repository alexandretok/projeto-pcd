#include <stdio.h>
#include <cv.h>
#include <highgui.h>
// #include <graphics.h>

IplImage* image = 0;
double low = 5, ratio = 3;

void canny();
void region_of_interest();

int main(int argc, char**argv){
	image = cvLoadImage(argv[1], CV_LOAD_IMAGE_GRAYSCALE);
	canny();

	region_of_interest();

	cvShowImage("rihanna", image);
	cvWaitKey(0);

	cvReleaseImage(&image);
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