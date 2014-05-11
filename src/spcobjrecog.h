#ifndef SPCOBJRECOG_H_
#define SPCOBJRECOG_H_

#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <ANN/ANN.h>
#include <vector>
#include <string>
#include <map>
#include "surf.h"
#include "ui_spcobjrecog.h"

class SpcObjRecog : public QMainWindow, Ui_MainWindow
{
    Q_OBJECT
    
public:
    SpcObjRecog();
    ~SpcObjRecog();
    
    void loadDataBase(const std::vector<std::string> &filepath);
    
protected:
    void timerEvent(QTimerEvent*);
    
private slots:
    void executeRecognition();
    
private:
    cv::Mat image_;
    cv::VideoCapture capture_;
    cv::SURF param_;
    std::vector<std::vector<std::pair<int, surf::feature> > > features_;
    std::vector<std::pair<int, std::string> > objects_;
    std::vector<ANNpointArray> dataPoints_;
    std::vector<ANNkd_tree*> kdTree_;
};

#endif
