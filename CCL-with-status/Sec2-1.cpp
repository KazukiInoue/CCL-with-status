#include "Func.h"
#include<iostream>
#include<vector>
#include<random>
#include<string>
#include<strstream>

//枠線かどうかを判定
void PanelBoundary(cv::Mat& src, const int  LINE_WIDTH) {

	int max[4] = {};
	int min[4] = {};
	int delta[4] = {};
	int dif[4] = {};
	int boundary_len[4] = {};

	//上下左右どこの判定をするかを決める

	//{xの始点、xの終点、yの始点、yの終点}
	int search_range[4][4] = {
		{ 0 , src.cols , 0 , 1 }, //余白が上
		{ 0 , src.cols , src.rows - 1 , src.rows }, //余白が下
		{ 0 , 1 , 0 , src.rows }, //余白が左
		{ src.cols - 1  , src.cols , 0, src.rows }, //余白が右
	};


	//最大値と最小値の計算

	//余白が上下の場合
	for (int i = 0; i <= 1; i++) {
		for (int y = search_range[i][2]; y < search_range[i][3]; y++) {
			for (int x = search_range[i][0]; x < search_range[i][1]; x++) {

				if (max[i] < x*src.ptr<float>(y)[x]) {
					max[i] = x*src.ptr<float>(y)[x];
				}
				if (min[i] > x*src.ptr<float>(y)[x]) {
					min[i] = x*src.ptr<float>(y)[x];
				}
			}
		}
	}

	//余白が左右の場合
	for (int i = 2; i <= 3; i++) {
		for (int y = search_range[i][2]; y < search_range[i][3]; y++) {
			for (int x = search_range[i][0]; x < search_range[i][1]; x++) {

				if (max[i] < y*src.ptr<float>(y)[x]) {
					max[i] = y*src.ptr<float>(y)[x];
				}
				if (min[i] > y*src.ptr<float>(y)[x]) {
					min[i] = y*src.ptr<float>(y)[x];
				}
			}
		}
	}

	//最大値と最小値の差分を計算
	for (int i = 0; i < 4; i++) {
		delta[i] = max[i] - min[i];
	}

	//条件を満たす線分を求める

	//余白が上下の場合
	for (int i = 0; i <= 1; i++) {
		for (int y = search_range[i][2]; y < search_range[i][3]; y++) {
			for (int x = search_range[i][0] + 1; x < search_range[i][1]; x++) {

				dif[i] += abs((src.ptr<float>(y)[x] - src.ptr<float>(y)[x - 1]));

				if (delta[i] > 0.7*x && dif[i] > 10) {
					boundary_len[i] = x + 1;
				}
			}
		}
	}

	//余白が左右の場合
	for (int i = 2; i <= 3; i++) {
		for (int y = search_range[i][2] + 1; y < search_range[i][3]; y++) {
			for (int x = search_range[i][0]; x < search_range[i][1]; x++) {

				dif[i] += abs((src.ptr<float>(y)[x] - src.ptr<float>(y - 1)[x]));

				if (delta[i] > 0.7*y && dif[i] > 10) {
					boundary_len[i] = y;
				}
			}
		}
	}

	//いよいよ！線を引来ますぞ！！
	cv::line(src, cv::Point(search_range[0][0], search_range[0][2]), cv::Point(boundary_len[0], search_range[0][2]), 0, LINE_WIDTH); //余白が上の時
	cv::line(src, cv::Point(search_range[1][0], search_range[1][2]), cv::Point(boundary_len[1], search_range[1][2]), 0, LINE_WIDTH); //余白が下の時
	cv::line(src, cv::Point(search_range[2][0], search_range[2][2]), cv::Point(search_range[2][0], boundary_len[2]), 0, LINE_WIDTH); //余白が左の時
	cv::line(src, cv::Point(search_range[3][0], search_range[3][2]), cv::Point(search_range[3][0], boundary_len[3]), 0, LINE_WIDTH); //余白が右の時
}










void ExecuteLabeling(cv::Mat& src, cv::Mat& dst) {

	const int area_th = src.rows*src.cols / 2830; //ひとまずこの値((1055+10)×(671+10)だと250くらい)

	//255に戻す
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

	//ROI(Region of Interest)の描画
	for (int i = 1; i < nLab; ++i) {
		int *param = stats.ptr<int>(i);

		int x_start = param[cv::ConnectedComponentsTypes::CC_STAT_LEFT];
		int y_start = param[cv::ConnectedComponentsTypes::CC_STAT_TOP];
		int height = param[cv::ConnectedComponentsTypes::CC_STAT_HEIGHT];
		int width = param[cv::ConnectedComponentsTypes::CC_STAT_WIDTH];

		//cv::rectangle(dst, cv::Rect(x_start, y_start, width, height), cv::Scalar(0, 255, 0), 2);
	}


	////重心の出力
	//for (int i = 1; i < nLab; ++i) {
	//	double *param = centroids.ptr<double>(i);
	//	int x = static_cast<int>(param[0]);
	//	int y = static_cast<int>(param[1]);

	//	cv::circle(dst, cv::Point(x, y), 3, cv::Scalar(0, 0, 255), -1);
	//}

	//面積値の出力
	int max_area = 0;

	for (int i = 1; i < nLab; ++i) {
		int *param = stats.ptr<int>(i);
		//std::cout << "area " << i << " = " << param[cv::ConnectedComponentsTypes::CC_STAT_AREA] << std::endl;

		//ROIの左上に番号を書き込む
		int x = param[cv::ConnectedComponentsTypes::CC_STAT_LEFT];
		int y = param[cv::ConnectedComponentsTypes::CC_STAT_TOP];
		std::stringstream num;
		num << i;

		//if (param[cv::ConnectedComponentsTypes::CC_STAT_AREA] >= area_th) {
		//	cv::putText(dst, num.str(), cv::Point(x + 5, y + 20), cv::FONT_HERSHEY_COMPLEX, 0.7, cv::Scalar(0, 255, 255), 2);
		//}


		//バウンディングボックスが最大の領域を探す
		if (max_area < param[cv::ConnectedComponentsTypes::CC_STAT_AREA]) {
			max_area = param[cv::ConnectedComponentsTypes::CC_STAT_AREA];
		}
	}

	//バウンディングボックスの面積が最大でなければ黒く塗る
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

	//Floatに戻す
	src.convertTo(src, CV_32F, 1 / 255.0);
}