#include "shadow_utils.h"

// img and bckImg are gray images, and maskImg is the foreground mask.
IplImage* detectShadowNCC(IplImage *img, IplImage *bckImg, IplImage *maskImg, 
                          int N, float L_NCC) {
    int height = img->height;
    int width = img->width;
    int step = img->widthStep/sizeof(uchar);
    int channels = img->nChannels;
    uchar *dataImg = (uchar *)img->imageData;
    uchar *dataBckImg = (uchar *)bckImg->imageData;

    BwImage imgWrapper(img);
    BwImage bckImgWrapper(bckImg);
    BwImage maskImgWrapper(maskImg);
	
    IplImage* shadowImg = cvCreateImage(cvSize(width, height), IPL_DEPTH_8U, 1);
    BwImage shadowImgWrapper(shadowImg);
	
    IplImage* shadowImgResultRGB = cvCreateImage(cvSize(width, height), IPL_DEPTH_8U, 3);
    RgbImage shadowImgResultRGBWrapper(shadowImgResultRGB);
	
    for(int i = 0; i < height; i++) {
        for(int j = 0; j < width; j++) {
            if(maskImgWrapper[i][j] > 0) {
                float NCC = 0;
                float ER = 0;
                float E_B = 0;
                float E_T = 0;
                float E_B_tmp = 0; 
                float E_T_tmp = 0;
                
                for( int n = -N; n <= N; n++ ) {
                    for( int m = -N; m <= N; m++ ) {
                        // Prevent the out of bound problem
                        if(!((i + n) < 0) && !((j + m) < 0) && 
                           !((i + n) > height) && !((j + m) > width)) {
							ER += (bckImgWrapper[i + n][j + m] * imgWrapper[i + n][j + m]);
							E_B_tmp += pow(bckImgWrapper[i + n][j + m], 2);
							E_T_tmp += pow(imgWrapper[i + n][j + m], 2);
						}
					}
				}
				E_B = sqrt(E_B_tmp);
				E_T = sqrt(E_T_tmp);
				
				if((E_B * E_T) == 0) {
				    cerr << "Divided by zero" << endl;
				    exit(1);
				}
				else {
					NCC = ER / (E_B * E_T);
				}
				
				if((NCC >= L_NCC) && (E_T < E_B)) {
				    // Shadow pixels as red
					shadowImgWrapper[i][j] = 255;
					shadowImgResultRGBWrapper[i][j].r = 255;
					shadowImgResultRGBWrapper[i][j].g = 0;
					shadowImgResultRGBWrapper[i][j].b = 0;
				}
				else {
				    // Object pixels as green
					shadowImgWrapper[i][j] = 0;
					shadowImgResultRGBWrapper[i][j].r = 0;
					shadowImgResultRGBWrapper[i][j].g = 255;
					shadowImgResultRGBWrapper[i][j].b = 0;
				}
			}
			else {
			    // Neither shadow pixels nor object pixels
				shadowImgWrapper[i][j] = 0;
				shadowImgResultRGBWrapper[i][j].r = 0;
				shadowImgResultRGBWrapper[i][j].g = 0;
				shadowImgResultRGBWrapper[i][j].b = 0;
			}
		}
	}

	return shadowImg;
}

IplImage* classify_pixels(IplImage *bckResult, IplImage *shadowImg) {
	int height = bckResult->height;
	int width = bckResult->width;
	
	BwImage bckResultImgWrapper( bckResult );
	BwImage shadowImgWrapper( shadowImg );
	
	IplImage* resultImg = cvCreateImage(cvSize(width, height), IPL_DEPTH_8U, 3);
	RgbImage resultImgWrapper(resultImg);
	
	for( int i = 0; i < height; i++ ) {
		for( int j = 0; j < width; j++ ) {
			if(bckResultImgWrapper[i][j] > 0) {
				if(shadowImgWrapper[i][j] > 0) {
				    // Shadow pixels as red
					resultImgWrapper[i][j].r = 255;
				}
                else {
                    // Object pixels as green
                    resultImgWrapper[i][j].g = 255;
                }
			}
		}
	}
	
	return resultImg;
}

int main(int argc, char ** argv) {
    int c;
    extern char *optarg;
    extern int optind, opterr, optopt;
    
    // th and ap are used for background subtraction.
    float th = 15;
    double ap = 0.7;
	
	int N = 4;	// for a ( 2N + 1 ) x ( 2N + 1 ) template
	float L_NCC = 0.995;
	int delay = 10;

    while((c = getopt(argc, argv, "T:N:L:d:")) != EOF) {
    	switch(c) {
            case 'T':
				sscanf(optarg, "%f", &th);
				break;
			case 'N':
				sscanf(optarg, "%d", &N);
				break;
			case 'L':
				sscanf(optarg, "%f", &L_NCC);
				break;
			case 'd':
				sscanf(optarg, "%d", &delay);
				break;
		}
	}

//	cout << "argc: " << argc << endl;
//	cout << "optind:" << optind << endl;

	if((argc - optind) != 1) {
		cout << "Usage error" << endl;
		cout << "Usage: shadow_ncc <video file>" << endl;
		cout << "Option: " << endl;
		cout << "-T <threshold for background subtraction>" << endl;
		cout << "-N <size of template>" << endl;
		cout << "-L <threshold for NCC>" << endl;
		cout << "-d <delay>" << endl;

		exit(1);
	}

    cout << "N: " << N << endl;
    cout << "L: " << L_NCC << endl;
	cout << "Threshold: " << th << endl;

    IplImage *bck_img = NULL; 
    IplImage *cur_img = NULL;
    IplImage *cur_img_gray = NULL;
    IplImage *bck_img_gray = NULL;
    IplImage *res_bck_img = NULL;
    IplImage *shadow_img = NULL;
    IplImage *res_img = NULL;

	CvCapture* bck_capture = cvCaptureFromAVI(argv[optind]);
	cvGrabFrame(bck_capture);
	bck_img = cvQueryFrame(bck_capture);
	
	int height = bck_img->height;
	int width = bck_img->width;
	int depth = bck_img->depth;
    int step = bck_img->widthStep/sizeof(uchar);
	int channels = bck_img->nChannels;
	
	// Convert the background image to grayscale.
	bck_img_gray = cvCreateImage(cvSize(width, height), IPL_DEPTH_8U, 1);
	cvCvtColor(bck_img, bck_img_gray, CV_BGR2GRAY);

	cvNamedWindow("Background Reference Image");
	cvMoveWindow("Background Reference Image", 0, 75);
	
	cvNamedWindow("Current Frame");
	cvMoveWindow("Current Frame", 390, 75);

	cvNamedWindow("Background Subtraction Result");
	cvMoveWindow("Background Subtraction Result", 0, 350);
	
	cvNamedWindow("Shadow Detection Result");
	cvMoveWindow("Shadow Detection Result", 390, 350);

	CvCapture* capture = cvCaptureFromAVI(argv[optind]);
	while(cvGrabFrame(capture)) {
        cur_img = cvQueryFrame(capture);
		if(!cur_img) {
            cerr << "Cannot grab a frame" << endl;
            break;
		}

		cur_img_gray = cvCreateImage(cvSize(cur_img->width, cur_img->height), 
		                             IPL_DEPTH_8U, 1);
		cvCvtColor(cur_img, cur_img_gray, CV_BGR2GRAY);

		res_bck_img = subtract_img(cur_img, bck_img, th, ap);

        cvMorphologyEx(res_bck_img, res_bck_img, NULL, NULL, CV_MOP_OPEN, 1);
        cvMorphologyEx(res_bck_img, res_bck_img, NULL, NULL, CV_MOP_CLOSE, 1);
		
		shadow_img = detectShadowNCC(cur_img_gray, bck_img_gray, res_bck_img, 
		                             N, L_NCC);

		res_img = classify_pixels(res_bck_img, shadow_img);

        cvShowImage("Background Reference Image", bck_img);
		cvShowImage("Background Subtraction Result", res_bck_img);
		cvShowImage("Current Frame", cur_img);
		cvShowImage("Shadow Detection Result", res_img);
		
		cvReleaseImage(&shadow_img);

		cvWaitKey(delay);
	}
	
	// Destroy windows
	cvDestroyWindow("Background Reference Image");
	cvDestroyWindow("Current Frame");
	cvDestroyWindow("Background Subtraction Results");
	cvDestroyWindow("Shadow Removal Results");

	return 1;
}




