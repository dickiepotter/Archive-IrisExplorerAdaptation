#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define LENGTH 16
#define CENTER_ABS 8

int main(void)
{
	int x,y,z;
	int zero_i = 0;
	int one_i = 1;
	float zero_f = 0;
	float one_f = 1;
	float *zero_i_fp = (float *) &zero_i;
	float *one_i_fp = (float *) &one_i;
	int *zero_f_ip = (int *) &zero_f;
	int *one_f_ip = (int *) &one_f;
	float res_x, res_y, res_z;
	int *res_x_int, *res_y_int, *res_z_int;
	srand( (unsigned) time(NULL));
	x= LENGTH * (rand() % RAND_MAX) / RAND_MAX;
	y= LENGTH * (rand() % RAND_MAX) / RAND_MAX;
	z= LENGTH * (rand() % RAND_MAX) / RAND_MAX;
	
	res_x = (float) (CENTER_ABS - x) / CENTER_ABS;
	res_y = (float) (CENTER_ABS - y) / CENTER_ABS;
	res_z = (float) (CENTER_ABS - z) / CENTER_ABS;
	res_x_int = (int *) &res_x;
	res_y_int = (int *) &res_y;
	res_z_int = (int *) &res_z;

	printf("float vals : %f, %f, %f\nint vals : %d, %d, %d\n0:%d,%f\t1:%d,%f\n", 
		res_x, res_y, res_z, *res_x_int, *res_y_int, *res_z_int,
		*zero_f_ip, *zero_i_fp, one_f_ip, one_i_fp);

}
