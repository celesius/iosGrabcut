//
//  CutoutImage.cpp
//  opencv_test
//
//  Created by vk on 15/8/11.
//  Copyright (c) 2015年 Clover. All rights reserved.
//

#include "CutoutImage.h"

CutoutImage::CutoutImage()
{
    CutoutImage::classCutMat = *new cv::Mat;
    _cloverGrabCut = new CloverGrabCut;
}

CutoutImage::~CutoutImage()
{

}
void CutoutImage::setColorImg(cv::Mat colorImg)
{
    _cloverGrabCut->setImage(colorImg);
    inputColorImageSize = colorImg.size();
}

void CutoutImage::processImageCreatMaskByGrabcut(std::vector<cv::Point> mouseSlideRegionDiscrete , const cv::Mat srcMat, cv::Mat &dstMat , int lineWidth)
{
    mouseSlideRegion.clear();
    cv::Size matSize = cv::Size(srcMat.cols, srcMat.rows);
    cv::Mat resultColorMat;
    cv::Mat showMergeColorImg = srcMat.clone();
    printf("lineWidth = %d\n",lineWidth);
    _cloverGrabCut->processGrabCut(mouseSlideRegionDiscrete, lineWidth, dstMat, resultColorMat);
    CutoutImage::colorDispResultWithFullSeedMat(showMergeColorImg,dstMat);
    //cv::imshow("dstMat", dstMat);
    //CutoutImage::drawLineAndMakePointSet(mouseSlideRegionDiscrete,matSize,lineWidth,mouseSlideRegion);

}
void CutoutImage::processImageCreatMask( std::vector<cv::Point> mouseSlideRegionDiscrete , const cv::Mat srcMat, cv::Mat &dstMat, int lineWidth, int expandWidth )
{
    cv::Mat showMat;// = srcMat.clone();
    cv::cvtColor(srcMat, showMat, CV_BGR2GRAY);
    cv::Mat showMergeColorImg = srcMat.clone();
    cv::Mat seedStoreMat = dstMat;   //seedStoreMat 在外部存储了本次操作所生成的全部种子点。
    mouseSlideRegion.clear();
    
    cv::Size matSize = *new cv::Size;
    matSize.width = showMat.cols;
    matSize.height = showMat.rows;
    CutoutImage::drawLineAndMakePointSet(mouseSlideRegionDiscrete,matSize,lineWidth,mouseSlideRegion);
    int lx = showMat.cols,rx = 0,ty = showMat.rows,by = 0;
    //求画线范围
    for(int i = 0;i<(int)mouseSlideRegion.size();i++)
    {
        //std::cout<<"point  = "<< mouseSlideRegion[i]<<std::endl;
        //cv::circle(showMatClone, mouseSlideRegion[i], 0.5, cv::Scalar(255)); //绘制现实点
        //最左面点x，最右面点x，最上面点y,最下面点y
        if(mouseSlideRegion[i].x < lx)
        {
            lx = mouseSlideRegion[i].x;
        }
        if(mouseSlideRegion[i].x > rx)
        {
            rx = mouseSlideRegion[i].x;
        }
        if(mouseSlideRegion[i].y <ty )
        {
            ty = mouseSlideRegion[i].y;
        }
        if(mouseSlideRegion[i].y > by){
            by = mouseSlideRegion[i].y;
        }
        //CvPoint forePtsCvPoint =
    }
    std::cout<<" lx " << lx << " rx " << rx << " ty " << ty << " by " << by <<std::endl;
    std::cout<<" orgMat cols " <<showMat.cols<< "orgMat rows " << showMat.rows <<std::endl;
    
    if( lx - expandWidth >= 0 )
        lx = lx - expandWidth;
  
    if( rx + expandWidth <= showMat.cols - 1 )
        rx = rx + expandWidth;
   
    if( ty - expandWidth >= 0)
        ty = ty - expandWidth;
    
    if( by + expandWidth <= showMat.rows - 1 )
        by = by + expandWidth;

    std::cout<<" lx " << lx << " rx " << rx << " ty " << ty << " by " << by <<std::endl;
    
    cv::Point ltP = cv::Point(lx,ty);
    cv::Point rtP = cv::Point(rx,ty);
    cv::Point lbP = cv::Point(lx,by);
    cv::Point rbP = cv::Point(rx,by);
    //要截取的图形
    int rectMatRow = by - ty + 1;
    int rectMatCol = rx - lx + 1;
    cv::Mat recMat = cv::Mat (rectMatRow,rectMatCol,CV_8UC1,cv::Scalar(0));
    //cv::rectangle(showMatClone, ltP, rbP, cv::Scalar(255),1); //画图形
    cv::Mat mouseSlideSeedStoreMat = cv::Mat(rectMatRow,rectMatCol,CV_8UC1,cv::Scalar(0));
    for(int y = 0;y<rectMatRow;y++){
        uchar *rectMatLineData = recMat.ptr<uchar>(y);
        uchar *orgMatLineData = showMat.ptr<uchar>(ty+y);
        uchar *msssMatLineData = mouseSlideSeedStoreMat.ptr<uchar>(y);
        uchar *ssMatLineData = seedStoreMat.ptr<uchar>(ty+y);
        for(int x = 0; x < rectMatCol; x++){
            rectMatLineData[x] = orgMatLineData[lx+x];
            msssMatLineData[x] = ssMatLineData[lx + x];
        }
    }
    //cutMat = recMat.clone();
    //cv::imshow("mouseSlideSeedStoreMat", mouseSlideSeedStoreMat);
    
    cv::Mat bitMat;
    int blockSize = 25;
    int constValue = 10;
    cv::adaptiveThreshold(recMat, bitMat, 255, CV_ADAPTIVE_THRESH_MEAN_C, CV_THRESH_BINARY_INV, blockSize, constValue);
    
    cv::Mat filterImg;
    CutoutImage::filterImage(recMat,filterImg);
    cv::Mat nextImg = filterImg.clone();
    cv::Mat regionGrowMat;
    CutoutImage::rectRegionGrow( mouseSlideRegion, ltP, filterImg, mouseSlideSeedStoreMat ,regionGrowMat);
   // cv::imshow("regionGrowMat", regionGrowMat);
    cv::Mat mergeMat;
    CutoutImage::mergeProcess(regionGrowMat,mergeMat);
   // cv::imshow("mergeMat", mergeMat);
    CutoutImage::storeSeed(seedStoreMat,mergeMat,ltP); // seedStoreMat 需要扣取的mask
   // cv::imshow("seedStoreMat", seedStoreMat);
    //cv::imshow("showMat", showMatClone);
    cv::Mat colorMergeMat;
    CutoutImage::colorDispResultWithFullSeedMat(showMergeColorImg,seedStoreMat);
}



void CutoutImage::processImageAddMask(std::vector<cv::Point> mouseSlideRegionDiscrete , const cv::Mat srcMat, cv::Mat &dstMat , int lineWidth, const cv::Mat colorMatForShow )
{
    CutoutImage::point2LineMat( srcMat, mouseSlideRegionDiscrete, lineWidth, dstMat );
    CutoutImage::colorDispResultWithFullSeedMat( colorMatForShow , dstMat);
    
}

void CutoutImage::processImageDeleteMask(std::vector<cv::Point> mouseSlideRegionDiscrete , cv::Mat &seedStoreMat ,  const cv::Mat srcMat, cv::Mat &dstMat , int lineWidth )
{
    cv::Mat deleteMat;
    cv::Mat showMergeColorImg = srcMat.clone();
    
    cv::Size matSize = *new cv::Size;
    matSize.width = showMergeColorImg.cols;
    matSize.height = showMergeColorImg.rows;
    
    CutoutImage::deleteMatCreat(mouseSlideRegionDiscrete,matSize,lineWidth,deleteMat);
    CutoutImage::deleteMask(deleteMat,seedStoreMat);
    CutoutImage::colorDispResultWithFullSeedMat(showMergeColorImg,seedStoreMat);
}

void CutoutImage::point2LineMat( const cv::Mat drawMat, std::vector<cv::Point> inPoint, int lineWide, cv::Mat &dstMat )
{
    cv::Mat drawLineMat = drawMat.clone();
//    int rows = drawMat.rows;
//    int cols = drawMat.cols;
    int vectorSize = (int)inPoint.size();
    for(int loop = 0;loop < vectorSize-1;loop ++)
    {
        cv::Point p1 = inPoint[loop];
        cv::Point p2 = inPoint[loop + 1];
        cv::line(drawLineMat, p1, p2, cv::Scalar(255),lineWide);
    }
    if(vectorSize == 1) //单击一下
    {
        cv::line(drawLineMat, inPoint[0], inPoint[0], cv::Scalar(255),lineWide);
    }
    dstMat = drawLineMat;
//    cv::imshow( "drawLineMat", drawLineMat );
}

bool CutoutImage::regionGrowClover( const cv::Mat srcMat ,cv::Mat &dstMat, int initSeedx, int initSeedy, int threshold) //4邻域生长灰度图生长,不传入已有种子点
{
    bool rState = false;
    std::vector<cv::Point> seedVector;
    cv::Point currentPoint = cv::Point(0,0);
    //dstMat = cv::Mat(srcMat.rows,srcMat.cols,CV_8UC1,cv::Scalar(0)); //灰度图，所以只有单通道8bit
    
    seedVector.push_back(cv::Point(initSeedx,initSeedy)); //种子阵列初始化
    dstMat.ptr<uchar>(initSeedy)[initSeedx] = 255; //设置当前点为种子点
    
    uchar currentPixelValue = srcMat.ptr<uchar>(initSeedy)[initSeedx]; //当前点
    
    while (!seedVector.empty()) {
        //cv::vector<cv::Point>::iterator iter;
        //iter = seedVector.begin();
        currentPoint = seedVector[0]; //取出第一个并进行计算
        int x = currentPoint.x;
        int y = currentPoint.y;
        seedVector.erase(seedVector.begin());
        //std::cout<<"seedVector.size() = "<<seedVector.size()<<std::endl;
        //取得原始图像素值与原始图四邻域像素值
        //uchar currentPixelValue = srcMat.ptr<uchar>(y)[x]; //当前点
        //判断左面
        if(x != 0) //不是最左边点,左边点有效
        {
            if(dstMat.ptr<uchar>(y)[x-1] == 0)  //取出左边点的seed标示,且这个点是0，这个点没有当过种子点
            {
                uchar currentLeftPValue = srcMat.ptr<uchar>(y)[x - 1];
                //cout<< "currentPixelValue = " << (int)currentPixelValue <<endl;
                //cout<< "currentLeftPValue = " << (int)currentLeftPValue <<endl;
                if(abs(currentPixelValue - currentLeftPValue) <= (uchar)threshold){
                    dstMat.ptr<uchar>(y)[x-1] = 255;
                    seedVector.push_back(cv::Point(x-1,y)); //符合计算条件，将左面点压人计算队列
                }
                //uchar currentDstLeftValue = dstMat.ptr<uchar>(y)[x - 1];
            }
        }
        
        if(y != 0) //不是最上面点，上面点有效
        {
            if(dstMat.ptr<uchar>(y-1)[x] == 0)
            {
                uchar currentUpPValue = srcMat.ptr<uchar>(y - 1)[x];
                if(abs(currentPixelValue - currentUpPValue) <= (uchar)threshold){
                    dstMat.ptr<uchar>(y-1)[x] = 255;
                    seedVector.push_back(cv::Point(x,y-1));
                }
            }
        }
        
        if(x != (srcMat.cols-1)) //不是最右面点，右面点有效
        {
            if(dstMat.ptr<uchar>(y)[x + 1] == 0)
            {
                uchar currentRightPValue = srcMat.ptr<uchar>(y)[x + 1];
                if(abs(currentPixelValue - currentRightPValue) <= (uchar)threshold){
                    dstMat.ptr<uchar>(y)[x + 1] = 255;
                    seedVector.push_back(cv::Point(x+1,y));
                }
            }
        }
        
        if( y != (srcMat.rows - 1)) //不是最下面的点
        {
            if(dstMat.ptr<uchar>(y+1)[x] == 0)
            {
                uchar currentDownPValue = srcMat.ptr<uchar>(y + 1)[x];
                if(abs(currentPixelValue - currentDownPValue) <= (uchar)threshold){
                    dstMat.ptr<uchar>(y+1)[x] = 255;
                    seedVector.push_back(cv::Point(x,y+1));
                }
            }
        }
    }
    return rState;
}

bool CutoutImage::regionGrowClover( const cv::Mat srcMat, const cv::Mat seedStoreMat ,cv::Mat &dstMat, int initSeedx, int initSeedy, int threshold) //4邻域生长灰度图生长,传入已有种子点
{
//    cv::imshow("srcMatregionGrowClover", srcMat);
    bool rState = false;
    std::vector<cv::Point> seedVector;
    cv::Point currentPoint = cv::Point(0,0);
    //dstMat = cv::Mat(srcMat.rows,srcMat.cols,CV_8UC1,cv::Scalar(0)); //灰度图，所以只有单通道8bit
    seedVector.push_back(cv::Point(initSeedx,initSeedy)); //种子阵列初始化
    //dstMat.ptr<uchar>(initSeedy)[initSeedx] = 255; //设置当前点为种子点
    dstMat = seedStoreMat;
    uchar currentPixelValue = srcMat.ptr<uchar>(initSeedy)[initSeedx]; //当前点
    while (!seedVector.empty()) {
        //cv::vector<cv::Point>::iterator iter;
        //iter = seedVector.begin();
        currentPoint = seedVector[0]; //取出第一个并进行计算
        int x = currentPoint.x;
        int y = currentPoint.y;
        seedVector.erase(seedVector.begin());
        uchar seedStoreMatPointData;
        //std::cout<<"seedVector.size() = "<<seedVector.size()<<std::endl;
        //取得原始图像素值与原始图四邻域像素值
        //uchar currentPixelValue = srcMat.ptr<uchar>(y)[x]; //当前点
        //判断左面
        if(x != 0) //不是最左边点,左边点有效
        {
            //seedStoreMatPointData = seedStoreMat.ptr<uchar>(y)[x-1];
            //if(dstMat.ptr<uchar>(y)[x-1] == 0 && seedStoreMatPointData == 0)  //取出左边点的seed标示,且这个点是0，这个点没有当过种子点
            if(dstMat.ptr<uchar>(y)[x-1] == 0)  //取出左边点的seed标示,且这个点是0，这个点没有当过种子点
            {
                uchar currentLeftPValue = srcMat.ptr<uchar>(y)[x - 1];
                //cout<< "currentPixelValue = " << (int)currentPixelValue <<endl;
                //cout<< "currentLeftPValue = " << (int)currentLeftPValue <<endl;
                if(abs(currentPixelValue - currentLeftPValue) <= (uchar)threshold){
                    dstMat.ptr<uchar>(y)[x-1] = 255;
                    seedVector.push_back(cv::Point(x-1,y)); //符合计算条件，将左面点压人计算队列
                }
                //uchar currentDstLeftValue = dstMat.ptr<uchar>(y)[x - 1];
            }
        }
        
        if(y != 0) //不是最上面点，上面点有效
        {
            //seedStoreMatPointData = seedStoreMat.ptr<uchar>(y-1)[x];
            //if(dstMat.ptr<uchar>(y-1)[x] == 0 && seedStoreMatPointData == 0)
            if(dstMat.ptr<uchar>(y-1)[x] == 0)
            {
                uchar currentUpPValue = srcMat.ptr<uchar>(y - 1)[x];
                if(abs(currentPixelValue - currentUpPValue) <= (uchar)threshold){
                    dstMat.ptr<uchar>(y-1)[x] = 255;
                    seedVector.push_back(cv::Point(x,y-1));
                }
            }
        }
        
        if(x != (srcMat.cols-1)) //不是最右面点，右面点有效
        {
            //seedStoreMatPointData = seedStoreMat.ptr<uchar>(y)[x+1];
            //if(dstMat.ptr<uchar>(y)[x + 1] == 0 && seedStoreMatPointData == 0)
            if(dstMat.ptr<uchar>(y)[x + 1] == 0)
            {
                uchar currentRightPValue = srcMat.ptr<uchar>(y)[x + 1];
                if(abs(currentPixelValue - currentRightPValue) <= (uchar)threshold){
                    dstMat.ptr<uchar>(y)[x + 1] = 255;
                    seedVector.push_back(cv::Point(x+1,y));
                }
            }
        }
        
        if( y != (srcMat.rows - 1)) //不是最下面的点
        {
            //seedStoreMatPointData = seedStoreMat.ptr<uchar>(y+1)[x];
            //if(dstMat.ptr<uchar>(y+1)[x] == 0 && seedStoreMatPointData == 0)
            if(dstMat.ptr<uchar>(y+1)[x] == 0)
            {
                uchar currentDownPValue = srcMat.ptr<uchar>(y + 1)[x];
                if(abs(currentPixelValue - currentDownPValue) <= (uchar)threshold){
                    dstMat.ptr<uchar>(y+1)[x] = 255;
                    seedVector.push_back(cv::Point(x,y+1));
                }
                
            }
        }
        
    }
    
    return rState;
}

void CutoutImage::line2PointSet(const cv::Mat lineMat,std::vector<cv::Point> &pointSet)
{
    cv::Mat aMat = lineMat.clone();
    int rows = aMat.rows;
    int cols = aMat.cols;
    
    pointSet.clear();
    
    for(int y = 0;y<rows;y++){
        uchar *matRowData = aMat.ptr<uchar>(y);
        for(int x = 0;x<cols;x++){
            if(matRowData[x] != 0)
            {
                pointSet.push_back(cv::Point(x,y));
            }
        }
    }
}

void CutoutImage::drawLineAndMakePointSet(std::vector<cv::Point> inPoint,cv::Size matSize,int lineWide,std::vector<cv::Point> &pointSet)    //输入有序点，然后绘制线再生成点集
{
    int rows = matSize.height;
    int cols = matSize.width;
    int vectorSize = (int)inPoint.size();
    cv::Mat drawLineMat = cv::Mat(rows,cols,CV_8UC1,cv::Scalar(0));
    for(int loop = 0;loop < vectorSize-1;loop ++)
    {
        cv::Point p1 = inPoint[loop];
        cv::Point p2 = inPoint[loop + 1];
        cv::line(drawLineMat, p1, p2, cv::Scalar(255),lineWide);
    }
    if(vectorSize == 1) //单击一下
    {
        cv::line(drawLineMat, inPoint[0], inPoint[0], cv::Scalar(255),lineWide);
    }
    CutoutImage::line2PointSet(drawLineMat,pointSet);
   // cv::imshow("drawLineMat", drawLineMat);
    //colorDraw(drawLineMat,cv::Scalar(255,0,0));
}

void CutoutImage::deleteBlackIsland(const cv::Mat srcBitMat ,cv::Mat &dstBitMat)
{
    cv::Mat a_mat = srcBitMat.clone();
    std::vector<std::vector<cv::Point>> contours;
    dstBitMat = cv::Mat(a_mat.rows, a_mat.cols, CV_8UC1,cv::Scalar(0));
    cv::findContours(a_mat, contours, CV_RETR_EXTERNAL, CV_CHAIN_APPROX_NONE);
    cv::drawContours(dstBitMat, contours, -1, cv::Scalar(255),CV_FILLED);
}

void CutoutImage::mergeProcess(const cv::Mat srcImg,cv::Mat &dstImg)
{
    cv::Mat a_mat = srcImg.clone();
    cv::Mat showContours = cv::Mat(a_mat.rows,a_mat.cols,CV_8UC1,cv::Scalar(0));
    //cv::Mat showContours = cv::Mat(srcImg.rows,srcImg.cols,CV_8UC4,cv::Scalar(0,0,0,0));
    std::vector<std::vector<cv::Point>> contours;
    std::vector<std::vector<cv::Point>> hiararchy;
    cv::findContours(a_mat, contours, CV_RETR_EXTERNAL , CV_CHAIN_APPROX_NONE);
    cv::drawContours(showContours, contours, -1, cv::Scalar(255),CV_FILLED);
    //cv::imshow("contours", showContours);
    cutMat1 = showContours.clone();
    cv::Mat closeMat;
//  cv::morphologyEx(showContours, closeMat, cv::MORPH_CLOSE, cv::Mat(11,11,CV_8U),cv::Point(-1,-1),1);
    cv::morphologyEx(showContours, closeMat, cv::MORPH_CLOSE, cv::Mat(11,11,CV_8U,cv::Scalar(1)));
    cutMat2 = closeMat.clone();
    // cv::imshow("closeMat", closeMat);
    //cv::medianBlur(closeMat, closeMat,5);
    //因为膨胀和腐蚀带来了一些"孤岛"(断裂的色块)，所以要再做一次背景块去除
    cv::Mat dbCloseMat;
    CutoutImage::deleteBlackIsland(closeMat,dbCloseMat);
    cv::medianBlur(dbCloseMat, dbCloseMat,5); //后作中值滤波
    cv::dilate(dbCloseMat, dbCloseMat, cv::Mat());
    //    cv::imshow("dbCloseMat", dbCloseMat);
    //    cv::imshow("medianBlurCloseMat", closeMat);
    dstImg = dbCloseMat;
}

void CutoutImage::filterImage(const cv::Mat imFrame,cv::Mat & outFrame)
{
    /* Soften image */
    cv::Mat tmpMat;
    cv::GaussianBlur(imFrame, tmpMat, cv::Size(7,7), 0,0);
    //myImageShow("Gaussian", tmpMat, cv::Size(640,480));
    //cvSmooth(ctx->image, ctx->temp_image3, CV_GAUSSIAN, 11, 11, 0, 0);
    /* Remove some impulsive noise */
    cv::medianBlur(tmpMat, tmpMat,3);
    //    cvSmooth(ctx->temp_image3, ctx->temp_image3, CV_MEDIAN, 11, 11, 0, 0);
    //cv::cvtColor(tmpMat, tmpMat, CV_BGR2HSV);
    //    cvCvtColor(ctx->temp_image3, ctx->temp_image3, CV_BGR2HSV);
    //
    //    /*
    //     * Apply threshold on HSV values to detect skin color
    //     */
    
    //cv::inRange(tmpMat, cv::Scalar(0,55,90,255), cv::Scalar(28,175,230,255), outFrame);
    
    //    cvInRangeS(ctx->temp_image3,
    //               cvScalar(0, 55, 90, 255),
    //               cvScalar(28, 175, 230, 255),
    //               ctx->thr_image);
    //
    //    /* Apply morphological opening */
    //    cvMorphologyEx(ctx->thr_image, ctx->thr_image, NULL, ctx->kernel,
    //                   CV_MOP_OPEN, 1);
    //    cvSmooth(ctx->thr_image, ctx->thr_image, CV_GAUSSIAN, 3, 3, 0, 0);
    
    //  IplConvKernel *kernel = cvCreateStructuringElementEx(9, 9, 4, 4, CV_SHAPE_RECT,NULL);
    cv::Mat kernelMat = getStructuringElement(CV_SHAPE_RECT, cv::Size(3,3),cv::Point(2,2));
    cv::morphologyEx(tmpMat, outFrame, CV_MOP_OPEN,kernelMat);
    //cv::GaussianBlur(outFrame, outFrame, cv::Size(3,3), 0,0);
}

void CutoutImage::filterImageForEdgeBlur(const cv::Mat imFrame,cv::Mat & outFrame)
{
    /* Soften image */
    cv::Mat tmpMat;
    cv::GaussianBlur(imFrame, tmpMat, cv::Size(21,21), 11.0);
    /* Remove some impulsive noise */
    cv::medianBlur(tmpMat, tmpMat,3);
    cv::Mat kernelMat = getStructuringElement(CV_SHAPE_RECT, cv::Size(3,3),cv::Point(2,2));
    cv::morphologyEx(tmpMat, outFrame, CV_MOP_OPEN,kernelMat);
    //cv::GaussianBlur(outFrame, outFrame, cv::Size(3,3), 0,0);
}

void CutoutImage::rectRegionGrow( std::vector<cv::Point> seedVector, cv::Point rectMatOrg, const cv::Mat srcMat, const cv::Mat seedStoreMat , cv::Mat &dstMat)
{
    int matRow = srcMat.rows;
    int matCol = srcMat.cols;
    
    dstMat = cv::Mat(matRow,matCol,CV_8UC1,cv::Scalar(0));
    
    for(int i = 0;i<seedVector.size();i++){
        int seedx = seedVector[i].x - rectMatOrg.x;
        int seedy = seedVector[i].y - rectMatOrg.y;
        regionGrowClover(srcMat, seedStoreMat, dstMat, seedx, seedy, 5);
    }
}

void CutoutImage::storeSeed(cv::Mat &storeSeedMat,cv::Mat currentSeedMat,cv::Point cseedMatAnchorPoint)
{
    int cSeedMatRows = currentSeedMat.rows;
    int cSeedMatCols = currentSeedMat.cols;
    
    for(int y = 0;y<cSeedMatRows;y++){
        uchar *csMatRowData = currentSeedMat.ptr<uchar>(y);
        uchar *ssMatRowData = storeSeedMat.ptr<uchar>(y + cseedMatAnchorPoint.y);
        for (int x = 0; x<cSeedMatCols; x++) {
            ssMatRowData[x+cseedMatAnchorPoint.x] =   ssMatRowData[x+cseedMatAnchorPoint.x]|csMatRowData[x];
        }
    }
}

void CutoutImage::deleteMatCreat(std::vector<cv::Point> inPoint,cv::Size matSize, int lineWide ,cv::Mat &dstMat)
{
    int rows = matSize.height;
    int cols = matSize.width;
    int vectorSize = (int)inPoint.size();
    cv::Mat drawLineMat = cv::Mat(rows,cols,CV_8UC1,cv::Scalar(0));
    for(int loop = 0;loop < vectorSize-1;loop ++)
    {
        cv::Point p1 = inPoint[loop];
        cv::Point p2 = inPoint[loop + 1];
        cv::line(drawLineMat, p1, p2, cv::Scalar(255),lineWide);
    }
    if(vectorSize == 1) //单击一下
    {
        cv::line(drawLineMat, inPoint[0], inPoint[0], cv::Scalar(255),lineWide);
    }
    dstMat = drawLineMat;
    //cv::imshow("DeleteDrawLineMat", drawLineMat);
}

void CutoutImage::deleteMask(const cv::Mat deleteMat,cv::Mat &seedMat)
{
    cv::Mat deleteMatClone = deleteMat.clone();
    int rows = seedMat.rows;
    int cols = seedMat.cols;
    
    for(int y = 0; y<rows; y++){
        uchar *seedMatRowData = seedMat.ptr<uchar>(y);
        uchar *deleteMatRowData = deleteMatClone.ptr<uchar>(y);
        for(int x = 0; x<cols; x++){
            if(deleteMatRowData[x] != 0){
                seedMatRowData[x] = 0;
            }
        }
    }
}

void CutoutImage::rotateMat (const cv::Mat srcMat ,cv::Mat &dstMat,const cv::Mat colorMat)
{
    std::vector<std::vector<cv::Point>> contours;
    std::vector<cv::Vec4f> lineVector;
    cv::Mat aMat = srcMat.clone();
    cv::findContours(aMat, contours, CV_RETR_EXTERNAL, CV_CHAIN_APPROX_NONE);
    cv::Mat showMat = cv::Mat(aMat.rows,aMat.cols,CV_8UC3,cv::Scalar(0,0,0));
    dstMat = aMat.clone();
    
    std::vector<cv::Rect> nailRect;
    
    for(int i = 0;i<(int)contours.size();i++)
    {
        if(contours[i].size() > 5)
        {
            cv::RotatedRect tmp = cv::minAreaRect(contours[i]);
            //ellipses.push_back(temp);
            cv::drawContours(showMat, contours, i, cv::Scalar(255,0,0), 1, 8);
            cv::ellipse(showMat, tmp, cv::Scalar(0,255,255), 2, 8);
            //cv::line(<#cv::Mat &img#>, <#Point pt1#>, <#Point pt2#>, <#const Scalar &color#>)
            cv::rectangle(showMat, tmp.boundingRect(), cv::Scalar(255,255,0),1,8);
            //imshow("Ellipses", showMat);
            float rotAngle = tmp.angle;
            tmp.angle = 0;
            //cv::circle(showMat,  cv::Point(tmp.boundingRect().x,tmp.boundingRect().y) , 2, cv::Scalar(0,0,255));
            if(tmp.boundingRect().width > tmp.boundingRect().height)
            {
                tmp.angle = 90;
                rotAngle = rotAngle - 90;
            }
            cv::rectangle(showMat, tmp.boundingRect(), cv::Scalar(255,255,0),1,8);
            nailRect.push_back(tmp.boundingRect());
            cv::ellipse(showMat, tmp, cv::Scalar(255,255,255), 2, 8);
            //imshow("Ellipses", showMat);
            //cv::Mat rotMat = cv::Mat(2,3,CV_32FC1);
            cv::Mat rotMat =   cv::getRotationMatrix2D(tmp.center,rotAngle, 1);
            //cv::transform(srcMat, dstMat, rotMat);
            cv::warpAffine(colorMat, dstMat, rotMat, cv::Size(std::max(srcMat.rows,srcMat.cols),std::max(srcMat.rows,srcMat.cols)));
            //cv::imshow("RRRRRR", dstMat);
            
        }
        else
        {
            // cv::drawContours(showMat, contours, i, cv::Scalar(255,255,255), -1, 8);
            // imshow("Ellipses", showMat);
        }
    }
   
    cv::RNG rng; //随机数
    int rows = dstMat.rows;
    int cols = dstMat.cols;
    
    int lx = cols;
    int rx = 0;
    int ty = rows;
    int by = 0;
    
    cv::Mat grayDst;
    cv::cvtColor(dstMat, grayDst, CV_BGRA2GRAY);
    
    for(int y = 0; y<rows; y++ ){
        uchar *grayDstRowsData = grayDst.ptr<uchar>(y);
        for(int x = 0; x<cols; x++ ){
            if(grayDstRowsData[x] != 0 )
            {
                if(x<lx){
                    lx = x;
                }
                if(x>rx){
                    rx = x;
                }
                if(y<ty){
                    ty = y;
                }
                if(y>by){
                    by = y;
                }
            }
        }
    }
    //扩大一下截图范围
    if(lx - 10 >= 0)
        lx = lx - 10;
    if(rx + 10 <= cols - 1)
        rx = rx + 10;
    if(ty - 10 >= 0)
        ty = ty - 10;
    if(by + 10 <= rows - 1)
        by = by + 10;
    
    cv::Point lt = cv::Point(lx,ty);
    cv::Point rb = cv::Point(rx,by);
   
    cv::Rect cutRect = *new cv::Rect;
  
    cutRect.x = lx;
    cutRect.y = ty;
    cutRect.width = rx - lx + 1;
    cutRect.height = by - ty + 1;
  
    cv::Mat cutDst;
    CutoutImage::cutImageByRect(dstMat, cutRect, cutDst); //截图
    
    dstMat = cutDst.clone();
    //cv::rectangle(dstMat, lt, rb, cv::Scalar(0,0,255,0));
    //cv::imshow("RRRRRR", dstMat);
}

void CutoutImage::colorDispResult(const cv::Mat picMat, cv::Mat cutPicBitMat, cv::Point cutPicAnchorPoint , cv::Mat &mergeColorMat)
{
    cv::Mat showMat = picMat.clone();
    
    if(showMat.channels() == 3)
        cv::cvtColor(showMat, showMat, CV_BGR2BGRA);
    else if(showMat.channels() == 1)
        cv::cvtColor(showMat, showMat, CV_GRAY2BGRA);
    
    cv::Mat colorCutPic = cutPicBitMat.clone();
    cv::cvtColor(colorCutPic, colorCutPic, CV_GRAY2BGRA);
    
    int colorCutPicRows = colorCutPic.rows;
    int colorCutPicCols = colorCutPic.cols*colorCutPic.channels();
    
 //   cv::imshow("cutPicBitMat", cutPicBitMat);
    
    for(int y= 0;y<colorCutPicRows;y++ ){
        uchar *cutMatRowsData = colorCutPic.ptr<uchar>(y);
        uchar *showMatRowsData = showMat.ptr<uchar>(cutPicAnchorPoint.y+y);
        for(int x = 0; x<colorCutPicCols; x = x+4){
            uchar oneChannelData = cutMatRowsData[x];
            if(oneChannelData == 255) //b
            {
                cutMatRowsData[x + 1] = 0; //g
                cutMatRowsData[x + 2] = 0; //r
                cutMatRowsData[x + 3] = 100; //a
                showMatRowsData[cutPicAnchorPoint.x*4 + x + 0] = 255;
                showMatRowsData[cutPicAnchorPoint.x*4 + x + 1] = showMatRowsData[cutPicAnchorPoint.x*4 + x + 1]/2;
                showMatRowsData[cutPicAnchorPoint.x*4 + x + 2] = showMatRowsData[cutPicAnchorPoint.x*4 + x + 2]/2;
                showMatRowsData[cutPicAnchorPoint.x*4 + x + 3] = 100;
                //x = x + 3;
            }
        }
    }
  //  cv::imshow("colorCutPicRows", colorCutPic);
  //  cv::imshow("showMatColor",showMat);
}

void CutoutImage::colorDispResultWithFullSeedMat(const cv::Mat picMat,const cv::Mat seedMat )
{
    cv::Mat showPic = picMat.clone();
    cv::Mat showSeed = seedMat.clone();
    
    CutoutImage::classCutMat = seedMat;
    
    if(showPic.channels() == 3)
        cv::cvtColor(showPic, showPic, CV_BGR2BGRA);
    else if(showPic.channels() == 1)
        cv::cvtColor(showPic, showPic, CV_GRAY2BGRA);
    
    cv::cvtColor(showSeed, showSeed, CV_GRAY2BGRA);
    
    int rows = picMat.rows;
    int cols = picMat.cols*showPic.channels();
    
    for(int y = 0;y<rows;y++){
        uchar *showPicLineData = showPic.ptr<uchar>(y);
        uchar *showSeedLineData = showSeed.ptr<uchar>(y);
        for(int x = 0;x<cols;x=x+4){
            uchar oneChannelData = showSeedLineData[x];
            if(oneChannelData == 255)
            {
                showPicLineData[x] = 255;
                showPicLineData[x + 1] =  showPicLineData[x + 1]/2;
                showPicLineData[x + 2] =  showPicLineData[x + 2]/2;
                showPicLineData[x + 3] =  255;
            }
        }
    }
    //cv::imshow("showPic",showPic);
    //cv::imshow("orgGray", showPic);
    CutoutImage::classMergeMat = showPic.clone();
}

cv::Mat CutoutImage::getMergeResult(){
    return CutoutImage::classMergeMat;
}

void CutoutImage::edgeBlur( const cv::Mat colorMat, const cv::Mat maskMat, int parameter, cv::Mat &dstMat )
{
    cv::Mat colorMatClone = colorMat.clone();
    cv::Mat maskMatClone = maskMat.clone();
    cv::Mat maskMarClone4smooth = maskMat.clone();
    cv::Mat bgrMaskMat;
    cv::cvtColor(maskMatClone, bgrMaskMat, CV_GRAY2BGR);
    cv::Mat smoothMask;
    cv::Mat tmp;
    CutoutImage::smoothContours( colorMatClone, bgrMaskMat, parameter, tmp, smoothMask );
    CutoutImage::translucentEdge(tmp, smoothMask, maskMat, dstMat);
}
/*
 将输入的二值图边缘平滑，用锐利边缘抠取输入的彩色图，然后再将彩色图与平滑边缘的图进行融合
 */
void CutoutImage::filterImageEdgeAndBlurMerge( const cv::Mat colorMat, const cv::Mat bitMat, cv::Mat &dstMat )
{
    cv::Mat aBitMat = bitMat.clone();
    cv::Mat aColorMat = colorMat.clone();
    cv::Mat filterMat;
    //CutoutImage::filterImage(aBitMat, filterMat); //使预模糊区域大于正常区域
    filterMat = aBitMat;  //使预模糊区域与正常区域一样大
    
    std::cout<<"aColorMat channels =  " <<aColorMat.channels()<<std::endl;
    int blockSize = 5;
    int constValue = 10;
    //    cv::adaptiveThreshold( filterMat, filterMat, 255, CV_ADAPTIVE_THRESH_MEAN_C, CV_THRESH_BINARY_INV, blockSize, constValue );
    cv::threshold(filterMat, filterMat, 1, 255, CV_THRESH_BINARY );
    //jiangbo test
    cv::Mat tmpMMat;
    //CutoutImage::smoothContours( filterMat, tmpMMat );
    
    int rows = aBitMat.rows;
    int cols = aBitMat.cols;
    //扣取范围较大的彩色图，并进行模糊，主要需要其边缘部分数据
    cv::Mat cutBigColorMat = cv::Mat( rows , cols, CV_8UC3, cv::Scalar(255,255,255) );
    for (int y = 0; y < rows; y++) {
        uchar *filterCutMatRowData = filterMat.ptr<uchar>(y);
        uchar *colorMatRowData = aColorMat.ptr<uchar>(y);
        uchar *cutBigColorMatRowData = cutBigColorMat.ptr<uchar>(y);
        for (int x = 0; x < cols; x++) {
            if(filterCutMatRowData[x] != 0){
                cutBigColorMatRowData[x*3]      = colorMatRowData[x*3];
                cutBigColorMatRowData[x*3 + 1]  = colorMatRowData[x*3 + 1];
                cutBigColorMatRowData[x*3 + 2]  = colorMatRowData[x*3 + 2];
            }
        }
    }
    cv::Mat cutBigColorMatFilter;
    //CutoutImage::filterImage(cutBigColorMat, cutBigColorMatFilter);
    CutoutImage::filterImageForEdgeBlur(cutBigColorMat, cutBigColorMatFilter);
    
    
    cv::Mat bgrFilterMat;
    cv::cvtColor(filterMat, bgrFilterMat, CV_GRAY2BGR);
    cv::Mat smoothMask;
    CutoutImage::smoothContours(colorMat, bgrFilterMat, 21 , tmpMMat, smoothMask);
    
    
    cv::Mat fooRusultMat = cv::Mat( rows , cols, CV_8UC3, cv::Scalar(0,0,0) );
    cv::Mat testEdgeData= cv::Mat( rows , cols, CV_8UC3, cv::Scalar(0,0,0) );
    //融合
    for(int y = 0; y < rows; y++){
        uchar *aBitMatRowData = aBitMat.ptr<uchar>(y);
        uchar *colorMatRowData = aColorMat.ptr<uchar>(y);
        uchar *cutBigColorMatRowData = cutBigColorMatFilter.ptr<uchar>(y);
        uchar *fooRusultMatRowData = fooRusultMat.ptr<uchar>(y);
        uchar *testEdgeDataRowData = testEdgeData.ptr<uchar>(y);
        for(int x = 0; x < cols; x++){
            if(aBitMatRowData[x] != 0){
                fooRusultMatRowData[x*3]     = colorMatRowData[x*3];
                fooRusultMatRowData[x*3 + 1] = colorMatRowData[x*3 + 1];
                fooRusultMatRowData[x*3 + 2] = colorMatRowData[x*3 + 2];
            }
            
            if(cutBigColorMatRowData[x*3] != 255 || cutBigColorMatRowData[x*3 + 1] != 255 || cutBigColorMatRowData[x*3 + 2] != 255){
                if(aBitMatRowData[x] == 0){
                    fooRusultMatRowData[x*3]     = cutBigColorMatRowData[x*3];
                    fooRusultMatRowData[x*3 + 1] = cutBigColorMatRowData[x*3 + 1];
                    fooRusultMatRowData[x*3 + 2] = cutBigColorMatRowData[x*3 + 2];
                    testEdgeDataRowData[x*3]     = cutBigColorMatRowData[x*3];
                    testEdgeDataRowData[x*3 + 1] = cutBigColorMatRowData[x*3 + 1];
                    testEdgeDataRowData[x*3 + 2] = cutBigColorMatRowData[x*3 + 2];
                }
            }
            
        }
    }
    
//    cv::imshow("cutBigColorMat", cutBigColorMat);
//    cv::imshow("cutBigColorMatFilter", cutBigColorMatFilter);
//    cv::imshow("fooRusultMat", fooRusultMat);
//    cv::imshow(" testEdgeData ", testEdgeData);
}

void CutoutImage::makeWhite2Black( const cv::Mat srcMat, cv::Mat &dstMat)
{
    cv::Mat srcMatClone = srcMat.clone();
    srcMatClone.convertTo(srcMatClone, CV_8UC3 ,255.0);
    std::cout<< srcMatClone.channels() <<std::endl;
    //dstMat = cv::Mat(srcMat.size(),CV_8UC3,cv::Scalar(0,0,0));
    int rows = srcMatClone.rows;
    int cols = srcMatClone.cols;
    for(int y = 0; y<rows; y++){
        uchar *srcMatCloneRowData = srcMatClone.ptr<uchar>(y);
        //uchar *dstMatRowData = dstMat.ptr<uchar>(y);
        for (int x = 0; x<cols; x++) {
            if(srcMatCloneRowData[x*3] == 255 && srcMatCloneRowData[x*3 + 1] == 255  && srcMatCloneRowData[x*3 + 2] == 255)
            {
                srcMatCloneRowData[x*3    ] = 0;
                srcMatCloneRowData[x*3 + 1] = 0;
                srcMatCloneRowData[x*3 + 2] = 0;
            }
        }
    }
    dstMat = srcMatClone.clone();
}

void  CutoutImage::smoothContours(const cv::Mat srcMat ,const cv::Mat cutMat, int parameter, cv::Mat &dstMat ,cv::Mat &smoothMask)
{
    cv::Mat img= cutMat.clone();
//    Mat whole_image=imread("D:\\ImagesForTest\\lena.jpg");
    cv::Mat whole_image= srcMat.clone();
    whole_image.convertTo(whole_image,CV_32FC3,1.0/255.0);
    cv::resize(whole_image,whole_image,img.size());
    img.convertTo(img,CV_32FC3,1.0/255.0);
    
//    cv::imshow("img",img);
    cv::Mat bg=cv::Mat(img.size(),CV_32FC3);
    bg=cv::Scalar(1.0,1.0,1.0);
    
    // Prepare mask
    cv::Mat mask;
    cv::Mat img_gray;
    cv::cvtColor(img,img_gray,cv::COLOR_BGR2GRAY);
    img_gray.convertTo(mask,CV_32FC1);
    threshold(1.0-mask,mask,0.9,1.0,cv::THRESH_BINARY_INV);
    
    cv::GaussianBlur(mask,mask,cv::Size(parameter,parameter),11.0);
    //cv::GaussianBlur(mask, mask, cv::Size(7,7), 11.0);
//    cv::imshow("mask",mask);
    smoothMask = mask.clone();
    //cv::waitKey(0);
    
    // Reget the image fragment with smoothed mask
    cv::Mat res;
    
    std::vector<cv::Mat> ch_img(3);
    std::vector<cv::Mat> ch_bg(3);
    cv::split(whole_image,ch_img);
    cv::split(bg,ch_bg);
    ch_img[0]=ch_img[0].mul(mask)+ch_bg[0].mul(1.0-mask);
    ch_img[1]=ch_img[1].mul(mask)+ch_bg[1].mul(1.0-mask);
    ch_img[2]=ch_img[2].mul(mask)+ch_bg[2].mul(1.0-mask);
    cv::merge(ch_img,res);
    cv::merge(ch_bg,bg);
    cv::Mat bg8UC3;
    bg.convertTo(bg8UC3, CV_8UC3,255);
    cv::Mat resultMat;
    CutoutImage::makeWhite2Black( res, resultMat );
    dstMat = resultMat.clone();
    //cv::imshow("result",res);
    //cv::imshow("result2",resultMat);
    //cv::waitKey(0);
    //cv::destroyAllWindows();
}

/*
4通道数据希望能达到边缘透明的效果
 */
void  CutoutImage::smoothContoursAlphard(const cv::Mat srcMat ,const cv::Mat cutMat, int parameter, cv::Mat &dstMat)
{
    cv::Mat img= cutMat.clone();
//    Mat whole_image=imread("D:\\ImagesForTest\\lena.jpg");
    cv::Mat whole_image= srcMat.clone();
    whole_image.convertTo(whole_image,CV_32FC4,1.0/255.0);
    cv::resize(whole_image,whole_image,img.size());
    img.convertTo(img,CV_32FC4,1.0/255.0);
    
//    cv::imshow("img",img);
    cv::Mat bg=cv::Mat(img.size(),CV_32FC4);
    bg=cv::Scalar(1.0,1.0,1.0,1.0);
   
    
    // Prepare mask
    cv::Mat mask;
    cv::Mat img_gray;
    cv::cvtColor(img,img_gray,cv::COLOR_BGR2GRAY);
    img_gray.convertTo(mask,CV_32FC1);
    threshold(1.0-mask,mask,0.9,1.0,cv::THRESH_BINARY_INV);
    
    cv::GaussianBlur(mask,mask,cv::Size(parameter,parameter),11.0);
    //cv::GaussianBlur(mask, mask, cv::Size(7,7), 11.0);
//    cv::imshow("mask",mask);
    cv::waitKey(0);
    
    // Reget the image fragment with smoothed mask
    cv::Mat res;
    
    std::vector<cv::Mat> ch_img(4);
    std::vector<cv::Mat> ch_bg(4);
    cv::split(whole_image,ch_img);
    cv::split(bg,ch_bg);
    ch_img[0]=ch_img[0].mul(mask)+ch_bg[0].mul(1.0-mask);
    ch_img[1]=ch_img[1].mul(mask)+ch_bg[1].mul(1.0-mask);
    ch_img[2]=ch_img[2].mul(mask)+ch_bg[2].mul(1.0-mask);
    ch_img[3]=ch_img[3].mul(mask)+ch_bg[3].mul(1.0-mask);
    cv::merge(ch_img,res);
    cv::merge(ch_bg,bg);
    
    cv::Mat bg8UC4;
    
    bg.convertTo(bg8UC4, CV_8UC4,255);
//    cv::imshow("bg8UC4", bg8UC4);
   
    cv::Mat resultMat;
    CutoutImage::makeWhite2Black( res, resultMat );
    dstMat = resultMat.clone();
    //cv::imshow("result",res);
    //cv::imshow("result2",resultMat);
    //cv::waitKey(0);
    //cv::destroyAllWindows();
}

void  CutoutImage::smoothContoursAngles( const std::vector<std::vector<cv::Point>> contoursIn, std::vector<std::vector<cv::Point>> &contoursOut )
{
    for(int i = 0; i< (int)contoursIn.size(); i++)
    {
        std::vector<cv::Point> currentContours = contoursIn[i];
        if((int)currentContours.size() > 2) //3个点
        {
            for(int j = 0; j < currentContours.size() - 2; j++)
            {
                cv::Point firstPoint = currentContours[j];
                cv::Point secondPoint = currentContours[j+1];
                cv::Point thirdPoint = currentContours[j+2];
                cv::Point p1 = cv::Point( (firstPoint.x - secondPoint.x) , -(firstPoint.y - secondPoint.y) );
                cv::Point p2 = cv::Point( (thirdPoint.x - secondPoint.x) , -(thirdPoint.y - secondPoint.y) );
                
                float angle = CutoutImage::angleBetween(p1, p2);
                
                if(angle < CV_PI/2)
                {
                    contoursOut[i].push_back(secondPoint);
                }
            }
        }
    }
}
//求解两个点之间的夹角
float CutoutImage::angleBetween(const cv::Point v1, const cv::Point v2)
{
    float len1 = sqrt(v1.x * v1.x + v1.y * v1.y);
    float len2 = sqrt(v2.x * v2.x + v2.y * v2.y);
    
    float dot = v1.x * v2.x + v1.y * v2.y;
    
    float a = dot / (len1 * len2);
    
    if (a >= 1.0)
        return 0.0;
    else if (a <= -1.0)
        return CV_PI;
    else
        return acos(a); // 0..PI
}

void CutoutImage::translucentEdge( const cv::Mat srcMat, const cv::Mat smoothMask, const cv::Mat liteMask, cv::Mat &dstMat ) //liteMask 是边缘模糊之前的数据
{
    cv::Mat smoothMask8uc1;
    smoothMask.convertTo(smoothMask8uc1, CV_8UC1 ,255.0);
    int rows = smoothMask8uc1.rows;
    int cols = smoothMask8uc1.cols;
    //cv::threshold(smoothMask8uc1, bitSmoothMask, 1, 255, CV_THRESH_BINARY );
    cv::Mat srcMatClone = srcMat.clone();
    cv::Mat liteMaskClone = liteMask.clone();
    cv::Mat edgeSmoothMask = cv::Mat( smoothMask.size(), CV_8UC1, cv::Scalar(0));
    cv::cvtColor(srcMatClone, srcMatClone, CV_BGR2BGRA);
    for(int y = 0; y < rows; y++){
        uchar *liteMaskCloneRowData = liteMaskClone.ptr<uchar>(y);
        uchar *smoothMask8uc1RowData = smoothMask8uc1.ptr<uchar>(y);
        uchar *srcMatCloneRowData = srcMatClone.ptr<uchar>(y);
        for (int x = 0; x < cols; x++) {
            //if(smoothMask8uc1RowData[x] != 0 && liteMaskCloneRowData[x] == 0)
            //if(smoothMask8uc1RowData[x] != 0 && smoothMask8uc1RowData[x] != 255)
            {
                srcMatCloneRowData[x*4 + 3] = smoothMask8uc1RowData[x];
            }
        }
    }
    dstMat = srcMatClone.clone();
    //cv::imwrite("cc.png", srcMat);
    //cv::imwrite("ccc.png", srcMatClone);
    //cv::waitKey(0);
}

void CutoutImage::cutImageByRect(const cv::Mat srcMat, cv::Rect cutRect,cv::Mat &dst)
{
    cv::Mat srcMat4Cut = srcMat.clone();
    int srcRow = srcMat.rows;
    int srcCol = srcMat.cols;
    int dstRow = cutRect.height;
    int dstCol = cutRect.width;
    int srcChannel = srcMat.channels();
    
    dst = cv::Mat( dstRow, dstCol, CV_8UC4, cv::Scalar(0,0,0,0));
    int dstChannel = dst.channels();
    for(int y = 0; y < dstRow; y++ ){
        uchar *srcMat4CutRowData = srcMat4Cut.ptr<uchar>(y+cutRect.y);
        uchar *dstMatRowData = dst.ptr<uchar>(y);
        for( int x = 0; x < dstCol; x++){
            dstMatRowData[x * 4] = srcMat4CutRowData[(x + cutRect.x)*4];
            dstMatRowData[x * 4 + 1] = srcMat4CutRowData[(x + cutRect.x)*4 + 1];
            dstMatRowData[x * 4 + 2] = srcMat4CutRowData[(x + cutRect.x)*4 + 2];
            dstMatRowData[x * 4 + 3] = srcMat4CutRowData[(x + cutRect.x)*4 + 3];
        }
    }
    
//    cv::imshow("cutedMat ", dst);
//    cv::imwrite("cutImg.png", dst);
}

void CutoutImage::getTestMat(cv::Mat & dstMat1, cv::Mat &dstMat2 )
{
    dstMat1 = cutMat1;
    dstMat2 = cutMat2;
}

cv::Mat CutoutImage::scaleFCMI2InputColorImageSize( const cv::Mat srcMat )
{
    cv::Mat aMat;// = cv::Mat(inputColorImageSize, srcMat.channels());
    cv::Mat rMat = cv::Mat(inputColorImageSize, CV_8UC4, cv::Scalar(0,0,0,0));  //cv::Mat(inputColorImageSize, srcMat.channels());
    if(srcMat.rows >= srcMat.cols)  //最终图像是竖直的
    {
        int scaleToHeight = inputColorImageSize.height;
        int scaleToWidth = (int)(((float)srcMat.cols/(float)srcMat.rows)*(float)inputColorImageSize.height);
        cv::resize(srcMat, aMat, cv::Size(scaleToWidth,scaleToHeight));
        cv::Rect copyRect = cv::Rect( (inputColorImageSize.width - scaleToWidth)/2,0,scaleToWidth,scaleToHeight );
        aMat.copyTo(rMat(copyRect));
    }
    else                            //最终图像是横向的
    {
        int scaleToHeight = int(((float)srcMat.rows/(float)srcMat.cols)*(float)inputColorImageSize.width);
        int scaleToWidth =  inputColorImageSize.width;// (int)(((float)srcMat.cols/(float)srcMat.rows)*(float)inputColorImageSize.height);
        cv::resize(srcMat, aMat, cv::Size(scaleToWidth,scaleToHeight));
        cv::Rect copyRect = cv::Rect( (inputColorImageSize.width - scaleToWidth)/2,0,scaleToWidth,scaleToHeight );
        aMat.copyTo(rMat(copyRect));
    }
    
    int rows = rMat.rows;
    int cols = rMat.cols;
    
    for(int y=0; y<rows; y++){
        uchar *rowsData = rMat.ptr<uchar>(y);
        for (int x = 0; x<cols*4; x = x+4) {
            //rowsData[x] = 0;
            rowsData[x + 3] = 0;
        }
    }
    
    return rMat;
}
