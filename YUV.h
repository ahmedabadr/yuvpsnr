#include <iostream>
#include <fstream>
using namespace std;

#define YUV420p 420
#define YUV422p 422
#define YUV444p 444

class YUV {
private:
	int width_; // in pixels
	int height_; // in pixels
	int size_; // size of a frame in pixels (width * height)
	int file_size_; // .yuv filesize in bytes
	int format_; // ChromaFormat
	ifstream f_;
	unsigned char *Y_; // points to the Y plane in the current frame
	unsigned char *U_; // points to the U plane in the current frame
	unsigned char *V_; // points to the V plane in the current frame
	
	int compute_filesize(char *filename); // returns the size of the yuv file in bytes

public:
	int no_of_frames_; // number of frames in the yuv file
	
	YUV(char *filename, int width, int height, char *format);
	~YUV();
	bool readFrame(); // read a single frame from the file
	unsigned char* getPlane(int plane); // return a pointer to the specified plane of the current frame
};

YUV::YUV(char *filename, int width, int height, char *format) {
	file_size_ = compute_filesize(filename); // find .yuv file size in bytes

	f_.open(filename, ios::in | ios::binary);
	if (f_.bad()) {
		cerr << endl << "ERROR: cannot open file " << filename << endl;
		cout << "Exiting ... " << endl;
		exit(-1);
	}
	width_ = width;
	height_ = height;
	size_ = width_ * height_;
	if (strcmp(format, "yuv420p") == 0) {
		format_ = YUV420p;
		if (file_size_ % (size_ + size_ >> 1) == 0)
			no_of_frames_ = file_size_ / (size_ * 1.5);
		else {
			cerr << endl << "ERROR: The calculated number of frames in " << filename << 
				" is not a whole number." << endl <<
				"This can be due to a wrong specified width, length, ChromaFormat or the file might be corrupted" << endl;
			cout << "Exiting ... " << endl;
			exit (-1);
		}
		Y_ = new unsigned char [size_];
		U_ = new unsigned char [size_ >> 2];
		V_ = new unsigned char [size_ >> 2];
	}
	else if (strcmp(format, "yuv422p") == 0){
		format_ = YUV422p;
		if (file_size_ % (size_ * 2) == 0)
			no_of_frames_ = file_size_ / (size_ * 2);
		else {
			cerr << endl << "ERROR: The calculated number of frames in " << filename << 
				" is not a whole number." << endl <<
				"This can be due to a wrong specified width, length, ChromaFormat or the file might be corrupted" << endl;
			cout << "Exiting ... " << endl;
			exit (-1);
		}
		Y_ = new unsigned char [size_];
		U_ = new unsigned char [size_ >> 1];
		V_ = new unsigned char [size_ >> 1];
	}
	else if (strcmp(format, "yuv444p") == 0) {
		format_ = YUV444p;
		if (file_size_ % (size_ * 3) == 0)
			no_of_frames_ = file_size_ / (size_ * 3);
		else {
			cerr << endl << "ERROR: The calculated number of frames in " << filename << 
				" is not a whole number." << endl <<
				"This can be due to a wrong specified width, length, ChromaFormat or the file might be corrupted" << endl;
			cout << "Exiting ... " << endl;
			exit (-1);
		}
		Y_ = new unsigned char [size_];
		U_ = new unsigned char [size_];
		V_ = new unsigned char [size_];
	}	
	else {
		cerr << endl << "ERROR: ChromaFormat can only be yuv420p, yuv422p or yuv444p" << endl;
		cout << "Exiting ... " << endl;
		exit(-1);
	}
	cout << no_of_frames_ << " frames in " << filename << endl;
}

YUV::~YUV() {
	f_.close();
	//fclose(f_);
	delete Y_, U_, V_;
}

bool YUV::readFrame() {
	if (f_.bad()) return false;

	if (format_ == YUV420p) {
		if (!f_.read((char*) Y_, size_)) return false;
		if (!f_.read((char*) U_, size_ >> 2)) return false;
		if (!f_.read((char*) V_, size_ >> 2)) return false;
	}
	else if (format_ == YUV422p) {
		if (!f_.read((char*) Y_, size_)) return false;
		if (!f_.read((char*) U_, size_ >> 1)) return false;
		if (!f_.read((char*) V_, size_ >> 1)) return false;
	}
	else if (format_ == YUV444p) {
		if (!f_.read((char*) Y_, size_)) return false;
		if (!f_.read((char*) U_, size_)) return false;
		if (!f_.read((char*) V_, size_)) return false;
	}
	else {
		return false;
	}
	return true;
}

unsigned char* YUV::getPlane(int plane){
	if (plane == 0)
		return Y_;
	else if (plane == 1)
		return U_;
	else if (plane == 2)
		return V_;
	else
		exit(-1);
}

int YUV::compute_filesize(char *filename) {
	FILE *f_tmp = fopen(filename, "rb");
	if (!f_tmp) {
		cerr << endl << "ERROR: cannot open file " << filename << endl;
		cout << "Exiting ... " << endl;
		exit(-1);
	}
	fseek(f_tmp,0,SEEK_END);
	int size = ftell(f_tmp);
	fclose(f_tmp);
	return size;
}