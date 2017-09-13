#include <stdio.h>
#include <cv.h>
#include <highgui.h>

IplImage* image = 0;
IplImage* original_image = 0;
double low = 5, ratio = 3;

void canny(IplImage*,int);
void region_of_interest(IplImage*);
void gauss(IplImage*,int);
void hough(IplImage*,float, float, int, float, float);
void find_lanes(IplImage*, IplImage*);
CvPoint polygon[4];

int main(int argc, char**argv){

	image = cvLoadImage(argv[1], CV_LOAD_IMAGE_GRAYSCALE);
	// original_image = cvLoadImage(argv[1], CV_LOAD_IMAGE_UNCHANGED);
	// gauss(image, 11);
	// canny(image,3);
	// region_of_interest(image);
	// cvShowImage("imagem3", image);
	// hough(image, 1, CV_PI/180, 69, 10, 10);
	// cvShowImage("imagem4", image);
	// find_lanes(image,original_image);
	// cvShowImage("imagem5", image);
	
	// cvShowImage("imagem", original_image);

	CvCapture* video = cvCaptureFromFile("solidWhiteRight.mp4");
	IplImage* original_frame;
	original_frame = cvQueryFrame(video);

	double fps = cvGetCaptureProperty(video, CV_CAP_PROP_FPS);
	CvSize frame_size;
    frame_size.width = original_frame->width;
    frame_size.height = original_frame->height;

	CvVideoWriter* output_video = cvCreateVideoWriter("output_video.mp4", CV_FOURCC('P','I','M','4'), fps, frame_size, 1);

	while(cvGrabFrame(video) > 0){
		printf("dsa ");
		original_frame = cvQueryFrame(video);
		IplImage* frame;

		cvSaveImage("original_frame.jpg", original_frame, 0);
		frame = cvLoadImage("original_frame.jpg", CV_LOAD_IMAGE_GRAYSCALE);

		gauss(frame, 11);
		canny(frame,3);
		region_of_interest(frame);
		hough(frame, 1, CV_PI/180, 69, 10, 10);
		find_lanes(frame,original_frame);

		cvWriteFrame(output_video, original_frame);

		// cvShowImage("frame", original_frame);
	}


	cvWaitKey(0);

	cvReleaseImage(&image);
}

void gauss(IplImage* image, int weight){
	if(weight % 2 == 0)
		printf("\n\nERROR IN: gauss(weight) -> weight should be an ODD number (ex: 1, 3, 5...)\n\n");

	cvSmooth(image, image, CV_GAUSSIAN, weight, weight, 0, 0);
}

void canny(IplImage* image, int apertureSize){
	const CvArr* img = image;
	cvCanny(img, image, low, low*ratio, apertureSize);
}

void region_of_interest(IplImage* image_frame){
	const CvArr* img = image_frame;
	IplImage* mask = cvCreateImage(cvGetSize(image_frame), 8, 1);
    polygon[0] = cvPoint(140,530);
    polygon[1] = cvPoint(430,330);
    polygon[2] = cvPoint(530,330);
    polygon[3] = cvPoint(920,530);
	CvPoint* curveArr[]={polygon};
	int nCurvePts[]={4};
	int nCurves=1;

	cvFillPoly(mask, curveArr, nCurvePts, nCurves, CV_RGB(255,255,255), 8, 0);
	cvAnd(image_frame, mask, image_frame, NULL);

}

void hough(IplImage* image, float rho, float theta, int threshold, float min_line_len, float max_line_gap){
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

void find_lanes(IplImage* image, IplImage* original_image){
	int upperLimit = polygon[1].y;

	// Finds the first (most to left and top) red pixel on the left side of the image
	int leftLine[4] = {0, 0, 0, 0};
	for (int i = upperLimit; i < image->height; i++){
		if(leftLine[0] != 0)
			break;
		for (int j = 0; j < image->width/2; j++){
			if(CV_IMAGE_ELEM(image,unsigned char,i,j) > 0){
				leftLine[0] = j;
            	leftLine[1] = i;
			}
		}
	}
	
    // Finds the last (most to right and bottom) red pixel on the left side of the image
    for (int i = image->height - 1; i > upperLimit; i--){
    	if(leftLine[2] != 0)
    		break;
		for (int j = 0; j < image->width/2; j++){
			if(CV_IMAGE_ELEM(image,unsigned char,i,j) > 0){
				leftLine[2] = j;
            	leftLine[3] = i;
			}
		}
    }

    // Finds the first (most to left and top) red pixel on the right side of the image
    int rightLine[4] = {0, 0, 0, 0};
	for (int i = upperLimit; i < image->height; i++){
		if(rightLine[0] != 0)
			break;
		for (int j = image->width/2; j < image->width; j++){
			if(CV_IMAGE_ELEM(image,unsigned char,i,j) > 0){
				rightLine[0] = j;
            	rightLine[1] = i;
			}
		}
	}

    // Finds the last (most to right and bottom) red pixel on the right side of the image
    for (int i = image->height - 1; i > upperLimit; i--){
    	if(rightLine[2] != 0)
    		break;
		for (int j = image->width/2; j < image->width; j++){
			if(CV_IMAGE_ELEM(image,unsigned char,i,j) > 0){
				rightLine[2] = j;
            	rightLine[3] = i;
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

    // CvPoint* lines = {leftPointLines, rightPointLines};

    cvLine(original_image, leftPointLines[0], leftPointLines[1], CV_RGB(255,0,0), 4, CV_AA, 0);
    cvLine(original_image, rightPointLines[0], rightPointLines[1], CV_RGB(255,0,0), 4, CV_AA, 0);

    // return lines;
}