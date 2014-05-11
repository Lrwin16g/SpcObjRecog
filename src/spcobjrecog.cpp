#include "spcobjrecog.h"
#include <opencv2/core/core.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <QImage>
#include <QString>
#include <iostream>
#include <ctime>

const int kNum = 1;

SpcObjRecog::SpcObjRecog()
    : image_(), capture_(0), param_(500, 4, 2, true)
{
    setupUi(this);
    
    capture_.set(CV_CAP_PROP_FRAME_WIDTH, 640);
    capture_.set(CV_CAP_PROP_FRAME_HEIGHT, 480);
    
    features_.resize(3);
    dataPoints_.resize(3);
    kdTree_.resize(3);
    
    connect(recogButton, SIGNAL(clicked()), this, SLOT(executeRecognition()));
    connect(epsSpinBox, SIGNAL(valueChanged(int)), epsSlider, SLOT(setValue(int)));
    connect(epsSlider, SIGNAL(valueChanged(int)), epsSpinBox, SLOT(setValue(int)));
    
    startTimer(100);
}

SpcObjRecog::~SpcObjRecog()
{
    annClose();
}

void SpcObjRecog::loadDataBase(const std::vector<std::string> &filepath)
{
    for (size_t i = 0; i < filepath.size(); ++i) {
	int key = i;
	std::vector<surf::feature> feature = surf::loadfile((filepath[i] + ".surf").c_str());
	for (size_t j = 0; j < feature.size(); ++j) {
	    std::pair<int, surf::feature> elem(key, feature[j]);
	    int class_id = feature[j].keypoint.class_id + 1;
	    features_[class_id].push_back(elem);
	}
	std::pair<int, std::string> elem(key, filepath[i] + ".jpg");
	objects_.push_back(elem);
    }
    
    for (int i = 0; i < 3; ++i) {
	int pointsNum = static_cast<int>(features_[i].size());
	dataPoints_[i] = annAllocPts(pointsNum * 2, param_.descriptorSize());
	for (size_t j = 0; j < features_[i].size(); ++j)	{
	    for (int k = 0; k < param_.descriptorSize(); ++k) {
		dataPoints_[i][j][k] = features_[i][j].second.descriptor[k];
	    }
	}
	kdTree_[i] = new ANNkd_tree(dataPoints_[i], pointsNum, param_.descriptorSize());
    }
}

void SpcObjRecog::timerEvent(QTimerEvent*)
{
    cv::Mat frame;
    capture_ >> frame;
    cv::cvtColor(frame, image_, CV_RGBA2BGR);
    QImage labelImage(reinterpret_cast<uchar*>(image_.data), image_.cols, image_.rows, QImage::Format_RGB888);
    
    imageLabel->setPixmap(QPixmap::fromImage(labelImage.scaled(imageLabel->width(), imageLabel->height())));
}

void SpcObjRecog::executeRecognition()
{
    // クエリ画像作成（グレースケール）
    cv::Mat queryImage;
    cv::cvtColor(image_, queryImage, CV_BGR2GRAY);
    
    // 描画用
    cv::Mat paintImage = image_.clone();
    
    // SURF特徴量抽出
    std::vector<surf::feature> surf_features = surf::extract(queryImage, param_);
    
    // SURF特徴量抽出結果を描画
    for (size_t i = 0; i < surf_features.size(); ++i) {
	cv::Point center(surf_features[i].keypoint.pt.x, surf_features[i].keypoint.pt.y);
	int radius = cv::saturate_cast<int>(surf_features[i].keypoint.size * 1.2 / 9.0 * 2.0);
	cv::circle(paintImage, center, radius, cv::Scalar(255, 255, 0), 1, 8, 0);
    }
    QImage extractImage(reinterpret_cast<uchar*>(paintImage.data), paintImage.cols, paintImage.rows, QImage::Format_RGB888);
    featureLabel->setPixmap(QPixmap::fromImage(extractImage.scaled(featureLabel->width(), featureLabel->height(), Qt::KeepAspectRatio, Qt::SmoothTransformation)));
    
    // 投票用配列作成
    std::vector<std::pair<int, int> > voteNum(objects_.size());
    for (size_t i = 0; i < voteNum.size(); ++i) {
	voteNum[i] = std::make_pair(0, i);
    }
    
    // ANNの準備
    ANNidxArray nnIdx = new ANNidx[kNum];
    ANNdistArray dists = new ANNdist[kNum];
    
    // 処理時間計測開始
    clock_t start = clock();
    
    // 抽出したSURFの数だけまわす
    for (size_t i = 0; i < surf_features.size(); ++i) {
	
	// ANN用のクエリデータ作成
	ANNpoint queryPt = annAllocPt(param_.descriptorSize());
	for (int j = 0; j < param_.descriptorSize(); ++j) {
	    queryPt[j] = surf_features[i].descriptor[j];
	}
	
	// 近似最近傍探索実行
	int laplacian = surf_features[i].keypoint.class_id + 1;
	kdTree_[laplacian]->annkSearch(queryPt, kNum, nnIdx, dists, static_cast<double>(epsSpinBox->value()));
	
	// 最近傍の特徴量データのオブジェクトに投票
	int key = features_[laplacian][nnIdx[0]].first;
	voteNum[key].first++;
    }
    
    // 処理時間計測終了
    clock_t end = clock();
    float time = static_cast<float>(end - start) / CLOCKS_PER_SEC;
    QString str_time = QString::number(time) + "[sec]";
    processTimeLabel->setText(str_time);
    
    // 投票数の多い順にソート
    std::sort(voteNum.begin(), voteNum.end(), std::greater<std::pair<int, int> >());
    
    // 開票
    // 1st
    QString candidate_1(objects_[voteNum[0].second].second.c_str());
    float votePer_1 = static_cast<float>(voteNum[0].first) / surf_features.size() * 100.0;
    resultLabel_1->setPixmap(QPixmap::fromImage(QImage(candidate_1).scaled(resultLabel_1->width(), resultLabel_1->height(), Qt::KeepAspectRatio, Qt::SmoothTransformation)));
    votePerLabel_1->setText(QString::number(votePer_1) + "%");
    // 2nd
    QString candidate_2(objects_[voteNum[1].second].second.c_str());
    float votePer_2 = static_cast<float>(voteNum[1].first) / surf_features.size() * 100.0;
    resultLabel_2->setPixmap(QPixmap::fromImage(QImage(candidate_2).scaled(resultLabel_2->width(), resultLabel_2->height(), Qt::KeepAspectRatio, Qt::SmoothTransformation)));
    votePerLabel_2->setText(QString::number(votePer_2) + "%");
    // 3rd
    QString candidate_3(objects_[voteNum[2].second].second.c_str());
    float votePer_3 = static_cast<float>(voteNum[2].first) / surf_features.size() * 100.0;
    resultLabel_3->setPixmap(QPixmap::fromImage(QImage(candidate_3).scaled(resultLabel_3->width(), resultLabel_3->height(), Qt::KeepAspectRatio, Qt::SmoothTransformation)));
    votePerLabel_3->setText(QString::number(votePer_3) + "%");
    
    delete[] nnIdx;
    delete[] dists;
}
