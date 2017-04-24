#include"Func.h"
#include<iostream>
#include<vector>
#include<random>
#include<string>
#include<strstream>

void PanelBlockSplitting(cv::Mat& src, cv::Mat& dst, const int MARGIN_WIDTH) {

	const int 	area_th = 500; //本当はSec2-2で書くつもり

	//srcを255に戻す
	src.convertTo(src, CV_8UC3, 255.0);

	//ラべリング処理
	cv::Mat LabelImg = cv::Mat::zeros(src.size(), CV_8UC3);    //ラベル画像
	cv::Mat stats = cv::Mat::zeros(src.size(), CV_8UC3);       //バウンディングボックスの範囲と面積値
	cv::Mat centroids = cv::Mat::zeros(src.size(), CV_8UC3);   //ラベル付けされ領域の重心(x,y)

	int nLab = cv::connectedComponentsWithStats(src, LabelImg, stats, centroids, 8, CV_32S);  //ラベル数 + 1

	// ラベリング結果の描画色を決定
	std::vector<cv::Vec3b> colors(nLab);
	colors[0] = cv::Vec3b(0, 0, 0);  //label[0]は使わない
	for (int i = 1; i < nLab; ++i) {
		int *param = stats.ptr<int>(i);

		//小さな領域は黒く塗る
		if (param[cv::ConnectedComponentsTypes::CC_STAT_AREA] < area_th) {
			colors[i] = cv::Vec3b(0, 0, 0);
		}
		else {
			colors[i] = cv::Vec3b((rand() & 255), (rand() & 255), (rand() & 255));
		}
	}

	// ラベリング結果の設定

	for (int i = 0; i < dst.rows; ++i) {
		for (int j = 0; j < dst.cols; ++j) {
			dst.ptr<cv::Vec3b>(i)[j] = colors[LabelImg.ptr<int>(i)[j]];
		}
	}

	cv::imshow("4 Labeling", dst);
	cv::waitKey(0);


	//ROI(Region of Interest)の描画

	//Floatに戻す
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


			//g_yを格納
			for (int y = top; y < top + height; y++) {

				g_y.push_back(0);

				for (int x = left; x < left + width; x++) {
					if (src_color.ptr<cv::Vec3b>(y)[x] == colors[i])
						g_y[y] += src.ptr<float>(y)[x];
				}
			}

			//c_hを計算
			c_h.push_back(0); //c_h[0]は使わない

			for (int y = 1; y < src.cols - 1; y++) {
				c_h.push_back(0);
				c_h[y] = 2 * g_y[y] - g_y[y + 1] - g_y[y - 1];

				//候補を探す
				candidate_c.push_back(0);
				if (c_h[y] > 0.1*width) {
					candidate_c[y] = c_h[y];
				}
			}

		}
	}


	////重心の出力

	//for (int i = 1; i < nLab; ++i) {

	//	int *param = stats.ptr<int>(i);
	//	double *centorids_param = centroids.ptr<double>(i);

	//	if (param[cv::ConnectedComponentsTypes::CC_STAT_AREA] >= area_th) {
	//		int x_center = static_cast<int>(centorids_param[0]);
	//		int y_center = static_cast<int>(centorids_param[1]);

	//		cv::circle(dst, cv::Point(x_center, y_center), 3, cv::Scalar(0, 0, 255), -1);

	//	}
	//}

	////面積値の出力
	//for (int i = 1; i < nLab; ++i) {
	//	int *param = stats.ptr<int>(i);

	//	if (param[cv::ConnectedComponentsTypes::CC_STAT_AREA] >= area_th) {
	//		//std::cout << "area " << i << " = " << param[cv::ConnectedComponentsTypes::CC_STAT_AREA] << std::endl;

	//		//ROIの左上に番号を書き込む
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
