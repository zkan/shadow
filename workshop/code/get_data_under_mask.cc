#include "shadow_utils.h"

IplImage *bckImg = 0;
IplImage *currImg = 0;
IplImage *maskImg = 0;
IplImage *bckImgHSV = 0;
IplImage *currImgHSV = 0;

float H_diff_mean = 0;
float H_diff_std = 0;
float S_diff_mean = 0;
float S_diff_std = 0;
float V_diff_mean = 0;
float V_diff_std = 0;
int num_pixels_under_mask = 0;

/*
void on_mouse( int event, int x, int y, int flags, void* param ) {
    switch( event ) {
        case CV_EVENT_LBUTTONDOWN: {
            int height = currImg->height;
            int width = currImg->width;
            int step = currImg->widthStep/sizeof( uchar );
            int channels = currImg->nChannels;
            uchar *dataImg = ( uchar * )currImgHSV->imageData;
            uchar *dataBckImg = ( uchar * )bckImgHSV->imageData;
			
            printf("Click at (%d, %d)\n", x, y);
        
            int i = y;
            int j = x;
            cout << "H_B at (" << x << ", " << y << "): " 
                 << 2 * dataBckImg[i * step + j * channels + 0] << endl;
            cout << "S_B at (" << x << ", " << y << "): " 
                 << dataBckImg[i * step + j * channels + 1] << endl;
            cout << "V_B at (" << x << ", " << y << "): " 
                 << dataBckImg[i * step + j * channels + 2] << endl;
            cout << "---------------------" << endl;
            
            printf("H_T at (%d, %d): %d\n", x, y, 2 * dataImg[i * step + j * channels + 0]);
            printf("S_T at (%d, %d): %d\n", x, y, dataImg[i * step + j * channels + 1]);
            printf("V_T at (%d, %d): %d\n", x, y, dataImg[i * step + j * channels + 2]);
            cout << "---------------------" << endl;
          
            int H_diff = (2 * dataImg[i * step + j * channels + 0]) - 
                         (2 * dataBckImg[i * step + j * channels + 0]);
            int S_diff = (dataImg[i * step + j * channels + 1]) - 
                         (dataBckImg[i * step + j * channels + 1]);
            int V_diff = (dataBckImg[i * step + j * channels + 2]) - 
                         (dataImg[i * step + j * channels + 2]);
                         
            if( H_diff > 180 ) {
                H_diff = 360 - H_diff;
            }
            else if(  H_diff < -180) {
                H_diff = -360 - H_diff;
            }
                         
            printf("H diff: %d\n", H_diff);
            printf("S diff: %d\n", S_diff);
            printf("V diff: %d\n", V_diff);
            cout << "---------------------" << endl << endl;
        }
        break;
    }
}
//*/

int main(int argc, char **argv) {
    int c;
    extern char *optarg;
    extern int optind, opterr, optopt;
	
    if(argc != 4) {
        cout << "Usage error" << endl;
        cout << "Usage: get-value-under-mask <bck> <current> <mask> " << endl;
        return 0;
    }

    if((bckImg = cvLoadImage( argv[argc - 3], -1)) == 0) {
        cerr << "Cannot load a background image" << endl;
        exit(1);
    }	
    if((currImg = cvLoadImage( argv[argc - 2], -1)) == 0) {
        cerr << "Cannot load a current image" << endl;
        exit(1);
    }	
    if((maskImg = cvLoadImage( argv[argc - 1], 0)) == 0) {
        cerr << "Cannot load a mask image" << endl;
        exit(1);
    }
	
    int width = currImg->width;
    int height = currImg->height;
    int step = currImg->widthStep/sizeof(uchar);
    int depth = currImg->depth;
    int channels = currImg->nChannels;
	
    bckImgHSV = cvCreateImage(cvSize(width, height), depth, channels);
    cvCvtColor(bckImg, bckImgHSV, CV_BGR2HSV);	
    currImgHSV = cvCreateImage(cvSize(width, height), depth, channels);
    cvCvtColor(currImg, currImgHSV, CV_BGR2HSV);
 
    uchar *dataImg = (uchar *)currImgHSV->imageData;
    uchar *dataBckImg = (uchar *)bckImgHSV->imageData;
    BwImage maskImgWrapper(maskImg);

    // Perform connected components (1st time flooding to make sure 
    // that all of the pixel values are the same)
    vector<CvConnectedComp> vcomp;
    CvPoint xy;
    int MIN_REGION_SIZE = 100;
    
    int center_x_pos = 0;
    int center_y_pos = 0;
    for( int i = 0; i < vcomp.size(); i++ ) {
        center_x_pos += vcomp[i].rect.x + vcomp[i].rect.width/2;
        center_y_pos += vcomp[i].rect.y + vcomp[i].rect.height/2;
    }

    // Print out the information (x, y, diff_h, diff_s, diff_v) at each pixel
    for(int y = 0; y < height; ++y) {
        for(int x = 0; x < width; ++x) {
            if( maskImgWrapper[y][x] > 0 ) {
                int H_diff = (2 * dataImg[y * step + x * channels + 0]) - 
    	                     (2 * dataBckImg[y * step + x * channels + 0]);
                int S_diff = (dataImg[y * step + x * channels + 1]) - 
	                         (dataBckImg[y * step + x * channels + 1]);
	            int V_diff = (dataBckImg[y * step + x * channels + 2]) - 
                             (dataImg[y * step + x * channels + 2]);
				
    	        if( H_diff > 180 ) {
	                H_diff = 360 - H_diff;
                }
	            else if( H_diff < -180) {
	                H_diff = -360 - H_diff;
                }
                cout << x << "\t" << y 
                     << "\t" << H_diff << "\t" << S_diff << "\t" << V_diff 
                     << endl;
            }
        }
    }

/*
    // Draw the rectangles and the circle as the center of mass
    for( int i = 0; i < vcomp.size(); i++ ) {
        CvFont font;
        double hScale = 1;
        double vScale = 1;
        int lineWidth = 2;
    
        char text[50];
        sprintf(text, "%d", i);
        cvInitFont(&font, CV_FONT_HERSHEY_PLAIN, hScale, vScale, 0, lineWidth);

        cvRectangle(currImg, cvPoint((int)(vcomp[i].rect.x + vcomp[i].rect.width/2) -
                                     (int)vcomp[i].rect.width/2, 
				                     (int)(vcomp[i].rect.y + vcomp[i].rect.height/2) - 
				                     (int)vcomp[i].rect.height/2), 
        		             cvPoint((int)(vcomp[i].rect.x + vcomp[i].rect.width/2) + 
			                         (int)vcomp[i].rect.width/2, 
			                         (int)(vcomp[i].rect.y + vcomp[i].rect.height/2) + 
			                         (int)vcomp[i].rect.height/2 ), cvScalar(0, 255, 0), 2);
        
        cvPutText(currImg, text, 
                  cvPoint((int)(vcomp[i].rect.x + vcomp[i].rect.width/2) + 
				          (int)vcomp[i].rect.width/4, 
				          (int)(vcomp[i].rect.y + vcomp[i].rect.height/2) - 
				          (int)vcomp[i].rect.height/2), &font, 
	              cvScalar(0, 0, 255));
        
        cvCircle(currImg, cvPoint(cvRound(center_x_pos/vcomp.size()), 
				                  cvRound(center_y_pos/vcomp.size())), 5, 
	             CV_RGB(0xff, 0x00, 0x00), 3);
    }
//*/

    ofstream hsv_file;
//    hsv_file.open("../data/working/data_file", ios::app);
    hsv_file.open("../data/working/data_file", ios::out);
    for(int y = 0; y < height; y++) {
        for(int x = 0; x < width; x++) {
            if(maskImgWrapper[y][x] > 0) {
	            num_pixels_under_mask++;
	            int H_diff = (2 * dataImg[y * step + x * channels + 0]) - 
                             (2 * dataBckImg[y * step + x * channels + 0]);
                int S_diff = (dataImg[y * step + x * channels + 1]) - 
                             (dataBckImg[y * step + x * channels + 1]);
	            int V_diff = (dataBckImg[y * step + x * channels + 2]) - 
                             (dataImg[y * step + x * channels + 2]);
	
                if( H_diff > 180 ) {
                    H_diff = 360 - H_diff;
                }
                else if( H_diff < -180) {
                    H_diff = -360 - H_diff;
                }
                hsv_file << x << "\t" << y << "\t" 
                         << H_diff << "\t" << S_diff << "\t" << V_diff 
                         << endl;
            }
        }
    }
    hsv_file.close();
		
//    cvSetMouseCallback( "Current Image", on_mouse, 0 );
//    cvSetMouseCallback( "Mask Image", on_mouse, 0 );
  
    return 1;
}

