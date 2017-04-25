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

			cv::rectangle(dst, cv::Rect(left, top, width, height), cv::Scalar(0, 255, 0), 2);

			std::vector<int> c_h;
			std::vector<int> cc_v;
			std::vector<int> g_y;
			std::vector<int> gg_x;
			std::vector<bool> candidate_s_y;
			std::vector<bool> candidate_ss_x;

			cv::imshow("start splitting", src);
			cv::waitKey(0);

			//g_y���i�[
			for (int y = 0; y < height + 2; y++) {
				g_y.push_back(0);

				src.convertTo(src, CV_8UC3, 255);
				cv::line(src, cv::Point(left, y), cv::Point(left + width, y), CV_RGB(255, 255, 0), 1);

				cv::imshow("start splitting", src);
				cv::waitKey(0);
				src.convertTo(src, CV_32F, 1 / 255.0);
				for (int x = left; x < left + width; x++) {
					if (src_color.ptr<cv::Vec3b>(y)[x] == colors[i])
						g_y[y] += src.ptr<float>(y + top - 1)[x];
					if (g_y[y] == 1.0f) {
						std::cout << y << " " << g_y[y] << std::endl;
					}


				}
			}

			std::cout << "�I���!" << std::endl;
			system("pause");

			//g_x���i�[
			for (int x = 0; x < left + 2; x++) {
				gg_x.push_back(0);
				for (int y = top; y < top + height; y++) {
					if (src_color.ptr<cv::Vec3b>(y)[x] == colors[i])
						gg_x[x] += src.ptr<float>(y)[x + left - 1];
				}
			}

			//c_h���v�Z
			c_h.push_back(0); //c_h[0]�͎g��Ȃ�

			for (int y = 0; y < height; y++) {
				c_h.push_back(0);
				c_h[y] = 2 * (1 - g_y[y + 1]) - (1 - g_y[y + 2]) - (1 - g_y[y]);  //�_���Ǝ����قȂ邱�Ƃɒ��ӁI�I�i�����ł͗]�������̂��߁j

				//����T��
				candidate_s_y.push_back(false);
				std::cout << i << " " << y << " " << c_h[y] << std::endl;
				if (c_h[y] > 0.1*width) {
					candidate_s_y[y] = true; //candidate_s_y[0]�͎g��Ȃ�
				}
				else {
					candidate_s_y[y] = false;
				}
			}

			//c_v���v�Z
			for (int x = 0; x < left; x++) {
				cc_v.push_back(0);
				cc_v[x] = 2 * (1 - gg_x[x + 1]) - (1 - gg_x[x + 2]) - (1 - gg_x[x]);  //�_���Ǝ����قȂ邱�Ƃɒ��ӁI�I�i�����ł͗]�������̂��߁j

				//����T��
				candidate_ss_x.push_back(false);
				if (cc_v[x] > 0.1*width) {
					candidate_ss_x[x] = true;
				}
				else {
					candidate_ss_x[x] = false;
				}
			}

			//���������Ă݂�
			for (int y = 0; y < height; y++) {
				if (candidate_s_y[y] == true) {
					cv::line(dst, cv::Point(left, y), cv::Point(width, y), CV_RGB(255, 0, 0), 10);
				}
			}
		}
	}

	cv::imshow("Draw line", dst);
	cv::waitKey(0);

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



	cv::imshow("After Splitting", dst);
	cv::waitKey(0);
}
