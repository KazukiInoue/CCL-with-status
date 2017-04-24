#pragma once
#include<opencv2/opencv.hpp>

//Useful
void ChangeOutput2Input(cv::Mat& output);

//Sec2-1
void PanelBoundary(cv::Mat& src, const int  LINE_WIDTH);
void ExecuteLabeling(cv::Mat& src, cv::Mat& dst);

//Sec2-2
void PanelBlockSplitting(cv::Mat& src, cv::Mat& dst, const int MARGIN_WIDTH);

//Sec2-3
void PanelShapeExtraction(cv::Mat& src, cv::Mat& shape, cv::Mat& dst, const int MARGIN_WIDTH);