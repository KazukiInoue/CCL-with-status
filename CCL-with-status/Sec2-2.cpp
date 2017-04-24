#include"Func.h"
#include<iostream>
#include<vector>
#include<random>
#include<string>
#include<strstream>

void PanelBlockSplitting(cv::Mat& src, cv::Mat& dst, const int MARGIN_WIDTH) {

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

	//Float�ɖ߂�
	cv::Mat src_color = src.clone();
	src.convertTo(src, CV_32F, 1 / 255.0);

	for (int i = 1; i < nLab; ++i) {
		int *param = stats.ptr<int>(i);

		if (param[cv::ConnectedComponentsTypes::CC_STAT_AREA] >= area_th) {
			int left = param[cv::ConnectedComponentsTypes::CC_STAT_LEFT];
			int top = param[cv::ConnectedComponentsTypes::CC_STAT_TOP];
			int height = param[cv::ConnectedComponentsTypes::CC_STAT_HEIGHT];
			int width = param[cv::ConnectedComponentsTypes::CC_STAT_WIDTH];

			//cv::rectangle(dst, cv::Rect(left, top, width, height), cv::Scalar(0, 255, 0), 2);

			std::vector<int> c_h;
			std::vector<int> g_y;
			std::vector<int> candidate_c;


			//g_y���i�[
			for (int y = top; y < top + height; y++) {

				g_y.push_back(0);

				for (int x = left; x < left + width; x++) {
					if (src_color.ptr<cv::Vec3b>(y)[x] == colors[i])
						g_y[y] += src.ptr<float>(y)[x];
				}
			}

			//c_h���v�Z
			c_h.push_back(0); //c_h[0]�͎g��Ȃ�

			for (int y = 1; y < src.cols - 1; y++) {
				c_h.push_back(0);
				c_h[y] = 2 * g_y[y] - g_y[y + 1] - g_y[y - 1];

				//����T��
				candidate_c.push_back(0);
				if (c_h[y] > 0.1*width) {
					candidate_c[y] = c_h[y];
				}
			}

		}
	}


	////�d�S�̏o��

	//for (int i = 1; i < nLab; ++i) {

	//	int *param = stats.ptr<int>(i);
	//	double *centorids_param = centroids.ptr<double>(i);

	//	if (param[cv::ConnectedComponentsTypes::CC_STAT_AREA] >= area_th) {
	//		int x_center = static_cast<int>(centorids_param[0]);
	//		int y_center = static_cast<int>(centorids_param[1]);

	//		cv::circle(dst, cv::Point(x_center, y_center), 3, cv::Scalar(0, 0, 255), -1);

	//	}
	//}

	////�ʐϒl�̏o��
	//for (int i = 1; i < nLab; ++i) {
	//	int *param = stats.ptr<int>(i);

	//	if (param[cv::ConnectedComponentsTypes::CC_STAT_AREA] >= area_th) {
	//		//std::cout << "area " << i << " = " << param[cv::ConnectedComponentsTypes::CC_STAT_AREA] << std::endl;

	//		//ROI�̍���ɔԍ�����������
	//		int x = param[cv::ConnectedComponentsTypes::CC_STAT_LEFT];
	//		int y = param[cv::ConnectedComponentsTypes::CC_STAT_TOP];
	//		std::stringstream num;
	//		num << i;

	//		if (param[cv::ConnectedComponentsTypes::CC_STAT_AREA] >= area_th) {
	//			cv::putText(dst, num.str(), cv::Point(x + 5, y + 20), cv::FONT_HERSHEY_COMPLEX, 0.7, cv::Scalar(0, 255, 255), 2);
	//		}
	//	}
	//}



	cv::imshow("5 Labeling", dst);
}
