//
//  CloverGrabCut.cpp
//  opencv_test
//
//  Created by vk on 15/9/6.
//  Copyright (c) 2015年 clover. All rights reserved.
//

#include "CloverGrabCut.h"


CloverGrabCut::CloverGrabCut()
{

}

CloverGrabCut::~CloverGrabCut()
{

}

void CloverGrabCut::setImage(cv::Mat imgMat)
{
    _srcImgMat = imgMat.clone();
    printf("RRR imgMat.rows = %d\n",imgMat.rows);
    printf("RRR imgMat.cols = %d\n",imgMat.cols);
    
    //_maskStoreMat  = cv::Mat(imgMat.size(), CV_8UC1, cv::Scalar(0));
}

cv::Mat CloverGrabCut::getFGByMask(cv::Mat mask)
{
    cv::Mat fg = cv::Mat::zeros(_srcImgMat.size(), _srcImgMat.type());
    //Mat aMask = mask;
    
    printf("fg.cols = %d, fg.rows = %d\n",fg.cols, fg.rows);
    printf("mask.cols = %d, mask.rows = %d\n",mask.cols, mask.rows);
    _srcImgMat.copyTo(fg, mask);
    return fg;
}
/**
 *  计算输入maskMat中的线条的mask
 *  本算法中一个maskMat中只有一个线条
 *  @param maskMat 输入的带有线条的mask
 *
 *  @return 返回线条的rect
 */
cv::Rect CloverGrabCut::getMaskRct( cv::Mat maskMat )
{
    int rows = maskMat.rows;
    int cols = maskMat.cols;
    uchar grayWillBeFind = 1;
    
    cv::Mat tmpMat;
    int lx = cols;
    int rx = 0;
    int ty = rows;
    int by = 0;
    
    for(int y = 0; y<rows; y++ ){
        uchar *maskMatRowsData = maskMat.ptr<uchar>(y);
        for(int x = 0; x<cols; x++ ){
            if(maskMatRowsData[x] == grayWillBeFind )
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
   
    //showBit(maskMat, tmpMat );
    cv::Rect rectFind = cv::Rect(lx,ty, rx-lx+1, by-ty+1);
    cv::rectangle(tmpMat, rectFind, cv::Scalar(0,0,255));
    //cv::imshow("tmpMat", tmpMat);
    
    return rectFind;
}

cv::Mat  CloverGrabCut::filterMaskAndMergeMat(cv::Mat srcMat, cv::Mat matStore, cv::Mat filterRuleMat)
{
    cv::Mat resultMat = matStore.clone();//cv::Mat(srcMat.size(),CV_8UC1,cv::Scalar(0));
    printf(" channels = %d\n ",resultMat.channels());
    
    cv::Mat aMatStore = matStore.clone();
    if (srcMat.channels() != 1){
        printf("EEERRRR\n");
        return resultMat;
    }
    int rows = srcMat.rows;
    int cols = srcMat.cols;
    
    cv::Rect filterRect = getMaskRct(filterRuleMat);
    
    cv::Mat contoursMat = cv::Mat(srcMat.size(), CV_8UC1, cv::Scalar(0));
    std::vector<std::vector<cv::Point>> contours;
    cv::findContours( srcMat.clone(), contours, CV_RETR_EXTERNAL, CV_CHAIN_APPROX_NONE );
    
    int lx = rows;
    int rx = 0;
    int ty = cols;
    int by = 0;
    /**
     *  计算每个contour的rect,同时与画线的rect比较
     *  若符合要求则留下，否则剔除
     */
    
    for(int i = 0;i<contours.size();i++){
        lx = rows;
        rx = 0;
        ty = cols;
        by = 0;
        for(int j = 0;j<contours[i].size();j++)
        {
            cv::Point perPoint = contours[i][j];
            if(perPoint.x < lx)
                lx = perPoint.x;
            if(perPoint.x > rx)
                rx = perPoint.x;
            if(perPoint.y < ty)
                ty = perPoint.y;
            if(perPoint.y > by)
                by = perPoint.y;
        }
        //cv::Point lt = cv::Point(lx,ty);
        cv::Rect rectDraw = cv::Rect(lx,ty,rx-lx+1,by-ty+1);
        
        if(contours.size() == 1){
            printf("!!!!!mergeMat( srcMat, aMatStore, rectDraw )!!!!!!\n");
            drawContours( contoursMat, contours, i, cv::Scalar(255), CV_FILLED);
//            resultMat = mergeMat( srcMat, aMatStore, rectDraw ); //若只有一个contur则直接返回
            resultMat = mergeMat( contoursMat, aMatStore, rectDraw ); //若只有一个contur则直接返回
        }
        else{
            if( diffRect(rectDraw,filterRect) )
            {
                /**
                 *  画一个contour然后直接当srcMat传入merge函数
                 */
                cv::drawContours( contoursMat, contours, i, cv::Scalar(255), CV_FILLED);
                resultMat = mergeMat(contoursMat, aMatStore, rectDraw);
            }
            else{
                printf("?????\n");
            }
        }
    }
    printf("resultMat.rows = %d\n",resultMat.rows);
    printf("resultMat.cols = %d\n",resultMat.cols);
    printf("filter end\n");
    return resultMat;
}

/**
 *  比较两个rect是否是包含关系，r1包含r2则返回ture,否则是false
 *
 *  @param r1 是否包含r2
 *  @param r2 是否被r1包含
 *
 *  @return r1包含r2则返回ture 否则false
 */
bool CloverGrabCut::diffRect(cv::Rect r1, cv::Rect r2)
{
   if( r1.x <= r2.x && r1.y <= r2.y ){
        if(r1.width >= r2.width && r1.height >= r2.height){
            if(r1.width + r1.x - 1 >= r2.width + r2.x - 1 && r1.height + r1.y - 1 >= r2.height + r2.y - 1){
                return true;
            }
            else{
                return false;
            }
        }
        else{
            return false;
        }
    }
    else{
        return false;
    }
    return false;
}

/**
 *  将第一个mat中的mergeRect位置上的内容合并的到第二个mat的mergeRect位置上
 *  输入输出都是单通道图像，前景为255背景为0
 *
 *  @param mergeMat   要合并的Mat
 *  @param mergeToMat 要合并到的Mat
 *  @param mergeRect  要合并的区域
 *
 *  @return 合并结果
 */
cv::Mat CloverGrabCut::mergeMat( cv::Mat mergeMat, cv::Mat mergeToMat, cv::Rect mergeRect )
{
    int liteRows = mergeRect.height;
    int liteCols = mergeRect.width;
    //int rows = mergeToMat.rows;
    //int cols = mergeToMat.cols;
    
    cv::Mat  newMat = mergeToMat.clone();
    
    //Mat liteMat = Mat(liteRows, liteCols, CV_8UC1, Scalar(0));
    for (int y = 0; y<liteRows; y++) {
        uchar *mergeMatRowData = mergeMat.ptr<uchar>(mergeRect.y + y);
        uchar *newMatRowData = newMat.ptr<uchar>(mergeRect.y + y);
        //  uchar *liteMatRowData = liteMat.ptr<uchar>(y);
        for (int x = 0; x<liteCols; x++) {
            newMatRowData[mergeRect.x + x] = mergeMatRowData[mergeRect.x + x] | newMatRowData[mergeRect.x + x];
            //     liteMatRowData[x] = mergeMatRowData[mergeRect.x + x];
        }
    }
    //imshow("liteMat", liteMat);
    
    //liteMat.copyTo(newMat(mergeRect));  //如果用这个则会造成若新的mask与旧mask部分重合且有部分区域少于旧mask则会造成删除旧mask
    return newMat;
}

cv::Mat CloverGrabCut::getBinMaskByMask(cv::Mat mask)
{
    cv::Mat binmask(mask.size(), CV_8U);
    binmask = mask& cv::GC_FGD;
    binmask = binmask * 255;
    cv::Mat tmp;
    binmask.copyTo(tmp);
    std::vector<std::vector<cv::Point> > co;
    std::vector<cv::Vec4i> hi;
    binmask *= 0;
    findContours(tmp,co,hi,CV_RETR_EXTERNAL,CV_CHAIN_APPROX_NONE);
    for(int i=0;i<co.size();i++){
        if(contourArea(cv::Mat(co[i])) < 50) continue;
        drawContours(binmask, co,i, CV_RGB(255,255,255), CV_FILLED, CV_AA);
    }
    //binmask.copyTo(_bin);
    return binmask;
}

void CloverGrabCut::grabcutByMergeToMatAndMskMat(const cv::Mat mergeToMat ,const cv::Mat msk, cv::Mat & resultMaskMat, cv::Mat &resultColorMat)
{
    cv::Mat srcImage = _srcImgMat;
    cv::Mat aGcut = msk.clone();
    cv::Size orgSize = msk.size();
    
    cv::Mat mskClone = msk.clone();
    cv::Mat aBgd;
    cv::Mat aFgd;
    
    cv::Mat srcScale;
    cv::Mat aGcutScale;
//    cv::grabCut(srcImage, aGcut,cv::Rect(), aBgd, aFgd, 1, cv::GC_INIT_WITH_MASK);
    scaleImg(srcImage, 0.25, srcScale);
    scaleImg(aGcut, 0.25, aGcutScale);
   
    int c = cv::countNonZero(aGcutScale);
    if(c>0){
        checkGcut(aGcutScale);
        cv::grabCut(srcScale, aGcutScale,cv::Rect(), aBgd, aFgd, 1, cv::GC_INIT_WITH_MASK);
    }
    else
        printf("!!!!!!!!!!!!!!!!!!!!!!!\n");
    scaleImg(aGcutScale, 4, aGcut);
    cv::resize(aGcut, aGcut, orgSize);
    cv::Mat bitMask = getBinMaskByMask(aGcut);
    
    deleteBlackIsland(bitMask, bitMask);
    filterImage(bitMask, bitMask);
    cv::threshold(bitMask, bitMask, 150, 255, CV_THRESH_BINARY);
    
    cv::Mat send = bitMask.clone();
    cv::Mat sendFilter;
    cv::Mat resultMat = filterMaskAndMergeMat(send, mergeToMat, mskClone);
    resultMaskMat = resultMat.clone();
    cv::Mat fg = getFGByMask(resultMat);//getFG();
}

cv::Mat CloverGrabCut::pointArray2MatWithLineWidth( std::vector<cv::Point> selectPoint, int lineWidth, cv::Size matSize )
{
    int rows = matSize.height;
    int cols = matSize.width;
    int vectorSize = (int)selectPoint.size();
    cv::Mat drawLineMat = cv::Mat::ones(matSize,CV_8UC1)*cv::GC_PR_BGD ;
    
    for(int loop = 0;loop < vectorSize-1;loop ++)
    {
        cv::Point p1 = selectPoint[loop];
        cv::Point p2 = selectPoint[loop + 1];
        cv::line(drawLineMat, p1, p2, cv::Scalar( cv::GC_FGD ),lineWidth);
    }
    if(vectorSize == 1) //单击一下
    {
        cv::line(drawLineMat, selectPoint[0], selectPoint[0], cv::Scalar(cv::GC_FGD),lineWidth);
    }
    
    return drawLineMat;
}

void CloverGrabCut::processGrabCut(std::vector<cv::Point> maskPoint, int lineWidth, cv::Mat &resultMaskMat, cv::Mat &resultColorMat)
{
    cv::Size matSize = _srcImgMat.size();
    cv::Mat lineMat = pointArray2MatWithLineWidth( maskPoint, lineWidth, matSize );
    _maskStoreMat = resultMaskMat.clone();
    grabcutByMergeToMatAndMskMat( _maskStoreMat , lineMat, resultMaskMat, resultColorMat);
    //_maskStoreMat = resultMaskMat.clone();
}


void CloverGrabCut::scaleImg(const cv::Mat srcMat, float scaleLevel, cv::Mat &dstMat)
{
    cv::Size newSize = cv::Size( int(srcMat.cols*scaleLevel), int(srcMat.rows*scaleLevel) );
    if(scaleLevel < 1)
        cv::resize(srcMat, dstMat, newSize, CV_INTER_AREA);
    else
        cv::resize(srcMat, dstMat, newSize);
}

void CloverGrabCut::deleteBlackIsland(const cv::Mat srcBitMat ,cv::Mat &dstBitMat)
{
    cv::Mat a_mat = srcBitMat.clone();
    std::vector<std::vector<cv::Point>> contours;
    dstBitMat = cv::Mat(a_mat.rows, a_mat.cols, CV_8UC1,cv::Scalar(0));
    cv::findContours(a_mat, contours, CV_RETR_EXTERNAL, CV_CHAIN_APPROX_NONE);
    cv::drawContours(dstBitMat, contours, -1, cv::Scalar(255),CV_FILLED);
}
void CloverGrabCut::filterImageForEdgeBlur(const cv::Mat imFrame,cv::Mat & outFrame)
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
void CloverGrabCut::filterImage(const cv::Mat imFrame,cv::Mat & outFrame)
{
    /* Soften image */
    cv::Mat tmpMat;
    cv::GaussianBlur(imFrame, tmpMat, cv::Size(7,7), 0,0);
    cv::medianBlur(tmpMat, tmpMat,3);
    cv::Mat kernelMat = getStructuringElement(CV_SHAPE_RECT, cv::Size(3,3),cv::Point(2,2));
    cv::morphologyEx(tmpMat, outFrame, CV_MOP_OPEN,kernelMat);
}

void CloverGrabCut::checkGcut(cv::Mat & srcMat)
{
    int rows = srcMat.rows;
    int cols = srcMat.cols;
    int prtInt = 0;
    
    for(int y = 0; y < rows; y++){
        uchar *rowData = srcMat.ptr<uchar>(y);
        for(int x = 0; x<cols; x++){
            if(rowData[x] == 1){
                prtInt ++;
            }
        }
    }
    printf("prtInt = %d\n",prtInt);
    if(prtInt < 6){
        for(int y = 0; y < rows; y++){
            uchar *rowData = srcMat.ptr<uchar>(y);
            for(int x = 0; x<cols; x++){
                if(prtInt>6)
                    break;
                if(rowData[x] == 1){
                    if( y!=0  && srcMat.ptr<uchar>(y-1)[x] != 1 )  {   //上
                        srcMat.ptr<uchar>(y-1)[x] = 1;
                        prtInt ++;
                    }
                    else
                    {
                        if( x != cols && rowData[x+1] == 1 ){
                            rowData[x+1] = 1;
                            prtInt ++;
                        }
                        else{
                            if( y != rows &&  srcMat.ptr<uchar>(y+1)[x] != 1 ){
                                srcMat.ptr<uchar>(y+1)[x] = 1;
                                prtInt ++;
                            }
                            else{
                                if( x != 0 && rowData[x-1] != 0 ){
                                    rowData[x-1] = 1;
                                    prtInt ++;
                                }
                                else{
                                    printf("!!!!EEEEEEEEERRrrrrrrrrrrrrrr\n");
                                }
                            }
                        }
                    }
                }
            }
        }   
    }
    else{
        return;
    }
    checkGcut( srcMat);
}
