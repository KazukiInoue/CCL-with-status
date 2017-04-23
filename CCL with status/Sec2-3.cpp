#include "Func.h"
#include<iostream>
#include<vector>
#include<random>
#include<string>
#include<strstream>

void PanelShapeExtraction(cv::Mat& src, cv::Mat& dst) {

	const int 	area_th = 200; //本当はSec2-2で書くつもり

    //255に戻す
	src.convertTo(src, CV_8UC3, 255.0);

	//ラべリング処理
	cv::Mat LabelImg = cv::Mat::zeros(src.size(), CV_8UC3);    //ラベル画像
	cv::Mat stats = cv::Mat::zeros(src.size(), CV_8UC3);       //バウンディングボックスの範囲と面積値
	cv::Mat centroids = cv::Mat::zeros(src.size(), CV_8UC3);   //重心(x,y)



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
	for (int i = 1; i < nLab; ++i) {
		int *param = stats.ptr<int>(i);

		if (param[cv::ConnectedComponentsTypes::CC_STAT_AREA] >= area_th) {
			int x_start = param[cv::ConnectedComponentsTypes::CC_STAT_LEFT];
			int y_start = param[cv::ConnectedComponentsTypes::CC_STAT_TOP];
			int height = param[cv::ConnectedComponentsTypes::CC_STAT_HEIGHT];
			int width = param[cv::ConnectedComponentsTypes::CC_STAT_WIDTH];

			cv::rectangle(dst, cv::Rect(x_start, y_start, width, height), cv::Scalar(0, 255, 0), 2);
		}
	}


	//重心の出力

	for (int i = 1; i < nLab; ++i) {

		double *param = centroids.ptr<double>(i);

		if (param[cv::ConnectedComponentsTypes::CC_STAT_AREA] >= area_th) {
			int x = static_cast<int>(param[0]);
			int y = static_cast<int>(param[1]);

			cv::circle(dst, cv::Point(x, y), 3, cv::Scalar(0, 0, 255), -1);
		}
	}

	//面積値の出力
	int max_area = 0;

	for (int i = 1; i < nLab; ++i) {
		int *param = stats.ptr<int>(i);

		if (param[cv::ConnectedComponentsTypes::CC_STAT_AREA] >= area_th) {
			std::cout << "area " << i << " = " << param[cv::ConnectedComponentsTypes::CC_STAT_AREA] << std::endl;

			//ROIの左上に番号を書き込む
			int x = param[cv::ConnectedComponentsTypes::CC_STAT_LEFT];
			int y = param[cv::ConnectedComponentsTypes::CC_STAT_TOP];
			std::stringstream num;
			num << i;

			if (param[cv::ConnectedComponentsTypes::CC_STAT_AREA] >= area_th) {
				cv::putText(dst, num.str(), cv::Point(x + 5, y + 20), cv::FONT_HERSHEY_COMPLEX, 0.7, cv::Scalar(0, 255, 255), 2);
			}


			//バウンディングボックスが最大の領域を探す
			if (max_area < param[cv::ConnectedComponentsTypes::CC_STAT_AREA]) {
				max_area = param[cv::ConnectedComponentsTypes::CC_STAT_AREA];
			}
		}
	}

	//Floatに戻す
	src.convertTo(src, CV_32F, 1 / 255.0);

	cv::imshow("5 Labeling", dst);
	cv::waitKey(0);
}