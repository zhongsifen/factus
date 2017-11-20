// _show.h

#ifndef _show_h_
#define _show_h_ 1

#include "opencv2/core/core.hpp"
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/imgproc/imgproc.hpp"
using namespace cv;

#include <stdint.h>

int show_point(
		Mat& show,
		Point point[], int count)
;
int show_point_a(
		Mat& show,
		Point point[], int count)
;
int show_line(
		Mat& show,
		Point point[][2], int count)
;
int show_grid(
		Mat& show,
		Size grid)
;
int show_mb(
		Mat& show,
		Point point[], int count,
		Point offset)
;
#endif // _show_h_

