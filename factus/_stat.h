// _stat.h

#ifndef _stat_h_
#define _stat_h_ 1

#include <opencv2/features2d.hpp>
using namespace cv;

#include <stdint.h>
using namespace std;

typedef struct {
	int l;
	int v[25];
}
stat_t;

#endif // _stat_h_
