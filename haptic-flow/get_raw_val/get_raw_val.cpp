#include <iostream>
#include <fstream>

#define LENGTH 64
#define FILE_SIZE LENGTH*LENGTH*LENGTH*3*sizeof(float)

using namespace std;

int main(int argc, char **argv)
{
	ifstream is( argv[1], ios::in | ios::binary );
	float * fdata;
	unsigned char *data = new unsigned char[FILE_SIZE];
	fdata = reinterpret_cast<float *>(data);

	if(argc !=2){
		std::cerr << "syntax: " << argv[0] << " filename" << std::endl;
	}
	is.read( (char *) data, FILE_SIZE);
	is.close();

	int offset=3*(64*64 + 64 + 1);
	cout << "Value of voxel 1,1,1: "
		<< fdata[offset] << ", "
		<< fdata[offset+1] << ", "
		<< fdata[offset+2] << std::endl;
}
