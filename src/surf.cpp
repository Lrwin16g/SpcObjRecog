#include "surf.h"
#include <fstream>

std::vector<surf::feature> surf::extract(const cv::Mat &image, const cv::SURF &param)
{
    std::vector<cv::KeyPoint> keypoints;
    std::vector<float> descriptors;
    
    param(image, cv::Mat(), keypoints, descriptors);
    
    std::vector<surf::feature> features(keypoints.size());
    for (size_t i = 0; i < keypoints.size(); ++i) {
	features[i].keypoint = keypoints[i];
	features[i].descriptor.resize(param.descriptorSize());
	for (int j = 0; j < param.descriptorSize(); ++j) {
	    features[i].descriptor[j] = descriptors[i * param.descriptorSize() + j];
	}
    }
    
    return features;
}

std::vector<surf::feature> surf::loadfile(const char *filename)
{
    std::ifstream ifs(filename, std::ios::binary);
    
    size_t featuresNum;
    ifs.read(reinterpret_cast<char*>(&featuresNum), sizeof(size_t));
    
    std::vector<surf::feature> features(featuresNum);
    
    for (size_t i = 0; i < featuresNum; ++i) {
	float x, y, size, angle, response;
	int octave, class_id;
	ifs.read(reinterpret_cast<char*>(&x), sizeof(float));
	ifs.read(reinterpret_cast<char*>(&y), sizeof(float));
	ifs.read(reinterpret_cast<char*>(&size), sizeof(float));
	ifs.read(reinterpret_cast<char*>(&angle), sizeof(float));
	ifs.read(reinterpret_cast<char*>(&response), sizeof(float));
	ifs.read(reinterpret_cast<char*>(&octave), sizeof(int));
	ifs.read(reinterpret_cast<char*>(&class_id), sizeof(int));
	features[i].keypoint = cv::KeyPoint(x, y, size, angle, response, octave, class_id);
	
	size_t descriptorSize;
	ifs.read(reinterpret_cast<char*>(&descriptorSize), sizeof(size_t));
	features[i].descriptor.resize(descriptorSize);
	
	for (size_t j = 0; j < descriptorSize; ++j) {
	    float value;
	    ifs.read(reinterpret_cast<char*>(&value), sizeof(float));
	    features[i].descriptor[j] = value;
	}
    }
    
    ifs.close();
    return features;
}

void surf::savefile(const char *filename, const std::vector<surf::feature> &features)
{
    std::ofstream ofs(filename, std::ios::binary);
    
    size_t featuresNum = features.size();
    ofs.write((char*)&featuresNum, sizeof(size_t));
    for (size_t i = 0; i < features.size(); ++i) {
	ofs.write((char*)&features[i].keypoint.pt.x, sizeof(float));
	ofs.write((char*)&features[i].keypoint.pt.y, sizeof(float));
	ofs.write((char*)&features[i].keypoint.size, sizeof(float));
	ofs.write((char*)&features[i].keypoint.angle, sizeof(float));
	ofs.write((char*)&features[i].keypoint.response, sizeof(float));
	ofs.write((char*)&features[i].keypoint.octave, sizeof(int));
	ofs.write((char*)&features[i].keypoint.class_id, sizeof(int));
	size_t descriptorSize = features[i].descriptor.size();
	ofs.write((char*)&descriptorSize, sizeof(size_t));
	for (size_t j = 0; j < features[i].descriptor.size(); ++j) {
	    ofs.write((char*)&features[i].descriptor[j], sizeof(float));
	}
    }
    
    ofs.close();
}

#ifdef __MAIN__

#include <opencv2/highgui/highgui.hpp>
#include <iostream>

int main(int argc, char *argv[])
{
    if (argc != 3) {
	std::cerr << "Usage: " << argv[0] << " <image> <keypoint>" << std::endl;
	return -1;
    }
    
    cv::Mat image = cv::imread(argv[1], 0);
    if (image.empty()) {
	std::cerr << "Error: Couldn't open the image '" << argv[1] << "'" << std::endl;
	return -1;
    }
    
    cv::SURF param = cv::SURF(500, 4, 2, true);
    
    std::vector<surf::feature> features = surf::extract(image, param);
    
    surf::savefile(argv[2], features);
    
    return 0;
}

#endif
