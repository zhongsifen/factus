//keyblock.cpp

#include "_keyblock.h"

int keyblock_detect(
		Mat& frame, Rect& r,
		int maxCorners, double minDistance,
		vector<KeyPoint>& KeyPt)
{
	//CV_WRAP GFTTDetector( int maxCorners=1000, double qualityLevel=0.01, double minDistance=1,
	//                      int blockSize=3, bool useHarrisDetector=false, double k=0.04 );
//	GFTTDetector detector(maxCorners, 0.01, minDistance, 3, true, 0.04);
//	detector.detect(frame(r), KeyPt);
	int threshold = 0;
	FAST(frame(r), KeyPt, threshold);
	
	Point tl = r.tl();
	for (vector<KeyPoint>::iterator p = KeyPt.begin(); p != KeyPt.end(); p++)
	{
		p->pt += Point2f(tl);
	}

	return 0;
}

int keyblock_show(
		Mat& frame,
		vector<KeyPoint>& KeyPt)
{
	drawKeypoints(frame, KeyPt, frame, Scalar(255,255,255), DrawMatchesFlags::DRAW_OVER_OUTIMG);
	imshow("keyblock", frame);
	waitKey(0);

	return 0;
}
