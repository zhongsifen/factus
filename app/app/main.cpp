//
//  main.cpp
//  app
//
//  Created by SIFEN ZHONG on 8/7/15.
//  Copyright (c) 2015 ___ZHONGSIFEN___. All rights reserved.
//

#include "factus/factus.h"
#include "board/vide.h"
#include <iostream>

#include <stdio.h>

int main(int argc, char* argv[])
{
	char name[256] = {"/Users/zhongsifen/Work/data/gym"};
	char ext[8] = {".flv"};

	int k=1;
	if (k < argc) sprintf(name, "%s", argv[k++]);
	if (k < argc) sprintf(ext,  "%s", argv[k++]);

	char filename_inp[256];
	char filename_ref[256];
	char filename_out[256];

	sprintf(filename_inp, "%s%s", name, ext);
	sprintf(filename_ref, "%s_ref%s", name, ".3gp");
	sprintf(filename_out, "%s_out%s", name, ".yuv");

	Mat frame_inp;
	
	bool ret = true;
	void* vide = vide_new();
	void* factus = factus_new();
	ret = vide_open_cap(vide, filename_inp);		if (!ret) return -1;
	//int count = 4227;
	int count = 627;
	// skip frames
	//for (int t=0; t<85; t++)
	for (int t=0; t<240; t++)
	{
		vide_load(vide, frame_inp);
		vide_show(vide, frame_inp);
		count--;
	}

	void* a = factus_app_new();
	factus_app_set(a, factus, vide, count);

	factus_app_0(a);
	factus_app_1(a);
	count -= 25;
	factus_app_2(a, count);
	factus_app_delete(a);
	factus_close(factus);
	vide_close(vide);
	vide_delete(vide);

	printf("Hello, world! This is factus.\n");

	return 0;
}
