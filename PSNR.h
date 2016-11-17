#include "YUV.h"

class PSNR {
private:
	float mse_[4]; // y,u,v,total
	float psnr_[4]; // y,u,v,total
	int size_[4]; // y,u,v,total in bytes
	float computeMSEPlane(unsigned char *plane_ref, unsigned char *plane_test, int plane);
public:
	PSNR(int width, int height, char *format);
	~PSNR();
	float mse2psnr(float mse);
	void computePSNR(YUV *yuv_ref, YUV *yuv_test);
	void getPSNR(float psnr[4]);
	void getMSE(float mse[4]);
};

PSNR::PSNR(int width, int height, char *format){
	
	if (strcmp(format, "yuv420p") == 0){
		//format_ = YUV420p;
		size_[0] = width * height;
		size_[1] = (width >> 1) * (height >> 1);
		size_[2] = (width >> 1) * (height >> 1);
		size_[3] = size_[0] + size_[1] + size_[2];
	}
	else if (strcmp(format, "yuv422p") == 0) {
		//format_ = YUV422p;
		size_[0] = width * height;
		size_[1] = (width >> 1) * height;
		size_[2] = (width >> 1) * height;
		size_[3] = size_[0] + size_[1] + size_[2];
	}
	else if (strcmp(format, "yuv444p") == 0){
		//format_ = YUV444p;
		size_[0] = width * height;
		size_[1] = width * height;
		size_[2] = width * height;
		size_[3] = size_[0] + size_[1] + size_[2];
	}
	else {
		cerr << endl << "ERROR: ChromaFormat can only be yuv420p, yuv422p or yuv444p" << endl;
		cout << "Exiting ... " << endl;
		exit(-1);
	}
}

PSNR::~PSNR(){
	//delete[] mse_, psnr_;
}

float PSNR::mse2psnr(float mse){
	if (mse == 0)
		return -1;
	else
		return 10 * log10((255 * 255) / mse);
}

float PSNR::computeMSEPlane(unsigned char *plane_ref, unsigned char *plane_test, int plane) {
	float mse = 0;
	for (int ii = 0; ii < size_[plane]; ii++) {
		mse += (*(plane_ref + ii) - *(plane_test + ii)) * (*(plane_ref + ii) - *(plane_test + ii));
	}
	return mse / (size_[plane]);
}

void PSNR::computePSNR(YUV *yuv_ref, YUV *yuv_test){
	for (int i = 0; i < 3; i++) {
		mse_[i] = computeMSEPlane(yuv_ref->getPlane(i), yuv_test->getPlane(i), i);
		psnr_[i] = mse2psnr(mse_[i]);
	}
	mse_[3] = (mse_[0] + mse_[1] + mse_[2]) / 3;
	psnr_[3] = mse2psnr(mse_[3]);
}

void PSNR::getPSNR(float psnr[4]){
	for (int i = 0; i < 4; i++)
		psnr[i] = psnr_[i];
}

void PSNR::getMSE(float mse[4]){
	for (int i = 0; i < 4; i++)
		mse[i] = mse_[i];
}
