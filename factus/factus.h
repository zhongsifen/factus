// factus.h

#ifndef _factus_h_
#define _factus_h_ 1

#include "opencv2/core/core.hpp"
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/imgproc/imgproc.hpp"
using namespace cv;

#include <stdint.h>


void* factus_new()
;
void factus_delete(void* p)
;
int factus_open(void* p,
		Mat& g, Mat& h)
;
int factus_close(void* p)
;
int factus_load(void* p,
		Mat& g, Mat& h)
;
int factus_save(void* p,
		void* f)
;
int factus_proc_transform(void* p,
		Mat& frame_inp,
		Mat& frame_out)
;

int factus_proc_alpha(void* p)
;
int factus_proc_beta(void* p)
;
int factus_show_base(void* p)
;
int factus_show_curr(void* p)
;

void* factus_app_new()
;
void factus_app_delete(void* p)
;
void factus_app_set(void* p,
		void* factus,
		void* board,
		int count)
;
void factus_set_stat(void* p,
		int stat_count,
		int stat_thres)
;
int factus_app_0(void* p)
;
int factus_app_1(void* p)
;
int factus_app_2(void* p,
		int count)
;

#endif // _factus_h_
