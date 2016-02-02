//
//  bridge2OpenCV.h
//  cutImageIOS
//
//  Created by vk on 15/8/24.
//  Copyright (c) 2015年 Clover. All rights reserved.
//

#import <Foundation/Foundation.h>
#import <UIKit/UIKit.h>

@protocol Bridge2OpenCVDelegate  <NSObject>
-(void) resultImageReady:(UIImage *) sendImage;
@end

@interface Bridge2OpenCV : NSObject
-(id) init;
-(id) initWithImage:(UIImage *)setImage;
-(void) setCalculateImage:(UIImage *)setImage andWindowSize:(CGSize) winSize;
-(void) setDrawPoint:(NSMutableArray*)selectPoint andLineWidth:(int)lineWidth;
-(void) setCreatPoint:(NSMutableArray*)selectPoint andLineWidth:(int)lineWidth;
-(void) setDeletePoint:(NSMutableArray*)selectPoint andLineWidth:(int)lineWidth;
-(UIImage *) getCutResult;
-(NSMutableArray *) getMutableCutResult;
-(void) resetAllMask;
-(void) redoPoint;
-(void) undoPoint;

@property (nonatomic,weak) id<Bridge2OpenCVDelegate> delegate;

@end

