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
	//グレースケール入力
	cv::Mat input = cv::imread("C:\\Users\\LAB\\Pictures\\naruto_514\\2_remove.jpg", cv::IMREAD_GRAYSCALE);
	//cv::Mat input = cv::imread("C:/Users/LAB/Pictures/white.jpg", cv::IMREAD_GRAYSCALE);

	// 画像の読み込みに失敗したらエラー終了する
	if (input.empty()) {
		std::cerr << "Failed to open image file." << std::endl;
		system("pause");
		return -1;
	}

	//リサイズ
	const double ORIGINAL_RESIZE = 800 / (double)input.rows;
	cv::resize(input, input, cv::Size(), ORIGINAL_RESIZE, ORIGINAL_RESIZE);

	cv::Mat src = input.clone();

	//Sec2-1 Handling unclosed panel

	//二値化
	cv::threshold(src, src, 235, 255, cv::THRESH_BINARY);

	//スケーリング
	src.convertTo(src, CV_32F, 1.0 / 255.0);

	//枠線かどうかを判定
	PanelBoundary(src, LINE_WIDTH);

	//addmを白で初期化
	cv::Mat addm = cv::Mat::zeros(cv::Size(src.cols + 2 * MARGIN_WIDTH, src.rows + 2 * MARGIN_WIDTH), cv::IMREAD_GRAYSCALE);
	//ネガポジ反転
	addm = ~addm;
	//スケーリング
	addm.convertTo(addm, CV_32F, 1.0 / 255.0);
	//余白を足す
	for (int y = 0; y < src.rows; y++) {
		for (int x = 0; x < src.cols; x++) {
			addm.ptr<float>(y + MARGIN_WIDTH)[x + MARGIN_WIDTH] = src.ptr<float>(y)[x];
		}
	}

	cv::imshow("After func", addm);
	cv::waitKey(0);

	//ラべリング処理
	cv::Mat daubing = cv::Mat::zeros(addm.size(), CV_8UC3);
	ExecuteLabeling(addm, daubing);

	cv::imshow("Input", input);
	cv::imshow("Labels", daubing);
	cv::waitKey();

	//PanelBoundaryで書いた枠線を取り除く
//スケーリング
	cv::cvtColor(daubing, daubing, CV_BGR2GRAY);
	daubing.convertTo(daubing, CV_32F, 1.0 / 255.0);

	std::cout << "start" << std::endl;
	for (int x = 0; x < input.cols; x++) {
		std::cout << "[上、真ん中、下] = "
			<< "[" << daubing.ptr<float>(MARGIN_WIDTH - 1)[x + MARGIN_WIDTH]
			<< ", " << daubing.ptr<float>(MARGIN_WIDTH)[x + MARGIN_WIDTH]
			<< ", " << daubing.ptr<float>(MARGIN_WIDTH + 1)[x + MARGIN_WIDTH]
			<< "]" << std::endl;
	}

	//system("pause");

	//上下の枠線

	for (int x = 0; x < daubing.cols; x++) {

		if (daubing.ptr<float>(MARGIN_WIDTH - 1)[x] == 1.0f
			&& daubing.ptr<float>(MARGIN_WIDTH)[x] == 0.0f
			&& daubing.ptr<float>(MARGIN_WIDTH + 1)[x] == 1.0f) {

			daubing.ptr<float>(MARGIN_WIDTH)[x] == 1.0f;


		}

		std::cout << "[上、真ん中、下] = "
			<< "[" << daubing.ptr<float>(MARGIN_WIDTH - 1)[x]
			<< ", " << daubing.ptr<float>(MARGIN_WIDTH)[x]
			<< ", " << daubing.ptr<float>(MARGIN_WIDTH + 1)[x]
			<< "]" << std::endl;



		if (daubing.ptr<float>(input.rows - 1 + MARGIN_WIDTH - 1)[x] == 1.0f
			&& daubing.ptr<float>(input.rows - 1 + MARGIN_WIDTH)[x] == 0.0f
			&& daubing.ptr<float>(input.rows - 1 + MARGIN_WIDTH + 1)[x] == 1.0f) {

			daubing.ptr<float>(input.rows - 1 + MARGIN_WIDTH)[x] == 1.0f;
		}

	}

	//左右の枠線
	for (int y = 0; y < daubing.rows; y++) {

		if (daubing.ptr<float>(y)[MARGIN_WIDTH - 1] == 1.0f
			&& daubing.ptr<float>(y)[MARGIN_WIDTH] == 0.0f
			&& daubing.ptr<float>(y)[MARGIN_WIDTH + 1] == 1.0f) {

			daubing.ptr<float>(y)[MARGIN_WIDTH] == 1.0f;
		}
		if (daubing.ptr<float>(y)[input.cols - 1 + MARGIN_WIDTH - 1] == 1.0f
			&& daubing.ptr<float>(y)[input.cols - 1 + MARGIN_WIDTH] == 0.0f
			&& daubing.ptr<float>(y)[input.cols - 1 + MARGIN_WIDTH + 1] == 1.0f) {

			daubing.ptr<float>(input.rows - 1 + MARGIN_WIDTH)[y] == 1.0f;
		}
	}

	/*cv::line(daubing, cv::Point(MARGIN_WIDTH, MARGIN_WIDTH), cv::Point(daubing.cols - MARGIN_WIDTH, MARGIN_WIDTH), CV_RGB(255, 255, 255), LINE_WIDTH + 1);
	cv::line(daubing, cv::Point(MARGIN_WIDTH, MARGIN_WIDTH), cv::Point(MARGIN_WIDTH, daubing.rows - MARGIN_WIDTH), CV_RGB(255, 255, 255), LINE_WIDTH + 1);
	cv::line(daubing, cv::Point(MARGIN_WIDTH, daubing.rows - MARGIN_WIDTH), cv::Point(daubing.cols - MARGIN_WIDTH, daubing.rows - MARGIN_WIDTH), CV_RGB(255, 255, 255), LINE_WIDTH + 1);
	cv::line(daubing, cv::Point(daubing.cols - MARGIN_WIDTH, MARGIN_WIDTH), cv::Point(daubing.cols - MARGIN_WIDTH, daubing.rows - MARGIN_WIDTH), CV_RGB(255, 255, 255), LINE_WIDTH + 1);
*/
	cv::imshow("remove white", daubing);
	cv::imshow("src", src);
	cv::waitKey(0);



	//Sec2-2 Panel Block Splitting



	//Sec2-3 Panel Shape Extraction

	daubing = ~daubing;



	//パネルの枠線を切り出す
	cv::Mat shape = cv::Mat::zeros(daubing.size(), CV_8UC3);
	cv::Mat dst = input.clone();
	cvtColor(dst, dst, CV_GRAY2RGB);

	PanelShapeExtraction(daubing, shape, dst, MARGIN_WIDTH);

	return 0;

}