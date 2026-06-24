#include <stdio.h>
#include <stdlib.h>
#include <limits.h>


#define LENGTH 64
#define CENTER_ABS (LENGTH/2)
#define FILE_SIZE LENGTH*LENGTH*LENGTH*3*sizeof(float)

// converts the float f/max to an int between -INT_MAX and INT_MAX
int float_to_rawfloat(float f, int max)
{
	double tmp;
	tmp = f/max;
	tmp *= INT_MAX;
	return (int) tmp;
}

// do the conversion in the other way (used for checking)
float rawfloat_to_float(int rf, int max)
{
	double tmp;
	tmp = (double) rf;
	tmp /= INT_MAX;
	tmp *= max;
	return (float) tmp;
}

int build_tab(int *tab)
{
	int x,y,z;
	int old_offset=-1;
	for(x=0;x<LENGTH;x++)
		for(y=0;y<LENGTH;y++)
			for(z=0;z<LENGTH;z++){
				int offset = 3*(LENGTH*LENGTH*x + LENGTH*y + z);
				if(old_offset!=-1){
					if( (offset-old_offset)!=3){
						fprintf(stderr, "pb @ x,y,z=%d,%d,%d\n", x,y,z);
					}
				}
				old_offset=offset;
				// H3D expects a number between -1 and 1,
				// multiplied by INT_MAX and rounded
				tab[offset]   =float_to_rawfloat(CENTER_ABS - z, CENTER_ABS); 
				tab[offset+1] =float_to_rawfloat(CENTER_ABS - y, CENTER_ABS); 
				tab[offset+2] =float_to_rawfloat(CENTER_ABS - x, CENTER_ABS); 
				if(x==1 && y==2 && z==3){
					printf("1,2,3: %d, %d, %d\n", tab[offset], tab[offset+1], tab[offset+2]);
				}
				/*
				tab[offset]   = 1;
				tab[offset+1] = z/LENGTH;
				tab[offset+2] = 0;
				*/
			}
	return 0;
}

int main(int argc, char **argv){
	int *res, *buf, x, y, z;
	FILE *f;
	if(argc !=2){
		fprintf(stderr, "syntax: %s output_filename\n", argv[0]);
		return 1;
	}
	res = malloc(FILE_SIZE);
	build_tab(res);
	fprintf(stderr, "voxel 0 0 1: %f, %f, %f\n",
			rawfloat_to_float(res[3], CENTER_ABS),
			rawfloat_to_float(res[4], CENTER_ABS),
			rawfloat_to_float(res[5], CENTER_ABS));
	fprintf(stderr, "voxel 0 1 0: %f, %f, %f\n",
			rawfloat_to_float(res[192], CENTER_ABS),
			rawfloat_to_float(res[193], CENTER_ABS),
			rawfloat_to_float(res[194], CENTER_ABS));
	x=res[3*(64*64 + 64 + 1)];
	y=res[3*(64*64 + 64 + 1)+1];
	z=res[3*(64*64 + 64 + 1)+2];
	fprintf(stderr, "voxel 1 1 1: %f, %f, %f\n",
			rawfloat_to_float(res[3*(64*64 + 64 + 1)], CENTER_ABS),
			rawfloat_to_float(res[3*(64*64 + 64 + 1)+1], CENTER_ABS),
			rawfloat_to_float(res[3*(64*64 + 64 + 1)+2], CENTER_ABS));
	f = fopen(argv[1], "w");
	fwrite(res, FILE_SIZE, 1, f);
	fclose(f);
	printf("%s written, with sizeof(float)=%d\n", argv[1], sizeof(float));
	printf("Checking... ");
	buf = malloc(FILE_SIZE);
	f = fopen(argv[1], "r");
	fread(buf, FILE_SIZE, 1, f);
	if(x!=buf[3*(64*64 + 64 + 1)] ||
			y!=buf[3*(64*64 + 64 + 1)+1] ||
			z!=buf[3*(64*64 + 64 + 1)+2]){
		printf("No!\nerror at voxel 1,1,1; read (%f, %f, %f); should be (%f, %f, %f)\n",
				buf[3*(64*64 + 64 + 1)], buf[3*(64*64 + 64 + 1)+1], buf[3*(64*64 + 64 + 1)+2],
				x, y, z);
	}else{
		printf("Ok.\n");
	}
	return 0;
}

// former version : with floats
/*
#define LENGTH 64
#define CENTER_ABS (LENGTH/2)
#define FILE_SIZE LENGTH*LENGTH*LENGTH*3*sizeof(float)
int build_tab(float *tab)
{
	int x,y,z;
	for(x=0;x<LENGTH;x++)
		for(y=0;y<LENGTH;y++)
			for(z=0;z<LENGTH;z++){
				int offset = 3*(LENGTH*LENGTH*x + LENGTH*y + z);
				tab[offset]   = (float)(CENTER_ABS - x)/CENTER_ABS;
				tab[offset+1] = (float)(CENTER_ABS - y)/CENTER_ABS;
				tab[offset+2] = (float)(CENTER_ABS - z)/CENTER_ABS;
			}
}

int main(int argc, char **argv){
	float *res;
	FILE *f;
	if(argc !=2){
		fprintf(stderr, "syntax: %s output_filename\n", argv[0]);
		return 1;
	}
	res = malloc(FILE_SIZE);
	build_tab(res);
	f = fopen(argv[1], "w");
	fwrite(res, FILE_SIZE, 1, f);
	fclose(f);
	printf("%s written, with sizeof(float)=%d\n", argv[1], sizeof(float));
	return 0;
}
*/
