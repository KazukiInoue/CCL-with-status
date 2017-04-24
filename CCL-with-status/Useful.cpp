#include "Func.h"
#include<iostream>
#include<vector>
#include<random>
#include<string>
#include<strstream>

void ChangeOutput2Input(cv::Mat& output) {
	output.convertTo(output, CV_32F, 1.0 / 255.0);
	cv::cvtColor(output, output, CV_BGR2GRAY);
}