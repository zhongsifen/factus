// proc.cpp

#include "_factus.h"

int alpha_h(
		Mat& base, Point& pt_base, 
		double& alpha)
{
	Rect rt_base(Point(-2,-2), Size(5,5));
	double val;

	Mat bk_base = base(rt_base + pt_base);
	val = norm(bk_base - mean(bk_base), NORM_L2);
	val = val/bk_base.total();

	alpha = val;

	return 0;
}

int alpha_g(
		Mat& base, Point& pt_base, 
		double& alpha)
{
	const int u1 = 8;
	const int u2 = 8;
	Rect rt_base(Point(-u1,-u2), Size(u1*2,u2*2));
	double val;

	Mat bk_base = base(rt_base + pt_base);
	val = norm(bk_base - mean(bk_base), NORM_L2);
	val = val/bk_base.total();

	alpha = val;

	return 0;
}

int beta_h(
		Mat& curr, Point& pt_curr, 
		Mat& base, Point& pt_base,
		Point& delta, double& beta)
{
	Rect rt_curr(Point(-4,-4), Size(9,9)); 
	Rect rt_base(Point(-2,-2), Size(5,5));
	Mat bk_curr = curr(rt_curr + pt_curr);
	Mat bk_base = base(rt_base + pt_base);
	Mat result(bk_curr.size() - bk_base.size() + Size(1,1), CV_32FC1);
	double minVal, maxVal; 
	Point  minLoc, maxLoc;
	Point loc;
	double val;

	matchTemplate(bk_curr, bk_base, result, TM_SQDIFF);
	minMaxLoc(result, &minVal, &maxVal, &minLoc, &maxLoc);

	loc = minLoc;
	val = minVal;

	delta = loc + rt_curr.tl() - rt_base.tl();
	beta = sqrt(val)/rt_base.area();
	if (beta > 8) return -1;
	if (delta.x < -1) return -1;
	if (delta.x > +1) return -1;
	if (delta.y < -1) return -1;
	if (delta.y > +1) return -1;

	return 0;
}

int beta_g(
		Mat& curr, Point& pt_curr, 
		Mat& base, Point& pt_base,
		Point& delta, double& beta, Point2f& epsilon)
{
	const int u1 = 8;
	const int u2 = 8;
	const int v1 = 6;
	const int v2 = 6;
	const int w1 = u1+v1;
	const int w2 = u2+v2;
	Rect rt_base(Point(-u1,-u2), Size(u1*2,u2*2));
	Rect rt_curr(Point(-w1,-w2), Size(w1*2,w2*2)); 
	Mat result(curr.size() - base.size() + Size(1,1), CV_32FC1);
	double minVal, maxVal; 
	Point  minLoc, maxLoc;
	Point loc;
	double val;

	matchTemplate(curr(rt_curr + pt_curr), base(rt_base + pt_base), result, TM_SQDIFF);
	minMaxLoc(result, &minVal, &maxVal, &minLoc, &maxLoc);

	loc = minLoc;
	val = minVal;

	delta = loc + rt_curr.tl() - rt_base.tl();
	beta = sqrt(val)/rt_base.area();
	if (beta > 2.0) return -1;
	if (delta.x <= -v1) return -1;
	if (delta.x >= +v1) return -1;
	if (delta.y <= -v2) return -1;
	if (delta.y >= +v2) return -1;

	int x = loc.x;
	int y = loc.y;
	float f_OO = result.at<float>(y  ,x  );
	float f_OP = result.at<float>(y  ,x+1);
	float f_ON = result.at<float>(y  ,x-1);
	float f_PO = result.at<float>(y+1,x  );
	float f_NO = result.at<float>(y-1,x  );

	float f_xx = (f_OP + f_ON - f_OO*2);
	float f_yy = (f_PO + f_NO - f_OO*2);

	if (f_xx < 1.0e-06F || f_yy < 1.0e-06F) return -1;

	epsilon = Point2f(
		-0.5F*(f_OP - f_ON) / f_xx,
		-0.5F*(f_PO - f_NO) / f_yy
	);

	return 0;
}
