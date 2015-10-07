//
//  CutoutImagePacking.h
//  opencv_test
//
//  Created by vk on 15/8/25.
//  Copyright (c) 2015年 leisheng526. All rights reserved.
//

#ifndef __opencv_test__CutoutImagePacking__
#define __opencv_test__CutoutImagePacking__

#include <stdio.h>
#include <opencv2/opencv.hpp>
#include "CutoutImage.h"

class CutoutImagePacking{
public:
    CutoutImagePacking();
    ~CutoutImagePacking();
    void setColorImage(cv::Mat srcImg, int maxSeedMatNum);                                                 //设置要计算的彩色图
    void setMaskColor(cv::Scalar maskColor);                                            //设置输出观测mask的颜色
    
    void drawMask( std::vector<cv::Point> selectPoint, int lineWidth, cv::Mat & drawResult );          //直接添加mask不带任何分割算法
    void creatMask( std::vector<cv::Point> selectPoint, int lineWidth, cv::Mat & drawResult );         //设置需要区域分割的点，输出计算后的融合结果
    void deleteMask( std::vector<cv::Point> selectPoint, int lineWidth, cv::Mat & drawResult );        //在mask中删除添加选择点，输出计算后的融合结果
    void redo( cv::Mat & dstMat );
    void undo( cv::Mat & dtsMat );
    void resetMask( cv::Mat &dstMat );                                                                   //回复原始状态
    
    cv::Mat getCurrentBitMask();                                                        //输出当前的结果的mask 输出的是8uc1的灰度图，255 有 mask 0 无mask
    cv::Mat getCurrenrColorMergeImg();                                                  //输出当前的融合结果
    cv::Mat getFinalBitCutMask();
    cv::Mat getFinalColorMergeImg();
    //Debug接口
    cv::Mat getDebugMat();
    cv::Mat getDebugMat2();
    
private:
    CutoutImage *cutoutImage;
    cv::Mat srcColorImg;
    cv::Mat srcGrayImg;
    std::vector<cv::Mat> seedMatVector;
    cv::Mat seedStoreMat;
    int selectSeedMat;
    int maxSelectSeedMat;
    cv::Scalar maskColor;
    cv::Mat getM1;
    cv::Mat getM2;
    
};


#endif /* defined(__opencv_test__CutoutImagePacking__) */
