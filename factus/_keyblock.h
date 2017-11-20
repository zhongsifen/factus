// _keyblock.h

#ifndef _keyblock_h_
#define _keyblock_h_ 1

#include "opencv2/highgui/highgui.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include <opencv2/features2d.hpp>
using namespace cv;

#include <stdint.h>
using namespace std;

typedef struct {
	int l;
	double v;
}
KeyPoint_Stat;

int keyblock_detect(
		Mat& frame, Rect& r,
		int maxCorners, double minDistance,
		vector<KeyPoint>& KeyPt)
;
int keyblock_show(
		Mat& image,
		vector<KeyPoint>& point)
;
#endif // _keyblock_h_
