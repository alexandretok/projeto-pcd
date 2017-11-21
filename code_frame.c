#include <stdio.h>
#include <sys/time.h>
#include <cv.h>
#include <highgui.h>

CvPoint polygon[4];
const double LOW = 30, RATIO = 4;
const int APERTURE_SIZE = 3;
const int WEIGHT = 11;

struct timeval tempo_inicial, tempo_final;
int long tmili;

void canny(IplImage*);
void region_of_interest(IplImage*);
void gauss(IplImage*);
void hough(IplImage*,float, float, int, float, float);
void find_lanes(IplImage*, IplImage*);

int main(int argc, char**argv){

	// load video
	CvCapture* video = cvCaptureFromFile("solidWhiteRight.mp4");

	// get video properties
	// double width = cvGetCaptureProperty(video, CV_CAP_PROP_FRAME_WIDTH);
	// double height = cvGetCaptureProperty(video, CV_CAP_PROP_FRAME_HEIGHT);
	// double fps = cvGetCaptureProperty(video, CV_CAP_PROP_FPS);
	// double frameCount = cvGetCaptureProperty(video, CV_CAP_PROP_FRAME_COUNT);


	// double framesProcessed = 0;

	// set video output
	// CvVideoWriter* output_video = cvCreateAVIWriter("output_video_3.avi", CV_FOURCC('D', 'I', 'V', 'X'), fps, cvSize(width, height), 1);

	gettimeofday(&tempo_inicial, NULL);  // inicio é uma struct com dois campos:tv_sec e tv_usec.

	printf("Starting...\n");

	// tempo_de_hoje = (int long) (1000 * (tempo_inicial.tv_sec) + (tempo_inicial.tv_usec) / 1000); // para transformar em milissegundos

	IplImage* original_frame;
	original_frame = cvQueryFrame(video);
	IplImage* frame = cvCreateImage(cvGetSize(original_frame),8,1);

	// convert to grayscale
	cvCvtColor(original_frame, frame, CV_BGR2GRAY);

	gauss(frame);
	// cvShowImage("gauss_frame", frame);
	// cvWaitKey(0);
	canny(frame);
	// cvShowImage("canny_frame", frame);
	// cvWaitKey(0);
	region_of_interest(frame);
	// cvShowImage("region_of_interest_frame", frame);
	// cvWaitKey(0);
	hough(frame, 1, CV_PI/180, 69, 10, 10);
	// cvShowImage("hough_frame", frame);
	// cvWaitKey(0);
	find_lanes(frame,original_frame);
	cvShowImage("find_lanes_frame", frame);
	cvShowImage("final_frame", original_frame);

	gettimeofday(&tempo_final, NULL);
    tmili = (int long) (1000 * (tempo_final.tv_sec - tempo_inicial.tv_sec) + (tempo_final.tv_usec - tempo_inicial.tv_usec) / 1000); // para transformar em milissegundos
    printf("tempo decorrido: %ld milissegundos\n", tmili);

	cvWaitKey(0);

	// while(1){

	// }

	// cvWriteFrame(output_video, original_frame);
	

	// while(framesProcessed < frameCount){
	// 	IplImage* original_frame;
	// 	original_frame = cvQueryFrame(video);
	// 	if(original_frame) {
	// 		IplImage* frame = cvCreateImage(cvGetSize(original_frame),8,1);

	// 		// convert to grayscale
	// 		cvCvtColor(original_frame, frame, CV_BGR2GRAY);

	// 		gauss(frame);
	// 		canny(frame);
	// 		region_of_interest(frame);
	// 		hough(frame, 1, CV_PI/180, 69, 10, 10);
	// 		find_lanes(frame,original_frame);
	// 		// cvShowImage("frame", original_frame);
	// 		// cvWaitKey(33);

	// 		cvWriteFrame(output_video, original_frame);
	// 	}

	// 	framesProcessed++;

	// 	// print progress every second processed
	// 	if((int)framesProcessed % (int)fps == 0 || framesProcessed == frameCount){
	// 		// system("clear");
	// 		printf("%.2f%%\n", 100 * framesProcessed/frameCount);
	// 	}

	// 	// gettimeofday(&tempo_final, NULL);
	//  //    tmili = (int long) (1000 * (tempo_final.tv_sec - tempo_inicial.tv_sec) + (tempo_final.tv_usec - tempo_inicial.tv_usec) / 1000); // para transformar em milissegundos
	//  //    printf("tempo decorrido: %ld milissegundos\n", tmili);
	//  //    break;
	// }
	
	printf("\nFinished!\n\n");
	// cvReleaseVideoWriter(&output_video);

    // return 0;
}

void gauss(IplImage* image){
	gettimeofday(&tempo_inicial, NULL);  // inicio é uma struct com dois campos:tv_sec e tv_usec.
	
	if(WEIGHT % 2 == 0)
		printf("\n\nERROR IN: gauss(WEIGHT) -> WEIGHT should be an ODD number (ex: 1, 3, 5...)\n\n");

	cvSmooth(image, image, CV_GAUSSIAN, WEIGHT, WEIGHT, 0, 0);

	gettimeofday(&tempo_final, NULL);
    tmili = (int long) (1000 * (tempo_final.tv_sec - tempo_inicial.tv_sec) + (tempo_final.tv_usec - tempo_inicial.tv_usec) / 1000); // para transformar em milissegundos
    printf("tempo decorrido gauss: %ld milissegundos\n", tmili);
}

void canny(IplImage* image){
	gettimeofday(&tempo_inicial, NULL);  // inicio é uma struct com dois campos:tv_sec e tv_usec.

	cvCanny(image, image, LOW, LOW*RATIO, APERTURE_SIZE);

	gettimeofday(&tempo_final, NULL);
    tmili = (int long) (1000 * (tempo_final.tv_sec - tempo_inicial.tv_sec) + (tempo_final.tv_usec - tempo_inicial.tv_usec) / 1000); // para transformar em milissegundos
    printf("tempo decorrido canny: %ld milissegundos\n", tmili);
}

void region_of_interest(IplImage* image_frame){
	gettimeofday(&tempo_inicial, NULL);  // inicio é uma struct com dois campos:tv_sec e tv_usec.

	IplImage* mask = cvCreateImage(cvGetSize(image_frame), 8, 1);
    polygon[0] = cvPoint(140,530);
    polygon[1] = cvPoint(430,330);
    polygon[2] = cvPoint(530,330);
    polygon[3] = cvPoint(920,530);
	CvPoint* curveArr[]={polygon};
	int nCurvePts[]={4};
	const int nCurves=1;
	cvSetZero(mask);
	cvFillPoly(mask, curveArr, nCurvePts, nCurves, CV_RGB(255,255,255), 8, 0);
	cvAnd(image_frame, mask, image_frame, NULL);

	gettimeofday(&tempo_final, NULL);
    tmili = (int long) (1000 * (tempo_final.tv_sec - tempo_inicial.tv_sec) + (tempo_final.tv_usec - tempo_inicial.tv_usec) / 1000); // para transformar em milissegundos
    printf("tempo decorrido ROI: %ld milissegundos\n", tmili);
}

void hough(IplImage* image, float rho, float theta, int threshold, float min_line_len, float max_line_gap){
	gettimeofday(&tempo_inicial, NULL);  // inicio é uma struct com dois campos:tv_sec e tv_usec.

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

	gettimeofday(&tempo_final, NULL);
    tmili = (int long) (1000 * (tempo_final.tv_sec - tempo_inicial.tv_sec) + (tempo_final.tv_usec - tempo_inicial.tv_usec) / 1000); // para transformar em milissegundos
    printf("tempo decorrido hough: %ld milissegundos\n", tmili);
}

void find_lanes(IplImage* image, IplImage* original_image){
	gettimeofday(&tempo_inicial, NULL);  // inicio é uma struct com dois campos:tv_sec e tv_usec.

	const int upperLimit = polygon[1].y;

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

    cvLine(original_image, leftPointLines[0], leftPointLines[1], CV_RGB(255,0,0), 4, CV_AA, 0);
    cvLine(original_image, rightPointLines[0], rightPointLines[1], CV_RGB(255,0,0), 4, CV_AA, 0);

   	gettimeofday(&tempo_final, NULL);
    tmili = (int long) (1000 * (tempo_final.tv_sec - tempo_inicial.tv_sec) + (tempo_final.tv_usec - tempo_inicial.tv_usec) / 1000); // para transformar em milissegundos
    printf("tempo decorrido FL: %ld milissegundos\n", tmili);
}