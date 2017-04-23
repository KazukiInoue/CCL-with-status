#include "Func.h"
#include<iostream>
#include<vector>
#include<random>
#include<string>
#include<strstream>
#include<opencv2/opencv.hpp>

const int LINE_WIDTH = 2;
const int MARGIN_WIDTH = 10;

int main()
{
	//�O���[�X�P�[������
	cv::Mat input = cv::imread("C:\\Users\\SOGE\\Pictures\\naruto_514\\2_remove.jpg", cv::IMREAD_GRAYSCALE);
	//cv::Mat input = cv::imread("C:/Users/LAB/Pictures/white.jpg", cv::IMREAD_GRAYSCALE);

	// �摜�̓ǂݍ��݂Ɏ��s������G���[�I������
	if (input.empty()) {
		std::cerr << "Failed to open image file." << std::endl;
		system("pause");
		return -1;
	}

	//���T�C�Y
	const double ORIGINAL_RESIZE = 800 / (double)input.rows;
	cv::resize(input, input, cv::Size(), ORIGINAL_RESIZE, ORIGINAL_RESIZE);

	cv::Mat src = input.clone();

	//Sec2-1

	//Handling unclosed panel

	//��l��
	cv::threshold(src, src, 235, 255, cv::THRESH_BINARY);

	//�X�P�[�����O
	src.convertTo(src, CV_32F, 1.0 / 255.0);

	//�]������̏ꍇ
	PanelBoundary(src, 1, 0, 0, 0, LINE_WIDTH);
	//�]�������̏ꍇ
	PanelBoundary(src, 0, 1, 0, 0, LINE_WIDTH);
	//�]�������̏ꍇ
	PanelBoundary(src, 0, 0, 1, 0, LINE_WIDTH);
	//�]�����E�̏ꍇ
	PanelBoundary(src, 0, 0, 0, 1, LINE_WIDTH);

	//addm�𔒂ŏ�����
	cv::Mat addm = cv::Mat::zeros(cv::Size(src.cols + 2 * MARGIN_WIDTH, src.rows + 2 * MARGIN_WIDTH), cv::IMREAD_GRAYSCALE);
	//�l�K�|�W���]
	addm = ~addm;
	//�X�P�[�����O
	addm.convertTo(addm, CV_32F, 1.0 / 255.0);
	//�]���𑫂�
	for (int y = MARGIN_WIDTH; y < src.rows + MARGIN_WIDTH; y++) {
		for (int x = MARGIN_WIDTH; x < src.cols + MARGIN_WIDTH; x++) {
			addm.ptr<float>(y)[x] = src.ptr<float>(y - MARGIN_WIDTH)[x - MARGIN_WIDTH];
		}
	}

	cv::imshow("After func", addm);
	cv::waitKey(0);

	//���׃����O����
	cv::Mat daubing = cv::Mat::zeros(addm.size(), CV_8UC3);
	ExecuteLabeling(addm, daubing);

	cv::imshow("Input", input);
	cv::imshow("Labels", daubing);
	cv::waitKey();

	//PanelBoundary�ŏ������g������菜��
	cv::line(daubing, cv::Point(MARGIN_WIDTH, MARGIN_WIDTH), cv::Point(daubing.cols - MARGIN_WIDTH, MARGIN_WIDTH), CV_RGB(255, 255, 255), LINE_WIDTH + 1);
	cv::line(daubing, cv::Point(MARGIN_WIDTH, MARGIN_WIDTH), cv::Point(MARGIN_WIDTH, daubing.rows - MARGIN_WIDTH), CV_RGB(255, 255, 255), LINE_WIDTH + 1);
	cv::line(daubing, cv::Point(MARGIN_WIDTH, daubing.rows - MARGIN_WIDTH), cv::Point(daubing.cols - MARGIN_WIDTH, daubing.rows - MARGIN_WIDTH), CV_RGB(255, 255, 255), LINE_WIDTH + 1);
	cv::line(daubing, cv::Point(daubing.cols - MARGIN_WIDTH, MARGIN_WIDTH), cv::Point(daubing.cols - MARGIN_WIDTH, daubing.rows - MARGIN_WIDTH), CV_RGB(255, 255, 255), LINE_WIDTH + 1);

	cv::imshow("remove white", daubing);
	cv::waitKey(0);

	//Sec2-2



	//Sec2-3

	daubing = ~daubing;

	//�X�P�[�����O
	daubing.convertTo(daubing, CV_32F, 1.0 / 255.0);
	cv::cvtColor(daubing, daubing, CV_BGR2GRAY);

	//���׃����O����
	cv::Mat dst = cv::Mat::zeros(daubing.size(), CV_8UC3);
	PanelShapeExtraction(daubing, dst);

	cv::imshow("re-CCL", dst);
	cv::waitKey(0);

	return 0;
}