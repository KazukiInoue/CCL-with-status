#include "Func.h"
#include<iostream>
#include<vector>
#include<random>
#include<string>
#include<strstream>

void PanelBoundary(cv::Mat& src, bool up, bool bottom, bool left, bool right, const int  LINE_WIDTH) {

	//�g�����ǂ����𔻒�

	int max = 0; //��f�̍ő�l
	int min = 0; //��f�̍ŏ��l
	int delta = 0; //�ő�l�ƍŏ��l�̍���
	int dif = 0; //��f�̔���
	int fix = 0; //�T���̎n�_��x or y
	int end = 0;   //�T���̏I�_��y or x
	int boundary_len = 0; //���߂�Le

	//�㉺���E�ǂ��̔�������邩�����߂�
	//�]������̏ꍇ
	if (up == 1 && bottom == 0 && left == 0 && right == 0) {
		fix = 0;
		end = src.cols;
	}
	//�]�������̏ꍇ
	if (up == 0 && bottom == 1 && left == 0 && right == 0) {
		fix = src.rows - 1;
		end = src.cols;
	}
	//�]�������̏ꍇ
	if (up == 0 && bottom == 0 && left == 1 && right == 0) {
		fix = 0;
		end = src.rows;
	}
	//�]�����E�̏ꍇ
	if (up == 0 && bottom == 0 && left == 0 && right == 1) {
		fix = src.cols - 1;
		end = src.rows;
	}

	//�ő�l�ƍŏ��l�̌v�Z

	//�]�����㉺�̏ꍇ
	if (left == 0 && right == 0) {
		for (int x = 0; x < end; x++) {

			if (max < x*src.ptr<float>(fix)[x]) {
				max = x*src.ptr<float>(fix)[x];
			}
			if (min > x*src.ptr<float>(fix)[x]) {
				min = x*src.ptr<float>(fix)[x];
			}
		}
	}
	//�]�������E�̏ꍇ
	if (up == 0 && bottom == 0) {
		for (int y = 0; y < end; y++) {

			if (max < y*src.ptr<float>(y)[fix]) {
				max = y*src.ptr<float>(y)[fix];
			}
			if (min > y*src.ptr<float>(y)[fix]) {
				min = y*src.ptr<float>(y)[fix];
			}
		}
	}

	//�ő�l�ƍŏ��l�̍������v�Z
	delta = max - min;

	//�]�����㉺�̏ꍇ
	if (left == 0 && right == 0) {
		for (int x = 1; x < end; x++) {
			dif += abs((src.ptr<float>(fix)[x] - src.ptr<float>(fix)[x - 1]));
			//std::cout << "(" << x << "," << fix << ")" << ", dif = " << dif << std::endl;
			if (delta > 0.7*x && dif > 10) {
				boundary_len = x + 1;
			}
		}
	}
	//�]�������E�̏ꍇ
	if (up == 0 && bottom == 0) {
		for (int y = 1; y < end; y++) {
			dif += abs((src.ptr<float>(y)[fix] - src.ptr<float>(y - 1)[fix]));
			//std::cout << "y = " << y << ", dif =" << dif << std::endl;
			if (delta > 0.7*y && dif > 10) {
				boundary_len = y + 1;
			}
		}
	}

	//��������
	//�]�����㉺�̏ꍇ
	if (left == 0 && right == 0) {
		cv::line(src, cv::Point(0, fix), cv::Point(boundary_len, fix), 0, LINE_WIDTH);
	}
	//�]�������E�̏ꍇ
	if (up == 0 && bottom == 0) {
		cv::line(src, cv::Point(fix, 0), cv::Point(fix, boundary_len), 0, LINE_WIDTH);
	}
}










void ExecuteLabeling(cv::Mat& src, cv::Mat& dst) {

	const int area_th = src.rows*src.cols / 2830; //�ЂƂ܂����̒l((1055+10)�~(671+10)����250���炢)

	//255�ɖ߂�
	src.convertTo(src, CV_8UC3, 255.0);

	//���׃����O����
	cv::Mat LabelImg = cv::Mat::zeros(src.size(), CV_8UC3);    //���x���摜
	cv::Mat stats = cv::Mat::zeros(src.size(), CV_8UC3);       //�o�E���f�B���O�{�b�N�X�͈̔͂Ɩʐϒl
	cv::Mat centroids = cv::Mat::zeros(src.size(), CV_8UC3);   //���x���t������̈�̏d�S(x,y)

	int nLab = cv::connectedComponentsWithStats(src, LabelImg, stats, centroids, 8, CV_32S);  //���x���� + 1

	 // ���x�����O���ʂ̕`��F������
	std::vector<cv::Vec3b> colors(nLab);
	colors[0] = cv::Vec3b(0, 0, 0);  //label[0]�͎g��Ȃ�
	for (int i = 1; i < nLab; ++i) {
		int *param = stats.ptr<int>(i);

		//�����ȗ̈�͍����h��
		if (param[cv::ConnectedComponentsTypes::CC_STAT_AREA] < area_th) {
			colors[i] = cv::Vec3b(0, 0, 0);
		}
		else {
			colors[i] = cv::Vec3b((rand() & 255), (rand() & 255), (rand() & 255));
		}

	}

	// ���x�����O���ʂ̐ݒ�

	for (int i = 0; i < dst.rows; ++i) {
		for (int j = 0; j < dst.cols; ++j) {
			dst.ptr<cv::Vec3b>(i)[j] = colors[LabelImg.ptr<int>(i)[j]];
		}
	}

	cv::imshow("4 Labeling", dst);
	cv::waitKey(0);

	//ROI(Region of Interest)�̕`��
	for (int i = 1; i < nLab; ++i) {
		int *param = stats.ptr<int>(i);

		int x_start = param[cv::ConnectedComponentsTypes::CC_STAT_LEFT];
		int y_start = param[cv::ConnectedComponentsTypes::CC_STAT_TOP];
		int height = param[cv::ConnectedComponentsTypes::CC_STAT_HEIGHT];
		int width = param[cv::ConnectedComponentsTypes::CC_STAT_WIDTH];

		//cv::rectangle(dst, cv::Rect(x_start, y_start, width, height), cv::Scalar(0, 255, 0), 2);
	}


	////�d�S�̏o��
	//for (int i = 1; i < nLab; ++i) {
	//	double *param = centroids.ptr<double>(i);
	//	int x = static_cast<int>(param[0]);
	//	int y = static_cast<int>(param[1]);

	//	cv::circle(dst, cv::Point(x, y), 3, cv::Scalar(0, 0, 255), -1);
	//}

	//�ʐϒl�̏o��
	int max_area = 0;

	for (int i = 1; i < nLab; ++i) {
		int *param = stats.ptr<int>(i);
		//std::cout << "area " << i << " = " << param[cv::ConnectedComponentsTypes::CC_STAT_AREA] << std::endl;

		//ROI�̍���ɔԍ�����������
		int x = param[cv::ConnectedComponentsTypes::CC_STAT_LEFT];
		int y = param[cv::ConnectedComponentsTypes::CC_STAT_TOP];
		std::stringstream num;
		num << i;

		//if (param[cv::ConnectedComponentsTypes::CC_STAT_AREA] >= area_th) {
		//	cv::putText(dst, num.str(), cv::Point(x + 5, y + 20), cv::FONT_HERSHEY_COMPLEX, 0.7, cv::Scalar(0, 255, 255), 2);
		//}


		//�o�E���f�B���O�{�b�N�X���ő�̗̈��T��
		if (max_area < param[cv::ConnectedComponentsTypes::CC_STAT_AREA]) {
			max_area = param[cv::ConnectedComponentsTypes::CC_STAT_AREA];
		}
	}

	//�o�E���f�B���O�{�b�N�X�̖ʐς��ő�łȂ���΍����h��
	for (int i = 1; i < nLab; i++) {
		int *param = stats.ptr<int>(i);
		if (max_area == param[cv::ConnectedComponentsTypes::CC_STAT_AREA]) {
			colors[i] = cv::Vec3b(255, 255, 255);
			continue;

		}
		else {
			colors[i] = cv::Vec3b(0, 0, 0);
		}
	}

	for (int i = 0; i < dst.rows; ++i) {
		for (int j = 0; j < dst.cols; ++j) {
			dst.ptr<cv::Vec3b>(i)[j] = colors[LabelImg.ptr<int>(i)[j]];

		}
	}


	//Float�ɖ߂�
	src.convertTo(src, CV_32F, 1 / 255.0);
}