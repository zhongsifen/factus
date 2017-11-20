// transform_core.h

#ifndef _TRANSFORM_CORE_H_
#define _TRANSFORM_CORE_H_ 1

#include "transform.h"

typedef void transform_app_t ( 
		InputArray src, OutputArray dst,
		InputArray M, Size dsize,
		int flags,						// =INTER_LINEAR,
		int borderMode,					// =BORDER_CONSTANT,
		const Scalar& borderValue		// =Scalar()
		)
;
typedef Mat transform_get_t( 
		const Point2f pt_src[], const Point2f pt_dst[], int pt_count)
;

typedef struct {
	int type;
	transform_app_t* app;
	transform_get_t* get;
	Mat op;
	Point2f pt_src[64];
	Point2f pt_dst[64];
	int pt_count;
}
transform_t;

#endif // _TRANSFORM_CORE_H_
