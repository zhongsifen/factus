// factus_core.h

#ifndef _factus_core_h_
#define _factus_core_h_ 1

#include "factus.h"
#include "_keyblock.h"
#include "_stat.h"
#include "board/_board.h"

#include "opencv2/core/core.hpp"
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/imgproc/imgproc.hpp"
using namespace cv;

#include <stdint.h>
using namespace std;

typedef struct {
	int l1, l2;
	int m1, m2;

	Mat base_g;
	Mat base_h;
	Mat curr_g;
	Mat curr_h;
	Mat prev_g;
	Mat prev_h;

	Point* pt_base_g;
	Point* pt_base_h;
	Point* pt_curr_g;
	Point* pt_curr_h;
	Point* pt_prev_g;
	Point* pt_prev_h;

	bool* pt_valid;
	int pt_count;

	stat_t* stat;
	KeyPoint_Stat* pt_stat;
	int stat_count;
	int stat_thres;

	double* pt_alpha;
	double* pt_beta;
	Point2f* pt_epsilon;

	Point2f m_u[100];
	Point2f m_v[100];
	int m_index[100];
	int m_count;
}
factus_t;

typedef struct {
	factus_t* factus;
	board_t* board;
	int count;
}
factus_app_t;

int alpha_h(
		Mat& base, Point& pt_base, 
		double& alpha)
;
int alpha_g(
		Mat& base, Point& pt_base, 
		double& alpha)
;
int beta_h(
		Mat& curr, Point& pt_curr, 
		Mat& base, Point& pt_base,
		Point& loc, double& val)
;
int beta_g(
		Mat& curr, Point& pt_curr, 
		Mat& base, Point& pt_base,
		Point& loc, double& val, Point2f& sub)
;
#endif // _factus_core_h_
