#include "shadow_utils.h"

int main(int argc, char **argv) {
    int c;
    extern char *optarg;
    extern int optind, opterr, optopt;
    
    float alpha = 0.5;
    float beta = 1;
    float T_S = 50;
    float T_H = 20;
    
    // th and ap are used for background subtraction.
    float th = 15;
    double ap = 0.7;
    int delay = 10;

    while((c = getopt(argc, argv, "a:b:s:h:t:d:")) != EOF) {
        switch(c) {
            case 'a':
        	    sscanf(optarg, "%f", &alpha);
        	    break;
            case 'b':
        	    sscanf(optarg, "%f", &beta);
        	    break;
        	case 's':
        	    sscanf(optarg, "%f", &T_S);
        	    break;
            case 'h':
        	    sscanf(optarg, "%f", &T_H);
        	    break;
            case 't':
        	    sscanf(optarg, "%f", &th);
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
		cout << "Usage: shadow_hsv <video>" << endl;
		cout << "Option: " << endl;
		cout << "-a <Alpha>" << endl;
		cout << "-b <Beta>" << endl;
		cout << "-s <T_S>" << endl;
		cout << "-h <T_H>" << endl;
		cout << "-t <Threshold for background subtraction>" << endl;
		cout << "-d <delay>" << endl;
		
        return 0;
	}
	
    cout << "Alpha: " << alpha << endl;
    cout << "Beta: " << beta << endl;
    cout << "T_S: " << T_S << endl;
    cout << "T_H: " << T_H << endl;
    cout << "Threshold: " << th << endl;

	IplImage *bck_img = NULL;
    IplImage *cur_img = NULL;
    IplImage *bck_sub_img = NULL;
    IplImage *cur_img_hsv = NULL;
    IplImage *res_img = NULL;

    cvNamedWindow("Background Reference Image");
    cvMoveWindow("Background Reference Image", 0, 75);
    
    cvNamedWindow("Current Frame");
    cvMoveWindow("Current Frame", 390, 75);
    
    cvNamedWindow("Background Subtraction Results");
    cvMoveWindow("Background Subtraction Results", 0, 350);
         
    cvNamedWindow("Shadow Detection Results");
    cvMoveWindow("Shadow Detection Results", 390, 350);

    // Grab the first frame to be a background image.
    // In order to make the program simple, we will not update the background model.
    CvCapture* bck_capture = cvCaptureFromAVI(argv[optind]);
    cvGrabFrame(bck_capture);
    bck_img = cvQueryFrame(bck_capture);
    
    int height = bck_img->height;
    int width = bck_img->width;
    int depth = bck_img->depth;
    int step = bck_img->widthStep/sizeof(uchar);
    int channels = bck_img->nChannels;

    // Convert the background image into HSV space.
    IplImage *bck_img_hsv = cvCreateImage(cvSize(width, height), depth, channels);
    cvCvtColor(bck_img, bck_img_hsv, CV_BGR2HSV);
    uchar *data_bck_img_hsv = (uchar *)bck_img_hsv->imageData;
    
    // Start capturing the frames.
    CvCapture* capture = cvCaptureFromAVI(argv[optind]);
    while(cvGrabFrame(capture)) {
        cur_img = cvQueryFrame(capture);
        if(!cur_img) {
            cerr << "Cannot grab a frame" << endl;
            break;
        }

        // Convert the current frame into HSV space.
        cur_img_hsv = cvCreateImage(cvSize(width, height), depth, channels);
        cvCvtColor(cur_img, cur_img_hsv, CV_BGR2HSV);
        uchar *data_img_hsv = (uchar *)cur_img_hsv->imageData;
        
        res_img = cvCreateImage(cvSize(width, height), depth, channels);
        RgbImage rgb_img_wrapper(res_img);
    
        // Subtract the current frame by the background model (the first frame).
        bck_sub_img = subtract_img(cur_img, bck_img, th, ap);

        cvMorphologyEx(bck_sub_img, bck_sub_img, NULL, NULL, CV_MOP_OPEN, 1);
        cvMorphologyEx(bck_sub_img, bck_sub_img, NULL, NULL, CV_MOP_CLOSE, 1);

        BwImage mask_img_wrapper(bck_sub_img);
        
        for(int i = 0; i < height; i++) {
            for(int j = 0; j < width; j++) {
                // Consider only the white pixels.
                if(mask_img_wrapper[i][j] > 0) {
                    // H component in OpenCV ranges from 0 to 180.
                    float I_H = 2 * data_img_hsv[i * step + j * channels + 0];
                    float B_H = 2 * data_bck_img_hsv[i * step + j * channels + 0];
                    
                    float I_S = data_img_hsv[i * step + j * channels + 1];
                    float B_S = data_bck_img_hsv[i * step + j * channels + 1];
                    float I_V = data_img_hsv[i * step + j * channels + 2];
                    float B_V = data_bck_img_hsv[i * step + j * channels + 2];
                    
                    float h_diff = I_H - B_H;
                    
                    if(h_diff > 180) {
                        h_diff = 360 - h_diff;
                    }
                    else if(h_diff < -180) {
                        h_diff = -360 + h_diff;
                    }

                    int sh = 0;
                    if((I_V / B_V >= alpha && I_V / B_V <= beta) && 
                        (I_S - B_S) <= T_S &&
                        abs(h_diff) <= T_H) {
                        sh = 1;
                    }

                    if(sh) {
                        // Shadow pixels as red
                        rgb_img_wrapper[i][j].r = 255;
                    }
                    else {
                        // Object pixels as green
                        rgb_img_wrapper[i][j].g = 255;
                    }
                }
            }
        }

        cvShowImage("Background Reference Image", bck_img);
        cvShowImage("Background Subtraction Results", bck_sub_img);
        cvShowImage("Current Frame", cur_img);
        cvShowImage("Shadow Detection Results", res_img);

        cvWaitKey(delay);
    }

    cvDestroyWindow("Background Reference Image");
    cvDestroyWindow("Background Subtraction Results");
    cvDestroyWindow("Current Frame");
    cvDestroyWindow("Shadow Detection Results");

	return 1;
}

