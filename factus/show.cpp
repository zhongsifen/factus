// show.cpp

#include "_show.h"

int show_point(
		Mat& show,
		Point point[], int count)
{
    for (int i=0; i<count; i++)
	{
		circle(show, point[i], 4, Scalar(0xFF,0xFF,0xFF));
	}

	return 0;
}

int show_point_a(
		Mat& show,
		Point point[], int count)
{
	for (int i=0; i<count; i++)
	{
		circle(show, point[i], 8, Scalar(0xFF,0xFF,0xFF));
	}

	return 0;
}

int show_line(
		Mat& show,
		Point point[][2], int count)
{
	for (int i=0; i<count; i++)
	{
		line(show, point[i][0], point[i][1], Scalar(0xFF,0xFF,0xFF));
	}

	return 0;
}

int show_grid(
		Mat& show,
		Size grid)
{
	int l1 = show.cols;
	int l2 = show.rows;
	int g1 = grid.width;
	int g2 = grid.height;

	for (int i1=g1; i1<l1; i1+=g1)
	{
		line(show, Point(i1,  0), Point(i1, l2), Scalar(0xFF,0xFF,0xFF));
	}

	for (int i2=g2; i2<l2; i2+=g2)
	{
		line(show, Point( 0, i2), Point(l1, i2), Scalar(0xFF,0xFF,0xFF));
	}

	return 0;
}

int show_mb(
		Mat& show,
		Point point[], int count,
		Point offset)
{
	for (int i=0; i<count; i++)
	{
		circle(show, point[i] + offset, 4, Scalar(0xFF,0xFF,0xFF));
	}

	return 0;
}
