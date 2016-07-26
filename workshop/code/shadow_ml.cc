#include "shadow_utils.h"

int main(int argc, char **argv) {
    int c;
    extern char *optarg;
    extern int optind, opterr, optopt;
    
    // th and ap are used for background subtraction.
    float th = 15;
    double ap = 0.7;
    
    int delay = 10;

    while((c = getopt(argc, argv, "T:d:")) != EOF) {
        switch(c) {
            case 'T':
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
		cout << "Usage: shadow_ml <video>" << endl;
		cout << "Option: " << endl;
		cout << "-T <threshold for background subtraction>" << endl;
		cout << "-d <delay>" << endl;
		
        return 0;
	}
	cout << "Threshold: " << th << endl;

	IplImage *bck_img = NULL;
    IplImage *cur_img = NULL;
    IplImage *bck_sub_img = NULL;
    IplImage *cur_img_hsv = NULL;
    IplImage *res_img = NULL;

    // Load and set the HSV data.
    float fg_mu_h = 0;
    float fg_std_h = 0;
    
    float fg_mu_s = 0;
    float fg_std_s = 0;
    
    float fg_mu_v = 0;
    float fg_std_v = 0;
    
    float sh_mu_h = 0;
    float sh_std_h = 0;
    
    float sh_mu_s = 0;
    float sh_std_s = 0;
    
    float sh_mu_v = 0;
    float sh_std_v = 0;

    // *** NEED TO CHANGE THE VALUES BELOW ***
    fg_mu_h = -1;
    fg_std_h =  -1;
    fg_mu_s = -1;
    fg_std_s =  -1;
    fg_mu_v =  -1;
    fg_std_v =  -1;
    sh_mu_h = -1;
    sh_std_h =  -1;
    sh_mu_s = -1;
    sh_std_s =  -1;
    sh_mu_v =  -1;
    sh_std_v =  -1;

    cvNamedWindow("Background Reference Image");
    cvMoveWindow("Background Reference Image", 0, 75);
    
    cvNamedWindow("Current Frame");
    cvMoveWindow("Current Frame", 390, 75);
    
    cvNamedWindow("Background Subtraction Results");
    cvMoveWindow("Background Subtraction Results", 0, 350);
         
    cvNamedWindow("Shadow Detection Results");
    cvMoveWindow("Shadow Detection Results", 390, 350);

    CvCapture* bck_capture = cvCaptureFromAVI(argv[optind]);
    cvGrabFrame(bck_capture);
    bck_img = cvQueryFrame(bck_capture);
    
    int height = bck_img->height;
    int width = bck_img->width;
    int depth = bck_img->depth;
    int step = bck_img->widthStep/sizeof(uchar);
    int channels = bck_img->nChannels;

    IplImage *bck_img_hsv = cvCreateImage(cvSize(width, height), depth, channels);
    cvCvtColor(bck_img, bck_img_hsv, CV_BGR2HSV);
    uchar *data_bck_img_hsv = (uchar *)bck_img_hsv->imageData;
    
    CvCapture* capture = cvCaptureFromAVI(argv[optind]);
    while(cvGrabFrame(capture)) {
        cur_img = cvQueryFrame(capture);
        if(!cur_img) {
            cerr << "Cannot grab a frame" << endl;
            break;
        }

        cur_img_hsv = cvCreateImage(cvSize(width, height), depth, channels);
        cvCvtColor(cur_img, cur_img_hsv, CV_BGR2HSV);
        uchar *data_img_hsv = (uchar *)cur_img_hsv->imageData;
        
        res_img = cvCreateImage(cvSize(width, height), depth, channels);
        RgbImage rgb_img_wrapper(res_img);
    
        bck_sub_img = subtract_img(cur_img, bck_img, th, ap);

        cvMorphologyEx(bck_sub_img, bck_sub_img, NULL, NULL, CV_MOP_OPEN, 1);
        cvMorphologyEx(bck_sub_img, bck_sub_img, NULL, NULL, CV_MOP_CLOSE, 1);

        BwImage mask_img_wrapper(bck_sub_img);
        for(int i = 0; i < height; i++) {
            for(int j = 0; j < width; j++) {
                if(mask_img_wrapper[i][j] > 0) {
                    int h_diff = (2 * data_img_hsv[i * step + j * channels + 0]) -
                                 (2 * data_bck_img_hsv[i * step + j * channels + 0]);
                    int s_diff = (data_img_hsv[i * step + j * channels + 1]) -
                                 (data_bck_img_hsv[i * step + j * channels + 1]);
                    int v_diff = (data_bck_img_hsv[i * step + j * channels + 2]) -
                                 (data_img_hsv[i * step + j * channels + 2]);

                    if(h_diff > 180) {
                        h_diff = 360 - h_diff;
                    }
                    else if(h_diff < -180) {
                        h_diff = -360 + h_diff;
                    }
                    
                    float fg_prob = pdf_gaussian_value((float) h_diff, fg_mu_h, fg_std_h) * 
                                    pdf_gaussian_value((float) s_diff, fg_mu_s, fg_std_s) * 
                                    pdf_gaussian_value((float) v_diff, fg_mu_v, fg_std_v);

                    float sh_prob = pdf_gaussian_value((float) h_diff, sh_mu_h, sh_std_h) * 
                                    pdf_gaussian_value((float) s_diff, sh_mu_s, sh_std_s) * 
                                    pdf_gaussian_value((float) v_diff, sh_mu_v, sh_std_v);

                    if(fg_prob >= sh_prob) {
                        // Object pixels as green
                        rgb_img_wrapper[i][j].g = 255;
                    }
                    else {
                        // Shadow pixels as red
                        rgb_img_wrapper[i][j].r = 255;
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

