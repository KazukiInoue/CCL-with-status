#include"Func.h"
#include<iostream>
#include<vector>
#include<random>
#include<string>
#include<strstream>
#include<opencv2/opencv.hpp>

const int LINE_WIDTH = 1;
const int MARGIN_WIDTH = 10;

int main()
{
	//�O���[�X�P�[������
	//cv::Mat input = cv::imread("C:\\Users\\LAB\\Pictures\\naruto_514\\3_remove.jpg", cv::IMREAD_GRAYSCALE);
	cv::Mat input = cv::imread("C:\\Users\\LAB\\Pictures\\Ghoul01.jpg", cv::IMREAD_GRAYSCALE);

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

	//Sec2-1 Handling unclosed panel

	//��l��
	cv::threshold(src, src, 235, 255, cv::THRESH_BINARY);

	//�X�P�[�����O
	src.convertTo(src, CV_32F, 1.0 / 255.0);

	//�g�����ǂ����𔻒�
	PanelBoundary(src, LINE_WIDTH);



	//addm�𔒂ŏ�����
	cv::Mat addm = cv::Mat::zeros(cv::Size(src.cols + 2 * MARGIN_WIDTH, src.rows + 2 * MARGIN_WIDTH), cv::IMREAD_GRAYSCALE);
	//�l�K�|�W���]
	addm = ~addm;
	//�X�P�[�����O
	addm.convertTo(addm, CV_32F, 1.0 / 255.0);
	//�]���𑫂�
	for (int y = 0; y < src.rows; y++) {
		for (int x = 0; x < src.cols; x++) {
			addm.ptr<float>(y + MARGIN_WIDTH)[x + MARGIN_WIDTH] = src.ptr<float>(y)[x];
		}
	}

	cv::imshow("After func", addm);

	/*std::cout << "addm start" << std::endl;
	for (int x = 0; x < addm.cols; x++) {
		if (addm.ptr<float>(MARGIN_WIDTH)[x] == 0.0f) {
			std::cout << x << std::endl;
		}
	}*/

	//���׃����O����
	cv::Mat daubing = cv::Mat::zeros(addm.size(), CV_8UC3);
	ExecuteLabeling(addm, daubing);

	cv::imshow("Input", input);
	cv::imshow("Labels", daubing);

	//PanelBoundary�ŏ������g������菜��

	//daubing.convertTo(daubing, CV_32F, 1.0 / 255.0);

	//�㉺�̘g��

	//std::cout << "start" << std::endl;

	//for (int x = 0; x < daubing.cols; x++) {
	//	if (daubing.ptr<float>(daubing.rows - MARGIN_WIDTH -1)[x] == 0.0f) {
	//		std::cout << x << std::endl;
	//	}
	//}
	//system("pause");

	////cv::line(daubing, cv::Point(10,0), cv::Point(10, daubing.rows), CV_RGB(255,0,0), LINE_WIDTH);    
	////cv::line(daubing, cv::Point(30, 0), cv::Point(30, daubing.rows), CV_RGB(0, 255, 255), LINE_WIDTH);


	//cv::imshow("Lines", daubing);
	//std::cout << "addm [cols, rows] = [" << addm.cols << " , " << addm.rows << "]"
	//	<< std::endl
	//	<< "daubing [cols, rows] = [" << daubing.cols << " , " << daubing.rows << "]"
	//	<< std::endl;


	//for (int x = 30; x < daubing.cols; x++) {
	//	daubing.ptr<float>(MARGIN_WIDTH)[x] = 1.0f;
	//}

	//cv::imshow("FROM 30", daubing);

	//cv::waitKey(0);

	//for (int x = 0; x < daubing.cols; x++) {

	//	//��̘g��
	//	if (//daubing.ptr<float>(MARGIN_WIDTH - 1)[x] == 1.0f&&
	//		daubing.ptr<float>(MARGIN_WIDTH)[x] == 0.0f
	//		&& daubing.ptr<float>(MARGIN_WIDTH + 1)[x] == 1.0f) {

	//		daubing.ptr<float>(MARGIN_WIDTH)[x] == 1.0f;

	//		std::cout << x << std::endl;
	//	}

	//	std::cout << daubing.ptr<float>(MARGIN_WIDTH - 1)[x]
	//		<< " " << daubing.ptr<float>(MARGIN_WIDTH)[x]
	//		<< " " << daubing.ptr<float>(MARGIN_WIDTH + 1)[x]
	//		<< std::endl;


	//	if (daubing.ptr<float>(daubing.rows - MARGIN_WIDTH - 1 - 1)[x] == 1.0f
	//		&& daubing.ptr<float>(daubing.rows - MARGIN_WIDTH - 1)[x] == 0.0f
	//		&& daubing.ptr<float>(daubing.rows - MARGIN_WIDTH - 1 + 1)[x] == 1.0f) {

	//		daubing.ptr<float>(daubing.rows - MARGIN_WIDTH - 1)[x] == 1.0f;
	//	}

	//}

	////���E�̘g��
	//for (int y = MARGIN_WIDTH; y < daubing.rows - MARGIN_WIDTH; y++) {

	//	//���̘g��
	//	if (daubing.ptr<float>(y)[MARGIN_WIDTH - 1] == 1.0f
	//		&& daubing.ptr<float>(y)[MARGIN_WIDTH] == 0.0f
	//		&& daubing.ptr<float>(y)[MARGIN_WIDTH + 1] == 1.0f) {

	//		daubing.ptr<float>(y)[MARGIN_WIDTH] == 1.0f;
	//	}
	//	//�E�̘g��
	//	if (daubing.ptr<float>(y)[daubing.cols - MARGIN_WIDTH - 1 - 1] == 1.0f
	//		&& daubing.ptr<float>(y)[daubing.cols - MARGIN_WIDTH - 1] == 0.0f
	//		&& daubing.ptr<float>(y)[daubing.cols - MARGIN_WIDTH - 1 + 1] == 1.0f) {

	//		daubing.ptr<float>(daubing.rows - MARGIN_WIDTH - 1)[y] == 1.0f;
	//	}
	//}

	//daubing.convertTo(daubing, CV_8UC3, 255.0);

	cv::line(daubing, cv::Point(MARGIN_WIDTH, MARGIN_WIDTH), cv::Point(daubing.cols - MARGIN_WIDTH - 1, MARGIN_WIDTH), CV_RGB(255, 255, 255), LINE_WIDTH);                                      //��̘g��
	cv::line(daubing, cv::Point(MARGIN_WIDTH, daubing.rows - MARGIN_WIDTH - 1), cv::Point(daubing.cols - MARGIN_WIDTH - 1, daubing.rows - MARGIN_WIDTH - 1), CV_RGB(255, 255, 255), LINE_WIDTH); //���̘g��
	cv::line(daubing, cv::Point(MARGIN_WIDTH, MARGIN_WIDTH), cv::Point(MARGIN_WIDTH, daubing.rows - MARGIN_WIDTH - 1), CV_RGB(255, 255, 255), LINE_WIDTH);                                        //���̘g��
	cv::line(daubing, cv::Point(daubing.cols - MARGIN_WIDTH - 1, MARGIN_WIDTH), cv::Point(daubing.cols - MARGIN_WIDTH - 1, daubing.rows - MARGIN_WIDTH), CV_RGB(255, 255, 255), LINE_WIDTH);    //�E�̘g��

	cv::imshow("remove white", daubing);

	cv::waitKey(0);

	//Sec2-2 Panel Block Splitting

	//Sec2-1�ł̏o�͂�����daubing��Sec2-2�̓��͂ɂ���



	daubing = ~daubing;

	cv::imshow("Negapoji", daubing);
	cv::waitKey(0);
	ChangeOutput2Input(daubing);

	cv::Mat split = cv::Mat::zeros(daubing.size(), CV_8UC3); //Sec2-2�ł̏o��

	PanelBlockSplitting(daubing, split, MARGIN_WIDTH);

	////Sec2-3 Panel Shape Extraction

	////Sec2-1�ł̏o�͂�����split��Sec2-2�̓��͂ɂ���
	//ChangeOutput2Input(split);

	////�p�l���̘g����؂�o��
	//cv::Mat shape = cv::Mat::zeros(split.size(), CV_8UC3);
	//cv::Mat dst = input.clone();
	//cvtColor(dst, dst, CV_GRAY2RGB);

	//PanelShapeExtraction(split, shape, dst, MARGIN_WIDTH);

	return 0;

}