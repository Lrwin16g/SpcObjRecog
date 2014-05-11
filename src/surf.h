#ifndef SURF_H_
#define SURF_H_

#include <opencv2/nonfree/nonfree.hpp>
#include <vector>

namespace surf
{
    
struct feature
{
    cv::KeyPoint keypoint;
    std::vector<float> descriptor;
};

std::vector<feature> extract(const cv::Mat &image, const cv::SURF &param);

std::vector<feature> loadfile(const char *filename);

void savefile(const char *filename, const std::vector<feature> &features);

}

#endif
