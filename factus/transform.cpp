// transform.cpp

#include "_transform.h"

void* transform_new()
{
	transform_t* hnd = new transform_t;

	return (void*)hnd;
}

void transform_delete(void* p)
{
	transform_t* hnd = (transform_t*)p;
	delete(hnd);
}

int transform_set_type(void* p,
		int type)
{
	transform_t* hnd = (transform_t*)p;

	switch (type)
	{
	case TRANSFORM_TRA:
		{
			hnd->type = TRANSFORM_TRA;
			hnd->app = warpAffine;
			hnd->get = getTraTransform;
		}
		break;
	case TRANSFORM_SIM:
		{
			hnd->type = TRANSFORM_SIM;
			hnd->app = warpAffine;
			hnd->get = getSimTransform;
		}
		break;
	case TRANSFORM_AFF:
		{
			hnd->type = TRANSFORM_AFF;
			hnd->app = warpAffine;
			hnd->get = getAffTransform;
		}
		break;
	case TRANSFORM_PER:
		{
			hnd->type = TRANSFORM_PER;
			hnd->app = warpPerspective;
			hnd->get = getPerTransform;
		}
		break;
	default:
		{
			hnd->type = TRANSFORM_PER;
			hnd->app = warpPerspective;
			hnd->get = getPerTransform;
		}
		break;
	}

	return 0;
}

int transform_set_pt(void* p,
		Point2f pt_src[],
		Point2f pt_dst[],
		int pt_count)
{
	transform_t* hnd = (transform_t*)p;

	for (int k=0; k<pt_count; k++)
	{
		hnd->pt_src[k] = pt_src[k];
		hnd->pt_dst[k] = pt_dst[k];
	}
	hnd->pt_count = pt_count;

	return 0;
}

int transform_get(void* p)
{
	transform_t* hnd = (transform_t*)p;

	hnd->op = hnd->get(hnd->pt_src, hnd->pt_dst, hnd->pt_count);

	return 0;
}

int transform_app(void* p,
		InputArray src, OutputArray dst)
{
	transform_t* hnd = (transform_t*)p;

	hnd->app(src, dst, hnd->op, src.size(), INTER_LINEAR, BORDER_CONSTANT, Scalar());

	return 0;
}

int transform_set_pt_nrm(void* p,
		Point2f pt_src[])
{
	transform_t* hnd = (transform_t*)p;

	float lft = pt_src[0].x;
	float top = pt_src[0].y;
	float d1_x = pt_src[1].x - lft;
	float d1_y = pt_src[1].y - top;
	float d2_x = pt_src[2].x - lft;
	float d2_y = pt_src[2].y - top;
	float rgt = lft + sqrtf(d1_x*d1_x + d1_y*d1_y);
	float btm = top + sqrtf(d2_x*d2_x + d2_y*d2_y);

	Point2f pt_nrm[4] = {
		Point2f(lft, top),
		Point2f(rgt, top),
		Point2f(lft, btm),
		Point2f(rgt, btm),
	};

	transform_set_pt(hnd, pt_src, pt_nrm, 4);

	return 0;
}


Mat getSimTransform( 
		const Point2f pt_src[], const Point2f pt_dst[], int pt_count)
{
	Point2f s = pt_src[1] - pt_src[0];
	Point2f d = pt_dst[1] - pt_dst[0];
	double s_norm = norm(s);
	double d_norm = norm(d);
	double alpha = d_norm / s_norm;

	double cs = (Mat(s).dot(Mat(d)))/(s_norm*d_norm);
	double sn = sqrt(1 - cs*cs);

	cs *= alpha;
	sn *= alpha;

	Point2f s0 = pt_src[0];
	Point2f d0 = pt_dst[0];

	Point2f b = d0 - Point2f((float)(+cs*s0.x +sn*s0.y), (float)(-sn*s0.x +cs*s0.y));

	Matx23d h(
		+cs, -sn, b.x,
		+sn, +cs, b.y);

	return Mat(h);
}

Mat getTraTransform( 
		const Point2f pt_src[], const Point2f pt_dst[], int pt_count)
{
	Point2f s0 = pt_src[0];
	Point2f d0 = pt_dst[0];

	Point2f b = d0 - s0;

	Matx23d h(
		1, 0, b.x,
		0, 1, b.y);

	return Mat(h);
}

Mat getAffTransform( 
		const Point2f pt_src[], const Point2f pt_dst[], int pt_count)
{
    Mat M(2, 3, CV_64F), X(6, 1, CV_64F, M.data);
    double a[6*6], b[6];
    Mat A(6, 6, CV_64F, a), B(6, 1, CV_64F, b);

    for( int i = 0; i < 3; i++ )
    {
        int j = i*12;
        int k = i*12+6;
        a[j] = a[k+3] = pt_src[i].x;
        a[j+1] = a[k+4] = pt_src[i].y;
        a[j+2] = a[k+5] = 1;
        a[j+3] = a[j+4] = a[j+5] = 0;
        a[k] = a[k+1] = a[k+2] = 0;
        b[i*2] = pt_dst[i].x;
        b[i*2+1] = pt_dst[i].y;
    }

    solve( A, B, X );
    return M;
}

Mat getPerTransform( 
		const Point2f pt_src[], const Point2f pt_dst[], int pt_count)
{
	int l = pt_count*2;
	Mat M(3, 3, CV_64F), X(8, 1, CV_64F, M.data);
	Mat A(l, 8, CV_64F), B(l, 1, CV_64F);

	double **a = new double*[l];
	double **b = new double*[l];
	for( int k=0; k<l; k++)
	{
		a[k] = A.ptr<double>(k);
		b[k] = B.ptr<double>(k);
	}

    for( int i=0,k0=0,k1=1; i<pt_count; i++,k0+=2,k1+=2 )
    {
        a[k0][0] = a[k1][3] = pt_src[i].x;
        a[k0][1] = a[k1][4] = pt_src[i].y;
        a[k0][2] = a[k1][5] = 1;
        a[k0][3] = a[k0][4] = a[k0][5] =
        a[k1][0] = a[k1][1] = a[k1][2] = 0;
        a[k0][6] = - pt_src[i].x * pt_dst[i].x;
        a[k0][7] = - pt_src[i].y * pt_dst[i].x;
        a[k1][6] = - pt_src[i].x * pt_dst[i].y;
        a[k1][7] = - pt_src[i].y * pt_dst[i].y;
        b[k0][0] = pt_dst[i].x;
        b[k1][0] = pt_dst[i].y;
    }

    solve( A.t()*A, A.t()*B, X, DECOMP_SVD );
    (( double*)M.data)[8] = 1.;

    return M;
}

#if 0
/* Calculates coefficients of perspective transformation
 * which maps (xi,yi) to (ui,vi), (i=1,2,3,4):
 *
 *      c00*xi + c01*yi + c02
 * ui = ---------------------
 *      c20*xi + c21*yi + c22
 *
 *      c10*xi + c11*yi + c12
 * vi = ---------------------
 *      c20*xi + c21*yi + c22
 *
 * Coefficients are calculated by solving linear system:
 * / x0 y0  1  0  0  0 -x0*u0 -y0*u0 \ /c00\ /u0\
 * | x1 y1  1  0  0  0 -x1*u1 -y1*u1 | |c01| |u1|
 * | x2 y2  1  0  0  0 -x2*u2 -y2*u2 | |c02| |u2|
 * | x3 y3  1  0  0  0 -x3*u3 -y3*u3 |.|c10|=|u3|,
 * |  0  0  0 x0 y0  1 -x0*v0 -y0*v0 | |c11| |v0|
 * |  0  0  0 x1 y1  1 -x1*v1 -y1*v1 | |c12| |v1|
 * |  0  0  0 x2 y2  1 -x2*v2 -y2*v2 | |c20| |v2|
 * \  0  0  0 x3 y3  1 -x3*v3 -y3*v3 / \c21/ \v3/
 *
 * where:
 *   cij - matrix coefficients, c22 = 1
 */
cv::Mat cv::getPerspectiveTransform( const Point2f src[], const Point2f dst[] )
{
    Mat M(3, 3, CV_64F), X(8, 1, CV_64F, M.data);
    double a[8][8], b[8];
    Mat A(8, 8, CV_64F, a), B(8, 1, CV_64F, b);

    for( int i = 0; i < 4; ++i )
    {
        a[i][0] = a[i+4][3] = src[i].x;
        a[i][1] = a[i+4][4] = src[i].y;
        a[i][2] = a[i+4][5] = 1;
        a[i][3] = a[i][4] = a[i][5] =
        a[i+4][0] = a[i+4][1] = a[i+4][2] = 0;
        a[i][6] = -src[i].x*dst[i].x;
        a[i][7] = -src[i].y*dst[i].x;
        a[i+4][6] = -src[i].x*dst[i].y;
        a[i+4][7] = -src[i].y*dst[i].y;
        b[i] = dst[i].x;
        b[i+4] = dst[i].y;
    }

    solve( A, B, X, DECOMP_SVD );
    ((double*)M.data)[8] = 1.;

    return M;
}

/* Calculates coefficients of affine transformation
 * which maps (xi,yi) to (ui,vi), (i=1,2,3):
 *
 * ui = c00*xi + c01*yi + c02
 *
 * vi = c10*xi + c11*yi + c12
 *
 * Coefficients are calculated by solving linear system:
 * / x0 y0  1  0  0  0 \ /c00\ /u0\
 * | x1 y1  1  0  0  0 | |c01| |u1|
 * | x2 y2  1  0  0  0 | |c02| |u2|
 * |  0  0  0 x0 y0  1 | |c10| |v0|
 * |  0  0  0 x1 y1  1 | |c11| |v1|
 * \  0  0  0 x2 y2  1 / |c12| |v2|
 *
 * where:
 *   cij - matrix coefficients
 */

cv::Mat cv::getAffineTransform( const Point2f src[], const Point2f dst[] )
{
    Mat M(2, 3, CV_64F), X(6, 1, CV_64F, M.data);
    double a[6*6], b[6];
    Mat A(6, 6, CV_64F, a), B(6, 1, CV_64F, b);

    for( int i = 0; i < 3; i++ )
    {
        int j = i*12;
        int k = i*12+6;
        a[j] = a[k+3] = src[i].x;
        a[j+1] = a[k+4] = src[i].y;
        a[j+2] = a[k+5] = 1;
        a[j+3] = a[j+4] = a[j+5] = 0;
        a[k] = a[k+1] = a[k+2] = 0;
        b[i*2] = dst[i].x;
        b[i*2+1] = dst[i].y;
    }

    solve( A, B, X );
    return M;
}

void cv::invertAffineTransform(InputArray _matM, OutputArray __iM)
{
    Mat matM = _matM.getMat();
    CV_Assert(matM.rows == 2 && matM.cols == 3);
    __iM.create(2, 3, matM.type());
    Mat _iM = __iM.getMat();

    if( matM.type() == CV_32F )
    {
        const float* M = (const float*)matM.data;
        float* iM = (float*)_iM.data;
        int step = (int)(matM.step/sizeof(M[0])), istep = (int)(_iM.step/sizeof(iM[0]));

        double D = M[0]*M[step+1] - M[1]*M[step];
        D = D != 0 ? 1./D : 0;
        double A11 = M[step+1]*D, A22 = M[0]*D, A12 = -M[1]*D, A21 = -M[step]*D;
        double b1 = -A11*M[2] - A12*M[step+2];
        double b2 = -A21*M[2] - A22*M[step+2];

        iM[0] = (float)A11; iM[1] = (float)A12; iM[2] = (float)b1;
        iM[istep] = (float)A21; iM[istep+1] = (float)A22; iM[istep+2] = (float)b2;
    }
    else if( matM.type() == CV_64F )
    {
        const double* M = (const double*)matM.data;
        double* iM = (double*)_iM.data;
        int step = (int)(matM.step/sizeof(M[0])), istep = (int)(_iM.step/sizeof(iM[0]));

        double D = M[0]*M[step+1] - M[1]*M[step];
        D = D != 0 ? 1./D : 0;
        double A11 = M[step+1]*D, A22 = M[0]*D, A12 = -M[1]*D, A21 = -M[step]*D;
        double b1 = -A11*M[2] - A12*M[step+2];
        double b2 = -A21*M[2] - A22*M[step+2];

        iM[0] = A11; iM[1] = A12; iM[2] = b1;
        iM[istep] = A21; iM[istep+1] = A22; iM[istep+2] = b2;
    }
    else
        CV_Error( CV_StsUnsupportedFormat, "" );
}

cv::Mat cv::getPerspectiveTransform(InputArray _src, InputArray _dst)
{
    Mat src = _src.getMat(), dst = _dst.getMat();
    CV_Assert(src.checkVector(2, CV_32F) == 4 && dst.checkVector(2, CV_32F) == 4);
    return getPerspectiveTransform((const Point2f*)src.data, (const Point2f*)dst.data);
}

cv::Mat cv::getAffineTransform(InputArray _src, InputArray _dst)
{
    Mat src = _src.getMat(), dst = _dst.getMat();
    CV_Assert(src.checkVector(2, CV_32F) == 3 && dst.checkVector(2, CV_32F) == 3);
    return getAffineTransform((const Point2f*)src.data, (const Point2f*)dst.data);
}
#endif