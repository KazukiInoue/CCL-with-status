#include "Func.h"
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
	cv::Mat input = cv::imread("C:\\Users\\SOGE\\Pictures\\naruto_514\\2_remove.jpg", cv::IMREAD_GRAYSCALE);

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

	//���׃����O����
	cv::Mat daubing = cv::Mat::zeros(addm.size(), CV_8UC3);
	ExecuteLabeling(addm, daubing);

	cv::imshow("Input", input);
	cv::imshow("Labels", daubing);
	
	//PanelBoundary�ŏ������g������菜��

	daubing.convertTo(daubing, CV_32F, 1.0 / 255.0);

	//cv::line(daubing, cv::Point(101, 0), cv::Point(101, daubing.rows), CV_RGB(255, 0,0), LINE_WIDTH);
	//cv::imshow("LINE", daubing);
	//cv::waitKey(0);

	std::cout <<"start" << std::endl;
	for (int y = 0; y < daubing.rows; y++) {

		std::cout << daubing.ptr<float>(y)[101] << std::endl;

		//if (daubing.ptr<float>(y)[101] == 0.0f) {
		//	std::cout << y << std::endl;
		//	system("pause");
		//}
	}
	system("pause");
	//�㉺�̘g��
	for (int x = MARGIN_WIDTH; x < daubing.cols - MARGIN_WIDTH; x++) {

		//��̘g��
		if (daubing.ptr<float>(MARGIN_WIDTH - 1)[x] == 1.0f
			&& daubing.ptr<float>(MARGIN_WIDTH)[x] == 0.0f
			&& daubing.ptr<float>(MARGIN_WIDTH + 1)[x] == 1.0f) {

			daubing.ptr<float>(MARGIN_WIDTH)[x] == 1.0f;
		}

		//std::cout << daubing.ptr<float>(MARGIN_WIDTH - 1)[x]
		//	<< " " << daubing.ptr<float>(MARGIN_WIDTH)[x]
		//	<< " " << daubing.ptr<float>(MARGIN_WIDTH + 1)[x]
		//	<< std::endl;


		if (daubing.ptr<float>(daubing.rows - MARGIN_WIDTH - 1 - 1)[x] == 1.0f
			&& daubing.ptr<float>(daubing.rows - MARGIN_WIDTH - 1)[x] == 0.0f
			&& daubing.ptr<float>(daubing.rows - MARGIN_WIDTH - 1 + 1)[x] == 1.0f) {

			daubing.ptr<float>(daubing.rows - MARGIN_WIDTH - 1)[x] == 1.0f;
		}

	}

	//���E�̘g��
	for (int y = MARGIN_WIDTH; y < daubing.rows - MARGIN_WIDTH; y++) {

		//���̘g��
		if (daubing.ptr<float>(y)[MARGIN_WIDTH - 1] == 1.0f
			&& daubing.ptr<float>(y)[MARGIN_WIDTH] == 0.0f
			&& daubing.ptr<float>(y)[MARGIN_WIDTH + 1] == 1.0f) {

			daubing.ptr<float>(y)[MARGIN_WIDTH] == 1.0f;
		}
		//�E�̘g��
		if (daubing.ptr<float>(y)[daubing.cols - MARGIN_WIDTH - 1 - 1] == 1.0f
			&& daubing.ptr<float>(y)[daubing.cols - MARGIN_WIDTH - 1] == 0.0f
			&& daubing.ptr<float>(y)[daubing.cols - MARGIN_WIDTH - 1 + 1] == 1.0f) {

			daubing.ptr<float>(daubing.rows - MARGIN_WIDTH - 1)[y] == 1.0f;
		}
	}

	daubing.convertTo(daubing, CV_8UC3, 255.0);

	/*cv::line(daubing, cv::Point(MARGIN_WIDTH, MARGIN_WIDTH), cv::Point(daubing.cols - MARGIN_WIDTH - 1, MARGIN_WIDTH), CV_RGB(255, 255, 255), LINE_WIDTH);
	cv::line(daubing, cv::Point(MARGIN_WIDTH, daubing.rows - MARGIN_WIDTH - 1), cv::Point(daubing.cols - MARGIN_WIDTH - 1, daubing.rows - MARGIN_WIDTH - 1), CV_RGB(255, 255, 255), LINE_WIDTH);
	cv::line(daubing, cv::Point(MARGIN_WIDTH, MARGIN_WIDTH), cv::Point(MARGIN_WIDTH, daubing.rows - MARGIN_WIDTH - 1), CV_RGB(255, 255, 255), LINE_WIDTH);
	cv::line(daubing, cv::Point(daubing.cols - MARGIN_WIDTH - 1, MARGIN_WIDTH ), cv::Point(daubing.cols - MARGIN_WIDTH - 1, daubing.rows - MARGIN_WIDTH ), CV_RGB(255, 255, 255), LINE_WIDTH);
*/
	cv::namedWindow("remove white", CV_WINDOW_FREERATIO);
	cv::imshow("remove white", daubing);
	cv::imshow("src", src);


	cv::waitKey(0);



	//Sec2-2 Panel Block Splitting



	//Sec2-3 Panel Shape Extraction

	daubing = ~daubing;

	//�X�P�[�����O
	daubing.convertTo(daubing, CV_32F, 1.0 / 255.0);
	cv::cvtColor(daubing, daubing, CV_BGR2GRAY);

	//�p�l���̘g����؂�o��
	cv::Mat shape = cv::Mat::zeros(daubing.size(), CV_8UC3);
	cv::Mat dst = input.clone();
	cvtColor(dst, dst, CV_GRAY2RGB);

	PanelShapeExtraction(daubing, shape, dst, MARGIN_WIDTH);

	return 0;

}