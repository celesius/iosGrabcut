//
//  CloverGrabCut.h
//  opencv_test
//
//  Created by vk on 15/9/6.
//  Copyright (c) 2015年 clover. All rights reserved.
//

#ifndef __opencv_test__CloverGrabCut__
#define __opencv_test__CloverGrabCut__

#include <stdio.h>
#include <opencv2/opencv.hpp>
class CloverGrabCut
{
public:
    CloverGrabCut();
    ~CloverGrabCut();
    void    setImage(cv::Mat imgMat);
    void    processGrabCut(std::vector<cv::Point> maskPoint, int lineWidth, cv::Mat &resultMaskMat, cv::Mat &resultColorMat);

private:
    cv::Mat getFGByMask(cv::Mat mask);
    cv::Rect    getMaskRct( cv::Mat maskMat );
    cv::Mat filterMaskAndMergeMat(cv::Mat srcMat, cv::Mat matStore, cv::Mat filterRuleMat);
    bool    diffRect(cv::Rect r1, cv::Rect r2);
    //void    showBit( cv::Mat gray, cv::Mat & out );
    cv::Mat mergeMat( cv::Mat mergeMat, cv::Mat mergeToMat, cv::Rect mergeRect );
    cv::Mat pointArray2MatWithLineWidth( std::vector<cv::Point> selectPoint, int lineWidth, cv::Size matSize );
    void    grabcutByMergeToMatAndMskMat(const cv::Mat mergeToMat ,const cv::Mat msk, cv::Mat & resultMaskMat, cv::Mat &resultColorMat);
    cv::Mat getBinMaskByMask(cv::Mat mask);

    void scaleImg( const cv::Mat srcMat, float scaleLevel, cv::Mat & dstMat);
    
    void deleteBlackIsland(const cv::Mat srcBitMat ,cv::Mat &dstBitMat);
    void filterImageForEdgeBlur(const cv::Mat imFrame,cv::Mat & outFrame);
    void filterImage(const cv::Mat imFrame,cv::Mat & outFrame);
    void checkGcut(cv::Mat & srcMat);
private:
    cv::Mat _srcImgMat;
    cv::Mat _maskStoreMat;
    
};
#endif /* defined(__opencv_test__CloverGrabCut__) */
