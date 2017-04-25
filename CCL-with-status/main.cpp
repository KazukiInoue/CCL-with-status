#include"Func.h"
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
	//cv::Mat input = cv::imread("C:\\Users\\LAB\\Pictures\\naruto_514\\3_remove.jpg", cv::IMREAD_GRAYSCALE);
	cv::Mat input = cv::imread("C:\\Users\\LAB\\Pictures\\Ghoul01.jpg", cv::IMREAD_GRAYSCALE);

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

	/*std::cout << "addm start" << std::endl;
	for (int x = 0; x < addm.cols; x++) {
		if (addm.ptr<float>(MARGIN_WIDTH)[x] == 0.0f) {
			std::cout << x << std::endl;
		}
	}*/

	//ラべリング処理
	cv::Mat daubing = cv::Mat::zeros(addm.size(), CV_8UC3);
	ExecuteLabeling(addm, daubing);

	cv::imshow("Input", input);
	cv::imshow("Labels", daubing);

	//PanelBoundaryで書いた枠線を取り除く

	//daubing.convertTo(daubing, CV_32F, 1.0 / 255.0);

	//上下の枠線

	//std::cout << "start" << std::endl;

	//for (int x = 0; x < daubing.cols; x++) {
	//	if (daubing.ptr<float>(daubing.rows - MARGIN_WIDTH -1)[x] == 0.0f) {
	//		std::cout << x << std::endl;
	//	}
	//}
	//system("pause");

	////cv::line(daubing, cv::Point(10,0), cv::Point(10, daubing.rows), CV_RGB(255,0,0), LINE_WIDTH);    
	////cv::line(daubing, cv::Point(30, 0), cv::Point(30, daubing.rows), CV_RGB(0, 255, 255), LINE_WIDTH);


	//cv::imshow("Lines", daubing);
	//std::cout << "addm [cols, rows] = [" << addm.cols << " , " << addm.rows << "]"
	//	<< std::endl
	//	<< "daubing [cols, rows] = [" << daubing.cols << " , " << daubing.rows << "]"
	//	<< std::endl;


	//for (int x = 30; x < daubing.cols; x++) {
	//	daubing.ptr<float>(MARGIN_WIDTH)[x] = 1.0f;
	//}

	//cv::imshow("FROM 30", daubing);

	//cv::waitKey(0);

	//for (int x = 0; x < daubing.cols; x++) {

	//	//上の枠線
	//	if (//daubing.ptr<float>(MARGIN_WIDTH - 1)[x] == 1.0f&&
	//		daubing.ptr<float>(MARGIN_WIDTH)[x] == 0.0f
	//		&& daubing.ptr<float>(MARGIN_WIDTH + 1)[x] == 1.0f) {

	//		daubing.ptr<float>(MARGIN_WIDTH)[x] == 1.0f;

	//		std::cout << x << std::endl;
	//	}

	//	std::cout << daubing.ptr<float>(MARGIN_WIDTH - 1)[x]
	//		<< " " << daubing.ptr<float>(MARGIN_WIDTH)[x]
	//		<< " " << daubing.ptr<float>(MARGIN_WIDTH + 1)[x]
	//		<< std::endl;


	//	if (daubing.ptr<float>(daubing.rows - MARGIN_WIDTH - 1 - 1)[x] == 1.0f
	//		&& daubing.ptr<float>(daubing.rows - MARGIN_WIDTH - 1)[x] == 0.0f
	//		&& daubing.ptr<float>(daubing.rows - MARGIN_WIDTH - 1 + 1)[x] == 1.0f) {

	//		daubing.ptr<float>(daubing.rows - MARGIN_WIDTH - 1)[x] == 1.0f;
	//	}

	//}

	////左右の枠線
	//for (int y = MARGIN_WIDTH; y < daubing.rows - MARGIN_WIDTH; y++) {

	//	//左の枠線
	//	if (daubing.ptr<float>(y)[MARGIN_WIDTH - 1] == 1.0f
	//		&& daubing.ptr<float>(y)[MARGIN_WIDTH] == 0.0f
	//		&& daubing.ptr<float>(y)[MARGIN_WIDTH + 1] == 1.0f) {

	//		daubing.ptr<float>(y)[MARGIN_WIDTH] == 1.0f;
	//	}
	//	//右の枠線
	//	if (daubing.ptr<float>(y)[daubing.cols - MARGIN_WIDTH - 1 - 1] == 1.0f
	//		&& daubing.ptr<float>(y)[daubing.cols - MARGIN_WIDTH - 1] == 0.0f
	//		&& daubing.ptr<float>(y)[daubing.cols - MARGIN_WIDTH - 1 + 1] == 1.0f) {

	//		daubing.ptr<float>(daubing.rows - MARGIN_WIDTH - 1)[y] == 1.0f;
	//	}
	//}

	//daubing.convertTo(daubing, CV_8UC3, 255.0);

	cv::line(daubing, cv::Point(MARGIN_WIDTH, MARGIN_WIDTH), cv::Point(daubing.cols - MARGIN_WIDTH - 1, MARGIN_WIDTH), CV_RGB(255, 255, 255), LINE_WIDTH);                                      //上の枠線
	cv::line(daubing, cv::Point(MARGIN_WIDTH, daubing.rows - MARGIN_WIDTH - 1), cv::Point(daubing.cols - MARGIN_WIDTH - 1, daubing.rows - MARGIN_WIDTH - 1), CV_RGB(255, 255, 255), LINE_WIDTH); //下の枠線
	cv::line(daubing, cv::Point(MARGIN_WIDTH, MARGIN_WIDTH), cv::Point(MARGIN_WIDTH, daubing.rows - MARGIN_WIDTH - 1), CV_RGB(255, 255, 255), LINE_WIDTH);                                        //左の枠線
	cv::line(daubing, cv::Point(daubing.cols - MARGIN_WIDTH - 1, MARGIN_WIDTH), cv::Point(daubing.cols - MARGIN_WIDTH - 1, daubing.rows - MARGIN_WIDTH), CV_RGB(255, 255, 255), LINE_WIDTH);    //右の枠線

	cv::imshow("remove white", daubing);

	cv::waitKey(0);

	//Sec2-2 Panel Block Splitting

	//Sec2-1での出力だったdaubingをSec2-2の入力にする



	daubing = ~daubing;

	cv::imshow("Negapoji", daubing);
	cv::waitKey(0);
	ChangeOutput2Input(daubing);

	cv::Mat split = cv::Mat::zeros(daubing.size(), CV_8UC3); //Sec2-2での出力

	PanelBlockSplitting(daubing, split, MARGIN_WIDTH);

	////Sec2-3 Panel Shape Extraction

	////Sec2-1での出力だったsplitをSec2-2の入力にする
	//ChangeOutput2Input(split);

	////パネルの枠線を切り出す
	//cv::Mat shape = cv::Mat::zeros(split.size(), CV_8UC3);
	//cv::Mat dst = input.clone();
	//cvtColor(dst, dst, CV_GRAY2RGB);

	//PanelShapeExtraction(split, shape, dst, MARGIN_WIDTH);

	return 0;

}