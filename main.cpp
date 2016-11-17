#include <iostream>
#include <fstream>
#include <iomanip>
#include <ctime>
#include <stdio.h>
#include <cstdlib>
#include <cstring>
#include <cmath>
#include "PSNR.h"

using namespace std;

int main(int argc, char *argv[]) {
	// USAGE
	if (argc < 7 || argc > 8) {
		cout << endl << "Usage:" << endl;
		cout << "	yuvpsnr ref.yuv test.yuv width height [ChromaFormat] [ResultsFile] [Print]" << endl;
		cout << "ChromaFormat:" << endl;
		cout << "	yuv420p" << endl;
		cout << "	yuv422p" << endl;
		cout << "	yuv444p" << endl;
		cout << "ResultsFile: specifies the output filename" << endl;
		cout << "	filename.txt" << endl;
		cout << "	filename.csv" << endl;
		cout << "	filename" << endl;
		cout << "Print: is an optional argument which specifies whether to print frame-by-frame scores" << endl;
		cout << "	0 does not print frame-by-frame scores" << endl;
		cout << "	1 prints frame-by-frame scores" << endl;
		cout << "	default = 0" << endl;
		return -1;
	}
	
	clock_t begin = clock();

	// Parse Arguments
	char *filename_ref = argv[1];
	char *filename_test = argv[2];
	int width = atoi(argv[3]);
	int height = atoi(argv[4]);
	char *chroma_format = argv[5];
	char *filename_results = argv[6];
	int print_framebyframe;
	if (argc == 7) print_framebyframe = 0; else print_framebyframe = atoi(argv[7]);
	
	// Instantiate YUV and PSNR Objects
	YUV *yuv_ref = new YUV(filename_ref, width, height, chroma_format);
	YUV *yuv_test = new YUV(filename_test, width, height, chroma_format);
	if (yuv_ref->no_of_frames_ != yuv_test->no_of_frames_){
		cerr << endl << "ERROR: reference and test files are not of the same size" << endl;
		cout << "Exiting ... " << endl;
		return -1;
	}
	PSNR *psnr_data = new PSNR(width, height, chroma_format);

	// Open results file for writing PESQ Scores
	ofstream f_results; 
	//FILE *f_results;
	f_results.open(filename_results);
	//f_results = fopen(filename_results, "w");
	if (f_results.bad()){
		cerr << endl << "ERROR: cannot open results file " << filename_results << endl;
		cout << "Exiting ... " << endl;
		return -1;
	}
		
	float psnr[4], mse[4]; // Values for the current frame
	float psnr_avg[4], mse_avg[4] = { 0 }; // Average across all frames
	// The first 3 values correspond to y, u and v planes and the 4th correspond to the overall

	int frame_number = 0;
	// Main Loop -- Read one frame at a time and compute its corresponding MSE and PSNR values for all planes
	while (yuv_ref->readFrame() && yuv_test->readFrame()) {
		frame_number++;
		psnr_data->computePSNR(yuv_ref, yuv_test);
		psnr_data->getPSNR(psnr);
		psnr_data->getMSE(mse);
		// print
		if (print_framebyframe)
			cout << "Frame Number: " << fixed << setw(4) << frame_number << 
				" -> PSNR(Y,U,V,YUV) = (" << 
				fixed << setw(6) << setprecision(2) << psnr[0] << " ," <<
				fixed << setw(6) << setprecision(2) << psnr[1] << " ," << 
				fixed << setw(6) << setprecision(2) << psnr[2] << " ," << 
				fixed << setw(6) << setprecision(2) << psnr[3] << " )" << endl;
		// write to results file
		f_results << fixed << setw(4) << frame_number << ", " << 
				fixed << setw(6) << setprecision(2) << psnr[0] << ", " <<
				fixed << setw(6) << setprecision(2) << psnr[1] << "," << 
				fixed << setw(6) << setprecision(2) << psnr[2] << "," << 
				fixed << setw(6) << setprecision(2) << psnr[3] << endl;
		for (int i = 0; i < 4; i++) mse_avg[i] += mse[i];
	}

	// compute average MSE to compute average PSNR across all frames for all planes
	for (int i = 0; i < 4; i++) {
		mse_avg[i] /= frame_number;
		psnr_avg[i] = psnr_data->mse2psnr(mse_avg[i]);
	}
	// print on screen
	cout << "Average PSNR: " << 
				"------> PSNR(Y,U,V,YUV) = (" << 
				fixed << setw(6) << setprecision(2) << psnr_avg[0] << " ," <<
				fixed << setw(6) << setprecision(2) << psnr_avg[1] << " ," << 
				fixed << setw(6) << setprecision(2) << psnr_avg[2] << " ," << 
				fixed << setw(6) << setprecision(2) << psnr_avg[3] << " )" << endl;
	// write to results file
	f_results << " avg, " << 
				fixed << setw(6) << setprecision(2) << psnr_avg[0] << ", " <<
				fixed << setw(6) << setprecision(2) << psnr_avg[1] << "," << 
				fixed << setw(6) << setprecision(2) << psnr_avg[2] << "," << 
				fixed << setw(6) << setprecision(2) << psnr_avg[3] << endl;

	psnr_data->~PSNR();
	yuv_ref->~YUV();
	yuv_test->~YUV();
	f_results.close();
	//fclose(f_results);

	clock_t end = clock();
	double elapsed_secs = double(end - begin) / CLOCKS_PER_SEC;
	cout << "Time Elapsed = " << fixed << setw(6) << setprecision(2) << elapsed_secs << " seconds" << endl;
	cout << "Processing Speed = " << fixed << setw(6) << setprecision(2) << frame_number / elapsed_secs << " fps" << endl;
	return 0;
}
