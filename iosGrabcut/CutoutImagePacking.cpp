//
//  CutoutImagePacking.cpp
//  opencv_test
//
//  Created by vk on 15/8/25.
//  Copyright (c) 2015年 leisheng526. All rights reserved.
//

#include "CutoutImagePacking.h"
/**
 *  类构造函数
 */
CutoutImagePacking::CutoutImagePacking()
{
    CutoutImagePacking::cutoutImage = new CutoutImage;
    CutoutImagePacking::seedMatVector.clear();
    CutoutImagePacking::selectSeedMat = 0;
    CutoutImagePacking::maskColor = cv::Scalar(0,0,0);
}

CutoutImagePacking::~CutoutImagePacking()
{
    free(CutoutImagePacking::cutoutImage);
}
/*
 设置要计算的彩色图,这里要重置seedMat,因为一个seedMat对应一个srcImg
 */
void CutoutImagePacking::setColorImage( cv::Mat srcMat, int maxSeedMatNum )
{
    CutoutImagePacking::selectSeedMat = 0;
    srcColorImg = srcMat;
    cv::cvtColor(srcColorImg, srcGrayImg, CV_BGR2BGRA);
    CutoutImagePacking::seedMatVector.clear();
    int cols = CutoutImagePacking::srcColorImg.cols;
    int rows = CutoutImagePacking::srcColorImg.rows;
    seedStoreMat = cv::Mat(rows, cols, CV_8UC1, cv::Scalar(0));
    cv::Mat zero = seedStoreMat.clone();
    CutoutImagePacking::seedMatVector.push_back(zero);
    maxSelectSeedMat = maxSeedMatNum;
    cutoutImage->setColorImg(srcMat);
}

void CutoutImagePacking::setMaskColor(cv::Scalar inputMaskColor)
{
    CutoutImagePacking::maskColor = inputMaskColor;
    
}//设置输出观测mask的颜色

void CutoutImagePacking::drawMask( std::vector<cv::Point> selectPoint, int lineWidth, cv::Mat & drawResult )
{
    cv::Mat sendSeedStoreMat = CutoutImagePacking::seedMatVector[selectSeedMat].clone();  //这个一定要注意否则就把当前拿出的mat修改了
    //    cv::imshow("sendSeedStoreMatA", sendSeedStoreMat);
    cutoutImage->processImageAddMask( selectPoint, sendSeedStoreMat, seedStoreMat, lineWidth, srcColorImg);
    drawResult = cutoutImage->getMergeResult();
    cv::Mat matWillSave = seedStoreMat.clone();     //clone 一下后续存入 vector
    
    if((int)seedMatVector.size() != 0)
    {
        for(;;){
            if( selectSeedMat + 1 == (int)seedMatVector.size() ){
                break;
            }
            else{
                seedMatVector.pop_back();
            }
        }
    }
    seedMatVector.push_back(matWillSave);
    int vCnt = (int)seedMatVector.size();
    if( vCnt ==  maxSelectSeedMat + 1) //保证只有最大设置参数个
    {
        seedMatVector.erase(seedMatVector.begin()); //删除最开始的mat
    }
    selectSeedMat = (int)seedMatVector.size() - 1;
}          //直接添加mask不带任何分割算法

void CutoutImagePacking::creatMask( std::vector<cv::Point> selectPoint, int lineWidth, cv::Mat & drawResult )
{
    if((int)seedMatVector.size() != 0){ //为0是最开始
        cv::Mat sendSeedStoreMat = seedMatVector[selectSeedMat].clone();  //这个一定要注意否则就把当前拿出的mat修改了
        //       cv::imshow("sendSeedStoreMatC", sendSeedStoreMat);
        //cutoutImage->processImageCreatMask( selectPoint, srcColorImg, sendSeedStoreMat, lineWidth,10 );
        double t = (double)cvGetTickCount();
        cutoutImage->processImageCreatMaskByGrabcut(selectPoint, srcColorImg, sendSeedStoreMat, lineWidth);
        t = (double)cvGetTickCount() - t;
        printf( "run time = %gms\n", t/(cvGetTickFrequency()*1000) );
        
        drawResult = cutoutImage->getMergeResult();
        seedStoreMat = sendSeedStoreMat;
    }
    else{
        cutoutImage->processImageCreatMask( selectPoint, srcColorImg, seedStoreMat, lineWidth, 10 );
        drawResult = cutoutImage->getMergeResult();
    }
    //这里要存储
    cv::Mat matWillSave = seedStoreMat.clone();
    //首先要删除selectSeedMat以后的内容，因为可能返回或者前进
    if((int)seedMatVector.size() != 0)
    {
        for(;;){
            if( selectSeedMat + 1 == (int)seedMatVector.size() ){
                break;
            }
            else{
                seedMatVector.pop_back();
            }
        }
    }
    seedMatVector.push_back(matWillSave);
    int vCnt = (int)seedMatVector.size();
    if( vCnt ==  maxSelectSeedMat + 1) //保证只有最大设置参数个
    {
        seedMatVector.erase(seedMatVector.begin()); //删除最开始的mat
    }
    selectSeedMat = (int)seedMatVector.size() - 1; //只要有修改就将selectSeedMat放到修改位置
    
    std::cout<<"CV_EVENT_LBUTTONUP" <<std::endl;
    std::cout<<" selectSeedMat =  " << selectSeedMat << std::endl;
}//设置需要区域分割的点，输出计算后的融合结果

void CutoutImagePacking::deleteMask( std::vector<cv::Point> selectPoint, int lineWidth, cv::Mat & drawResult )
{
    if((int)seedMatVector.size() != 0){
        seedStoreMat = seedMatVector[selectSeedMat].clone();
    }
    else{
        seedStoreMat = cv::Mat( srcColorImg.rows, srcColorImg.cols,CV_8UC1,cv::Scalar(0) );
    }
    
    cv::Mat emptyMat;
    cutoutImage->processImageDeleteMask( selectPoint, seedStoreMat, srcColorImg, emptyMat, lineWidth );
    drawResult = cutoutImage->getMergeResult();
    if((int)seedMatVector.size() != 0)  //若已经生成过计算结果
    {
        cv::Mat matWillBeStore = seedStoreMat.clone();
        seedMatVector.push_back(matWillBeStore);
        int vCnt = (int)seedMatVector.size();
        if( vCnt == maxSelectSeedMat + 1 ) //保证只有设置的最大数量
        {
            seedMatVector.erase(seedMatVector.begin());
        }
        selectSeedMat = (int)seedMatVector.size() - 1;
    }
    //        cv::imshow("orgGray", allShow);
    //        cv::imshow("deleteMat", deleteMat);
    std::cout<<"CV_EVENT_RBUTTONUP" <<std::endl;
    std::cout<<" selectSeedMat =  " << selectSeedMat << std::endl;
}//在mask中删除添加选择点，输出计算后的融合结果

void CutoutImagePacking::redo( cv::Mat & dstMat)
{
    if((int)seedMatVector.size() != 0 && selectSeedMat != 0)  //
    {
        selectSeedMat --;
        cutoutImage->colorDispResultWithFullSeedMat(srcColorImg, seedMatVector[selectSeedMat]);
        dstMat = cutoutImage->getMergeResult();
    }
    else
    {
        cutoutImage->colorDispResultWithFullSeedMat(srcColorImg, seedMatVector[0]);
        dstMat = cutoutImage->getMergeResult();
    }
}

void CutoutImagePacking::undo( cv::Mat & dstMat)
{
    if( selectSeedMat !=  maxSelectSeedMat - 1 && selectSeedMat != seedMatVector.size() - 1 ){
        selectSeedMat ++;
        cutoutImage->colorDispResultWithFullSeedMat(srcColorImg, seedMatVector[selectSeedMat]);
        dstMat = cutoutImage->getMergeResult();
    }
    else
    {
        cutoutImage->colorDispResultWithFullSeedMat(srcColorImg, seedMatVector[selectSeedMat]);
        dstMat = cutoutImage->getMergeResult();
    }
}

void CutoutImagePacking::resetMask( cv::Mat & dstMat )
{
    selectSeedMat = 0;
    //initSeedMatVector(aSize);
    seedMatVector.clear();
    cv::Mat initZeroMat = cv::Mat( srcColorImg.rows, srcColorImg.cols, CV_8UC1, cv::Scalar(0) );
    seedMatVector.push_back(initZeroMat);
    
    seedStoreMat = initZeroMat.clone(); //cv::Mat( img.rows, img.cols, CV_8UC1, cv::Scalar(0) );
    cutoutImage->colorDispResultWithFullSeedMat( srcColorImg, seedMatVector[selectSeedMat]);
    dstMat = cutoutImage->getMergeResult();
}

cv::Mat CutoutImagePacking::getCurrentBitMask()
{
    cv::Mat backMat = seedMatVector[selectSeedMat];
    return backMat;
}//输出当前的结果的mask 输出的是8uc1的灰度图，255 有 mask 0 无mask

cv::Mat CutoutImagePacking::getCurrenrColorMergeImg()
{
    cv::Mat backMat;
    return backMat;
}//输出当前的融合结果

cv::Mat CutoutImagePacking::getFinalBitCutMask()
{
    cv::Mat backMat;
    return backMat;
}

cv::Mat CutoutImagePacking::getFinalColorMergeImg()
{
    cv::Mat dstMat;
    cv::Mat blobMat = seedMatVector[selectSeedMat];
    cv::Mat edgeBlurResult;
    cutoutImage->edgeBlur( srcColorImg, blobMat, 1, edgeBlurResult);  //dstMat就是扣取结果，还要对结果进行椭圆拟合和旋转
    //cutoutImage->rotateMat(cutoutImage->classCutMat, dstMat, edgeBlurResult);
    //dstMat = cutoutImage->scaleFCMI2InputColorImageSize(dstMat);
    dstMat = cutoutImage->scaleFCMI2InputColorImageSize(edgeBlurResult);
    return dstMat;
}

cv::Mat CutoutImagePacking::getDebugMat()
{
    cutoutImage->getTestMat(getM1,getM2);
    return getM1.clone();
}

cv::Mat CutoutImagePacking::getDebugMat2()
{
    return getM2.clone();
}
