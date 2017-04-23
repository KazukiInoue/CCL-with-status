#include "Func.h"
#include<iostream>
#include<vector>
#include<random>
#include<string>
#include<strstream>

#include<math.h>

const float PI = 3.14159265359;

void PanelShapeExtraction(cv::Mat& src, cv::Mat& dst) {

	const int 	area_th = 500; //�{����Sec2-2�ŏ�������

	//src��255�ɖ߂�
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

		if (param[cv::ConnectedComponentsTypes::CC_STAT_AREA] >= area_th) {
			int left = param[cv::ConnectedComponentsTypes::CC_STAT_LEFT];
			int top = param[cv::ConnectedComponentsTypes::CC_STAT_TOP];
			int height = param[cv::ConnectedComponentsTypes::CC_STAT_HEIGHT];
			int width = param[cv::ConnectedComponentsTypes::CC_STAT_WIDTH];

			//cv::rectangle(dst, cv::Rect(left, top, width, height), cv::Scalar(0, 255, 0), 2);
		}
	}


	//�d�S�̏o��

	for (int i = 1; i < nLab; ++i) {

		int *param = stats.ptr<int>(i);
		double *centorids_param = centroids.ptr<double>(i);

		if (param[cv::ConnectedComponentsTypes::CC_STAT_AREA] >= area_th) {
			int x_center = static_cast<int>(centorids_param[0]);
			int y_center = static_cast<int>(centorids_param[1]);

			cv::circle(dst, cv::Point(x_center, y_center), 3, cv::Scalar(0, 0, 255), -1);

			//���x���t�����ꂽ�̈���A���ꂼ��̗̈�̒��S�_�����_�Ƃ����f�J���g���W�����ƂɊp��������

			int left = param[cv::ConnectedComponentsTypes::CC_STAT_LEFT];
			int top = param[cv::ConnectedComponentsTypes::CC_STAT_TOP];
			int height = param[cv::ConnectedComponentsTypes::CC_STAT_HEIGHT];
			int width = param[cv::ConnectedComponentsTypes::CC_STAT_WIDTH];

			int point_max_distance[4][2] = {}; //�R�}�̊p�S�_

			const int START_POINT[4][2] = {
						{ left + width / 2 ,top}, //���ی��̎n�_
						{ left ,top }, //���ی��̎n�_
						{ left ,top + height / 2 }, //��O�ی��̎n�_
						{ left + width / 2 ,top + height / 2 }, //��l�ی��̎n�_
			};

			//���ی��A���ی��A��O�ی��A��l�ی��Ɖ񂵂Ă���
			for (int quadrant = 0; quadrant < 4; quadrant++) {

				int max_distance = 0;

				for (int y = START_POINT[quadrant][1]; y < START_POINT[quadrant][1] + height / 2; y++) {
					for (int x = START_POINT[quadrant][0]; x < START_POINT[quadrant][0] + width / 2; x++) {

						//�����ꏊ�Ō��_����ł������_��T��
						if (dst.ptr<cv::Vec3b>(y)[x] == colors[i]) {
							int distance = pow(x - x_center, 2) + pow(y - y_center, 2);
							if (max_distance < distance) {
								max_distance = distance;
								point_max_distance[quadrant][0] = x;
								point_max_distance[quadrant][1] = y;
							}
						}
					}
				}

				cv::circle(dst, cv::Point(point_max_distance[quadrant][0], point_max_distance[quadrant][1]), 5, cv::Scalar(255, 0, 0), -1);


				cv::imshow("Corner", dst);
				cv::waitKey(0);
			}

			//�S�_������
			cv::line(dst, cv::Point(point_max_distance[1][0], point_max_distance[1][1]), cv::Point(point_max_distance[0][0], point_max_distance[0][1]), CV_RGB(255, 0, 0), 3);
			cv::line(dst, cv::Point(point_max_distance[1][0], point_max_distance[1][1]), cv::Point(point_max_distance[2][0], point_max_distance[2][1]), CV_RGB(255, 0, 0), 3);
			cv::line(dst, cv::Point(point_max_distance[3][0], point_max_distance[3][1]), cv::Point(point_max_distance[0][0], point_max_distance[0][1]), CV_RGB(255, 0, 0), 3);
			cv::line(dst, cv::Point(point_max_distance[3][0], point_max_distance[3][1]), cv::Point(point_max_distance[2][0], point_max_distance[2][1]), CV_RGB(255, 0, 0), 3);
		}
	}

	cv::imshow("Complete", dst);
	cv::waitKey(0);

	//�ʐϒl�̏o��
	for (int i = 1; i < nLab; ++i) {
		int *param = stats.ptr<int>(i);

		if (param[cv::ConnectedComponentsTypes::CC_STAT_AREA] >= area_th) {
			//std::cout << "area " << i << " = " << param[cv::ConnectedComponentsTypes::CC_STAT_AREA] << std::endl;

			//ROI�̍���ɔԍ�����������
			int x = param[cv::ConnectedComponentsTypes::CC_STAT_LEFT];
			int y = param[cv::ConnectedComponentsTypes::CC_STAT_TOP];
			std::stringstream num;
			num << i;

			if (param[cv::ConnectedComponentsTypes::CC_STAT_AREA] >= area_th) {
				cv::putText(dst, num.str(), cv::Point(x + 5, y + 20), cv::FONT_HERSHEY_COMPLEX, 0.7, cv::Scalar(0, 255, 255), 2);
			}
		}
	}

	//Float�ɖ߂�
	src.convertTo(src, CV_32F, 1 / 255.0);



	cv::imshow("5 Labeling", dst);
	cv::waitKey(0);
}