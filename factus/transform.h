// transform.h

#ifndef _TRANSFORM_H_
#define _TRANSFORM_H_ 1

#include <stdint.h>
#include <opencv2/opencv.hpp>
using namespace cv;


#define TRANSFORM_TRA 1
#define TRANSFORM_SIM 2
#define TRANSFORM_AFF 3
#define TRANSFORM_PER 4

void* transform_new()
;
void transform_delete(void* p)
;
int transform_set_type(void* p,
		int type)
;
int transform_set_pt(void* p,
		Point2f pt_src[],
		Point2f pt_dst[],
		int pt_count)
;
int transform_set_pt_nrm(void* p,
		Point2f pt_src[])
;
int transform_get(void* p)
;
int transform_app(void* p,
		InputArray src, OutputArray dst)
;

Mat getTraTransform(
		const Point2f pt_src[], const Point2f pt_dst[], int pt_count)
;
Mat getSimTransform( 
		const Point2f pt_src[], const Point2f pt_dst[], int pt_count)
;
Mat getAffTransform( 
		const Point2f pt_src[], const Point2f pt_dst[], int pt_count)
;
Mat getPerTransform( 
		const Point2f pt_src[], const Point2f pt_dst[], int pt_count)
;

#endif // _TRANSFORM_H_
