// factus.cpp

#include "_factus.h"
#include "_keyblock.h"
#include "_stat.h"
#include "_show.h"
#include "board/vide.h"
#include "board/proc.h"
#include "transform.h"

#include <iostream>
using namespace std;

//#undef  MANUAL
#define MANUAL 1

void* factus_new()
{
	factus_t* factus = new factus_t;

	return (void*)factus;
}

void factus_delete(void* p)
{
	factus_t* factus = (factus_t*)p;
	delete factus;
}

void factus_init(void* p,
		int n)
{
	factus_t* factus = (factus_t*)p;

	factus->pt_base_h = new Point[n];
	factus->pt_curr_h = new Point[n];
	factus->pt_base_g = new Point[n];
	factus->pt_curr_g = new Point[n];

	factus->pt_stat  = new KeyPoint_Stat[n];
	factus->pt_valid = new bool[n];

	factus->pt_beta  = new double[n];
	factus->pt_alpha = new double[n];
	factus->pt_epsilon = new Point2f[n];

	factus->stat = new stat_t[n];
}

int factus_open(void* p,
		Mat& g, Mat& h)
{
	factus_t* factus = (factus_t*)p;

	factus->l1 = g.cols;
	factus->l2 = g.rows;
	factus->m1 = h.cols;
	factus->m2 = h.rows;

	g.copyTo(factus->base_g);
	h.copyTo(factus->base_h);

	return 0;
}

int factus_close(void* p)
{
	factus_t* factus = (factus_t*)p;
	
	delete factus;

	return 0;
}

int factus_load(void* p,
		Mat& g, Mat& h)
{
	factus_t* factus = (factus_t*)p;

	g.copyTo(factus->curr_g);
	h.copyTo(factus->curr_h);

	return 0;
}

int factus_show_base(void* p)
{
	factus_t* factus = (factus_t*)p;

	int n = factus->pt_count;
	Mat show_g = factus->base_g.clone();
	Point* u = new Point[n];
	int k = 0;
	for (int i=0; i<n; i++)
	{
		if (factus->pt_valid[i])
		{
			u[k] = factus->pt_base_g[i];
			k++;
		}
	}
	show_point_a(show_g, u, k);
	imshow("base_g", show_g);

	return 0;
}

int factus_show_curr(void* p)
{
	factus_t* factus = (factus_t*)p;

	int n = factus->pt_count;
	Mat show_g = factus->curr_g.clone();
	Point* u = new Point[n];
	int k = 0;
	for (int i=0; i<n; i++)
	{
		if (factus->pt_valid[i])
		{
			u[k] = factus->pt_curr_g[i];
			k++;
		}
	}
	show_point_a(show_g, u, k);
	imshow("curr_g", show_g);
	n = factus->m_count;
	k = 0;
	for (int i=0; i<n; i++)
	{
		u[k] = factus->pt_curr_g[factus->m_index[i]];
		k++;
	}
	show_point(show_g, u, k);
	imshow("curr_g", show_g);

	return 0;
}

int factus_proc_alpha(void* p)
{
	factus_t* factus = (factus_t*)p;

#if defined(MANUAL)
	Point pt_base[100];
	int count = 0;
	pt_base[count++] = Point( 80,  50);
	pt_base[count++] = Point( 36,  70);
	pt_base[count++] = Point(448,  50);
	pt_base[count++] = Point(340,  40);
	pt_base[count++] = Point( 64, 222);
	pt_base[count++] = Point( 44, 182);
	pt_base[count++] = Point(484, 208);
	pt_base[count++] = Point(332, 182);
	int n = count;
#else
	const int e = 10*4;
	Size s(factus->l1-e-e, factus->l2-e-e);
	Rect r(Point(e,e), s);
	vector<KeyPoint> pt_base_g;
	keyblock_detect(factus->base_g, r, 200, 32, pt_base_g);
	int n = (int)pt_base_g.size();
#endif
	factus_init(factus, n);
	int k=0;
	for (int i=0; i<n; i++)
	{
#if defined(MANUAL)
		Point pt = pt_base[i];
		factus->pt_base_g[k] =
		factus->pt_curr_g[k] = pt;
		factus->pt_valid[k] = true;
#else
		Point pt = Point(pt_base_g[i].pt);
		factus->pt_base_g[k] =
		factus->pt_curr_g[k] = pt;

		double alpha;
		alpha_g(factus->base_g, pt, alpha);
		factus->pt_alpha[k] = alpha;
		factus->pt_valid[k] = (alpha > 2.0);
#endif
		factus->pt_stat[k].l = 0;
		factus->pt_stat[k].v = 0;

 		k++;
	}
	factus->pt_count = k;

	return 0;
}

int factus_proc_beta(void* p)
{
	factus_t* factus = (factus_t*)p;

	int ret = 0;
	Point delta;
	double beta;
	Point2f epsilon;
	int n = factus->pt_count;
	int k = 0;
	for (int i=0; i<n; i++)
	{
		if (!factus->pt_valid[i]) continue;

		ret = beta_g(factus->curr_g, factus->pt_curr_g[i], factus->base_g, factus->pt_base_g[i], delta, beta, epsilon);
		if (ret < 0) continue;

		factus->pt_curr_g[i] += delta;
		factus->pt_beta[i] = beta;
		factus->pt_epsilon[i] = epsilon;

		factus->pt_stat[i].l++;
		factus->pt_stat[i].v += beta;

		factus->m_index[k++] = i;
	}
	factus->m_count = k;

	return 0;
}

int factus_proc_transform(void* p,
		Mat& frame_inp,
		Mat& frame_out)
{
	factus_t* factus = (factus_t*)p;

	for (int k=0; k<factus->m_count; k++)
	{
		int i = factus->m_index[k];

		factus->m_u[k] = Point2f(factus->pt_curr_g[i]) + factus->pt_epsilon[i];
		factus->m_v[k] = Point2f(factus->pt_base_g[i]);
	}

//	Mat transform = getSimTransform(factus->m_u, factus->m_v, factus->m_count);
	Mat transform = getPerTransform(factus->m_u, factus->m_v, factus->m_count);


//	warpAffine     (frame_inp, frame_out, transform, (frame_out).size());
	warpPerspective(frame_inp, frame_out, transform, (frame_out).size());

	return 0;
}

void factus_set_stat(void* p,
		int stat_count,
		int stat_thres)
{
	factus_t* factus = (factus_t*)p;

	factus->stat_count = stat_count;
	factus->stat_thres = stat_thres;
}

void* factus_app_new()
{
	factus_app_t* a = new factus_app_t;

	return (void*)a;
}

void factus_app_delete(void* p)
{
	factus_app_t* a = (factus_app_t*)p;
	delete a;
}

void factus_app_set(void* p,
		void* factus,
		void* board,
		int count)
{
	factus_app_t* a = (factus_app_t*)p;

	a->factus = (factus_t*)factus;
	a->board  = (board_t*)board;
	a->count  = count;
}

int factus_app_0(void* p)
{
	factus_app_t* a = (factus_app_t*)p;

	factus_t* factus = a->factus;
	board_t* vide = a->board;
	int count = a->count;
	
	const int stat_count = 25;
	const int stat_thres = stat_count*4/5;
	count -= stat_count;
	factus_set_stat(factus, stat_count, stat_thres);

	Mat frame_inp, frame_out;
	Mat f, g, h;
	vide_load(vide, frame_inp);
	proc_fgh(frame_inp, f, g, h);
	factus_open(factus, g, h);
	factus_proc_alpha(factus);
	a->count = count;

	return 0;
}

int factus_app_1(void* p)
{
	factus_app_t* a = (factus_app_t*)p;

	factus_t* factus = a->factus;
	board_t* vide = a->board;
	
	int count = factus->stat_count;
	int thres = factus->stat_thres;

	Mat frame_inp, frame_out;
	Mat f, g, h;
	for (int t=0; t<count; t++)
	{
		vide_load(vide, frame_inp);
		proc_fgh(frame_inp, f, g, h);
		factus_load(factus, g, h);
		factus_proc_beta(factus);
	}

	int n = factus->pt_count;
	bool* valid = factus->pt_valid;
	KeyPoint_Stat* stat = factus->pt_stat;
	for (int k=0; k<n; k++)
	{
		valid[k] = (stat[k].l > thres);
	}

	factus_show_base(factus);

	return 0;
}

int factus_app_2(void* p,
		int count)
{
	factus_app_t* a = (factus_app_t*)p;

	factus_t* factus = a->factus;
	board_t* vide = a->board;
	
	Mat frame_inp, frame_out;
	Mat f, g, h;
	VideoWriter vw;
	vw.open("/Users/zhongsifen/Data/tmp/factus_two.avi", CV_FOURCC('M', 'J', 'P', 'G'), 25, Size(512, 288*2));
	for (int t=0; t<count; t++)
	{
		vide_load(vide, frame_inp);
		proc_fgh(frame_inp, f, g, h);
		factus_load(factus, g, h);
		factus_proc_beta(factus);
//		factus_show_curr(factus);
		factus_proc_transform(factus, frame_inp, frame_out);
		
		int l1 = frame_inp.cols;
		int l2 = frame_inp.rows;
		Mat frame_two(Size(l1, l2*2), frame_inp.type());
		frame_inp.copyTo(frame_two(Rect(Point(0,  0), Size(l1, l2))));
		frame_out.copyTo(frame_two(Rect(Point(0, l2), Size(l1, l2))));
		imshow("factus_two", frame_two);
		vw.write(frame_two);
		
//		imshow("factus_inp", frame_inp);
//		imshow("factus_out", frame_out);
		waitKey(1);
	}
	vw.release();

	return 0;
}
