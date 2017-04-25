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

			cv::rectangle(dst, cv::Rect(left, top, width, height), cv::Scalar(0, 255, 0), 2);

			std::vector<int> c_h;
			std::vector<int> cc_v;
			std::vector<int> g_y;
			std::vector<int> gg_x;
			std::vector<bool> candidate_s_y;
			std::vector<bool> candidate_ss_x;

			cv::imshow("start splitting", src);
			cv::waitKey(0);

			//g_yを格納
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

			std::cout << "終わり!" << std::endl;
			system("pause");

			//g_xを格納
			for (int x = 0; x < left + 2; x++) {
				gg_x.push_back(0);
				for (int y = top; y < top + height; y++) {
					if (src_color.ptr<cv::Vec3b>(y)[x] == colors[i])
						gg_x[x] += src.ptr<float>(y)[x + left - 1];
				}
			}

			//c_hを計算
			c_h.push_back(0); //c_h[0]は使わない

			for (int y = 0; y < height; y++) {
				c_h.push_back(0);
				c_h[y] = 2 * (1 - g_y[y + 1]) - (1 - g_y[y + 2]) - (1 - g_y[y]);  //論文と式が異なることに注意！！（ここでは余白が黒のため）

				//候補を探す
				candidate_s_y.push_back(false);
				std::cout << i << " " << y << " " << c_h[y] << std::endl;
				if (c_h[y] > 0.1*width) {
					candidate_s_y[y] = true; //candidate_s_y[0]は使わない
				}
				else {
					candidate_s_y[y] = false;
				}
			}

			//c_vを計算
			for (int x = 0; x < left; x++) {
				cc_v.push_back(0);
				cc_v[x] = 2 * (1 - gg_x[x + 1]) - (1 - gg_x[x + 2]) - (1 - gg_x[x]);  //論文と式が異なることに注意！！（ここでは余白が黒のため）

				//候補を探す
				candidate_ss_x.push_back(false);
				if (cc_v[x] > 0.1*width) {
					candidate_ss_x[x] = true;
				}
				else {
					candidate_ss_x[x] = false;
				}
			}

			//線を引いてみる
			for (int y = 0; y < height; y++) {
				if (candidate_s_y[y] == true) {
					cv::line(dst, cv::Point(left, y), cv::Point(width, y), CV_RGB(255, 0, 0), 10);
				}
			}
		}
	}

	cv::imshow("Draw line", dst);
	cv::waitKey(0);

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

	//面積値の出力
	for (int i = 1; i < nLab; ++i) {
		int *param = stats.ptr<int>(i);

		if (param[cv::ConnectedComponentsTypes::CC_STAT_AREA] >= area_th) {
			//std::cout << "area " << i << " = " << param[cv::ConnectedComponentsTypes::CC_STAT_AREA] << std::endl;

			//ROIの左上に番号を書き込む
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
