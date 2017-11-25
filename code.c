#include <stdio.h>
#include <sys/time.h>
#include <cv.h>
#include <highgui.h>
#include <omp.h>
#include <math.h>
#include <string.h>

CvPoint polygon[4];

void canny(IplImage*,int);
void region_of_interest(IplImage*);
void gauss(IplImage*,int);
void hough(IplImage*,float, float, int, float, float);
void find_lanes(IplImage*, IplImage*);

int threadCount;

int main(int argc, char**argv){
	struct timeval tempo_inicial, tempo_final;
    int long tmili;

    threadCount = strtol(argv[1], NULL, 10);

    polygon[0] = cvPoint(140,530);
    polygon[1] = cvPoint(430,330);
    polygon[2] = cvPoint(530,330);
    polygon[3] = cvPoint(920,530);

	// load video
	CvCapture* video = cvCaptureFromFile("solidYellowLeft_maior.mp4");

	// get video properties
	double width = cvGetCaptureProperty(video, CV_CAP_PROP_FRAME_WIDTH);
	double height = cvGetCaptureProperty(video, CV_CAP_PROP_FRAME_HEIGHT);
	double fps = cvGetCaptureProperty(video, CV_CAP_PROP_FPS);
	double frameCount = cvGetCaptureProperty(video, CV_CAP_PROP_FRAME_COUNT);

	int pieceWidth = width/sqrt(threadCount);
	int pieceHeight = height/sqrt(threadCount);

	double framesProcessed = 0;

	// set video output
	CvVideoWriter* output_video = cvCreateAVIWriter("output_video_3.avi", CV_FOURCC('D', 'I', 'V', 'X'), fps, cvSize(width, height), 1);

	printf("Starting...\n");

	gettimeofday(&tempo_inicial, NULL);

	int total = 0;

	while(framesProcessed < frameCount){
		IplImage* original_frame;
		original_frame = cvQueryFrame(video);
		if(original_frame) {
			IplImage* frame = cvCreateImage(cvGetSize(original_frame),8,1);

			// convert to grayscale
			cvCvtColor(original_frame, frame, CV_BGR2GRAY);

			# pragma omp parallel num_threads(threadCount) private(tempo_inicial, tempo_final, total, tmili)
			{
				IplImage *_image = cvCloneImage(frame);
				int my_rank = omp_get_thread_num();

				int myX = (width / sqrt(threadCount)) * (my_rank % (int)sqrt(threadCount));
				int myY = (height / sqrt(threadCount)) * (my_rank / (int)sqrt(threadCount));

				cvSetImageROI(_image, cvRect(myX, myY, pieceWidth, pieceHeight));
				IplImage *piece = cvCreateImage(cvGetSize(_image), 8, 1);
				cvCopy(_image, piece, NULL);
				cvResetImageROI(_image);

				gettimeofday(&tempo_inicial, NULL);

				gauss(piece, 11);
				canny(piece,3);
				// region_of_interest(piece);
				hough(piece, 1, CV_PI/180, 69, 10, 10);

				gettimeofday(&tempo_final, NULL);
			    tmili = (int long) (1000 * (tempo_final.tv_sec - tempo_inicial.tv_sec) + (tempo_final.tv_usec - tempo_inicial.tv_usec) / 1000); // para transformar em milissegundos
			    total += tmili;
			    printf("Frame processado em: %f milissegundos\n", total / (framesProcessed+1));

				// junta a imagem
				# pragma omp critical
				{
					cvSetImageROI(frame, cvRect(myX, myY, pieceWidth, pieceHeight));
					cvCopy(piece, frame, NULL);
					cvResetImageROI(frame);
				}
			}

			find_lanes(frame,original_frame);
			// cvShowImage("frame", original_frame);
			// cvWaitKey(33);

			cvWriteFrame(output_video, original_frame);
		}

		framesProcessed++;

		// print progress every second processed
		if((int)framesProcessed % (int)fps == 0 || framesProcessed == frameCount){
			printf("%.2f%%\n", 100 * framesProcessed/frameCount);
		}
	}
	
	printf("\nFinished!\n\n");
	cvReleaseVideoWriter(&output_video);

	gettimeofday(&tempo_final, NULL);
    tmili = (int long) (1000 * (tempo_final.tv_sec - tempo_inicial.tv_sec) + (tempo_final.tv_usec - tempo_inicial.tv_usec) / 1000); // para transformar em milissegundos
    printf("Tempo decorrido: %ld milissegundos\n", tmili);
}

void gauss(IplImage* image, int weight){
	if(weight % 2 == 0)
		printf("\n\nERROR IN: gauss(weight) -> weight should be an ODD number (ex: 1, 3, 5...)\n\n");

	cvSmooth(image, image, CV_GAUSSIAN, weight, 0, 0, 0);
}

void canny(IplImage* image, int apertureSize){
	double low = 30, ratio = 4;
	cvCanny(image, image, low, low*ratio, apertureSize);
}

void region_of_interest(IplImage* image_frame){
	IplImage* mask = cvCreateImage(cvGetSize(image_frame), 8, 1);
	CvPoint* curveArr[]={polygon};
	int nCurvePts[]={4};
	int nCurves=1;
	cvSetZero(mask);
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
	for (int i = upperLimit; i < image->height && !leftLine[0]; i++){
		for (int j = 0; j < image->width/2; j++){
			if(CV_IMAGE_ELEM(image,unsigned char,i,j) > 0){
				leftLine[0] = j;
            	leftLine[1] = i;
			}
		}
	}
	
    // Finds the last (most to right and bottom) red pixel on the left side of the image
    for (int i = image->height - 1; i > upperLimit && !leftLine[2]; i--){
		for (int j = 0; j < image->width/2; j++){
			if(CV_IMAGE_ELEM(image,unsigned char,i,j) > 0){
				leftLine[2] = j;
            	leftLine[3] = i;
			}
		}
    }

    // Finds the first (most to left and top) red pixel on the right side of the image
    int rightLine[4] = {0, 0, 0, 0};
	for (int i = upperLimit; i < image->height && !rightLine[0]; i++){
		for (int j = image->width/2; j < image->width; j++){
			if(CV_IMAGE_ELEM(image,unsigned char,i,j) > 0){
				rightLine[0] = j;
            	rightLine[1] = i;
			}
		}
	}

    // Finds the last (most to right and bottom) red pixel on the right side of the image
    for (int i = image->height - 1; i > upperLimit && !rightLine[2]; i--){
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
}