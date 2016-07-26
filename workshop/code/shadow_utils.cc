#include "shadow_utils.h"

// Basic frame-by-frame subtraction.
IplImage* subtract_img(IplImage *img, IplImage *img_back, 
                      double threshold, double ap) {
	CvScalar s, s_tmp, s_back;
	int height = img->height;
	int width = img->width;

	IplImage* bImage = cvCreateImage(cvSize(width, height), IPL_DEPTH_8U, 1);
	for(int i = 0; i < height; i++) {
		for(int j = 0; j < width; j++) {
			s = cvGet2D( img, i, j );
			s_back = cvGet2D( img_back, i, j );
			if((fabs(s.val[0] - s_back.val[0]) >= threshold) || 
               (fabs(s.val[1] - s_back.val[1]) >= threshold) || 
               (fabs(s.val[2] - s_back.val[2]) >= threshold)) {

				// Set the pixel's value to 255 (white).
			    s_tmp = cvGet2D(bImage, i, j);
			    s_tmp.val[0] = 255;
			    cvSet2D(bImage, i, j, s_tmp);
			}
    		else {
    			// Update the background.
    			s_back.val[0] = (ap * s_back.val[0]) + ((1 - ap) * s.val[0]);
    			s_back.val[1] = (ap * s_back.val[1]) + ((1 - ap) * s.val[1]);
    			s_back.val[2] = (ap * s_back.val[2]) + ((1 - ap) * s.val[2]);
    			cvSet2D( img_back, i, j, s_back );
				
				// Set the pixel's value to 0 (black).
				s_tmp = cvGet2D(bImage, i, j);
				s_tmp.val[0] = 0;
				cvSet2D(bImage, i, j, s_tmp);
		    }
	    }
    }

    return bImage;
}

float cdf_gaussian_value(float x, float mean, float std) {
    //cout << erf( ( x - mean )/( std * sqrt( 2 ) ) ) << endl;
    float prob;
    if( ( x >= ( mean - std ) ) && ( x <= ( mean + std ) ) ) {
        prob = 0.5 * ( 1 + erf( ( x - mean )/( std * sqrt( 2 ) ) ) );
        if( prob == 0 ) {
            return 0.00001;
        }
        else {
            return prob;
        }
    }
    else {
        return 0.00001;
    }
}

float pdf_gaussian_value(float x, float mean, float std) {
    //cout << pow( x - mean, 2 )/( 2 * pow( std, 2 ) ) << endl;
    return ( 1/( std * sqrt( 2 * PI ) ) ) * exp( -( pow( x - mean, 2 ) )/( 2 * pow( std, 2 ) ) );
}


float pdf_gaussian_value2(float x, float y, 
                          float mean1, float mean2, 
                          float std1, float std2) {
    return exp(-((pow(x - mean1, 2))/(2 * pow(std1, 2)) + 
                 (pow(y - mean2, 2))/(2 * pow(std2, 2))));
}

float pdf_exp_value(float x, float V_B, float lambda = 1) {
    printf( "%f %f %f\n", x, V_B, -( x - V_B ) );
    //return lambda * exp( -( x - V_B ) );
    return lambda * exp( -( x ) );
}

float get_z_score(float x, float mean, float std) {
    float z = (x - mean)/std;

    return z;
}

// credit: uvts_cvs's reply at
// http://stackoverflow.com/questions/392981/how-can-i-convert-string-to-double-in-c
double string_to_double(string str) {
    istringstream i(str);
    double val;

    if(!(i >> val)) {
        return 0;
    }
    
    return val;
}

// credit: heeen's reply at 
// http://stackoverflow.com/questions/599989/is-there-a-built-in-way-to-split-strings-in-c
vector<string> split(string str, string delimiters) {
    vector<string> tokens;
    
    // skip delimiters at beginning
    string::size_type last_pos = str.find_first_not_of(delimiters, 0);
    // find first "non-delimiter"
    string::size_type pos = str.find_first_of(delimiters, last_pos);

    // npos is a static member constant value (-1) with the greatest 
    // possible value for an element of type size_t.
    // credit: http://www.cplusplus.com/reference/string/string/npos/
    while(string::npos != pos || string::npos != last_pos) {
        // found a token, add it to the vector
        tokens.push_back(str.substr(last_pos, pos - last_pos));
        // skip delimiters
        last_pos = str.find_first_not_of(delimiters, pos);
        // find next "non-delimiter"
        pos = str.find_first_of(delimiters, last_pos);
    }

    return tokens;
}


