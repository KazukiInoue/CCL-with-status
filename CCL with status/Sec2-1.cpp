#include "Func.h"
#include<iostream>
#include<vector>
#include<random>
#include<string>
#include<strstream>

void PanelBoundary(cv::Mat& src, bool up, bool bottom, bool left, bool right, const int  LINE_WIDTH) {

	//枠線かどうかを判定

	int max = 0; //画素の最大値
	int min = 0; //画素の最小値
	int delta = 0; //最大値と最小値の差分
	int dif = 0; //画素の微分
	int fix = 0; //探索の始点のx or y
	int end = 0;   //探索の終点のy or x
	int boundary_len = 0; //求めるLe

	//上下左右どこの判定をするかを決める
	//余白が上の場合
	if (up == 1 && bottom == 0 && left == 0 && right == 0) {
		fix = 0;
		end = src.cols;
	}
	//余白が下の場合
	if (up == 0 && bottom == 1 && left == 0 && right == 0) {
		fix = src.rows - 1;
		end = src.cols;
	}
	//余白が左の場合
	if (up == 0 && bottom == 0 && left == 1 && right == 0) {
		fix = 0;
		end = src.rows;
	}
	//余白が右の場合
	if (up == 0 && bottom == 0 && left == 0 && right == 1) {
		fix = src.cols - 1;
		end = src.rows;
	}

	//最大値と最小値の計算

	//余白が上下の場合
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
	//余白が左右の場合
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

	//最大値と最小値の差分を計算
	delta = max - min;

	//余白が上下の場合
	if (left == 0 && right == 0) {
		for (int x = 1; x < end; x++) {
			dif += abs((src.ptr<float>(fix)[x] - src.ptr<float>(fix)[x - 1]));
			//std::cout << "(" << x << "," << fix << ")" << ", dif = " << dif << std::endl;
			if (delta > 0.7*x && dif > 10) {
				boundary_len = x + 1;
			}
		}
	}
	//余白が左右の場合
	if (up == 0 && bottom == 0) {
		for (int y = 1; y < end; y++) {
			dif += abs((src.ptr<float>(y)[fix] - src.ptr<float>(y - 1)[fix]));
			//std::cout << "y = " << y << ", dif =" << dif << std::endl;
			if (delta > 0.7*y && dif > 10) {
				boundary_len = y + 1;
			}
		}
	}

	//線を引く
	//余白が上下の場合
	if (left == 0 && right == 0) {
		cv::line(src, cv::Point(0, fix), cv::Point(boundary_len, fix), 0, LINE_WIDTH);
	}
	//余白が左右の場合
	if (up == 0 && bottom == 0) {
		cv::line(src, cv::Point(fix, 0), cv::Point(fix, boundary_len), 0, LINE_WIDTH);
	}
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
	cv::waitKey(0);

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