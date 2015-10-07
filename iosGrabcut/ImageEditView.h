//
//  ImageEditView.h
//  cutImageIOS
//
//  Created by vk on 15/9/1.
//  Copyright (c) 2015年 quxiu8. All rights reserved.
//

#import <UIKit/UIKit.h>
#import "DrawView.h"
#import "Bridge2OpenCV.h"

@interface ImageEditView : UIView<Bridge2OpenCVDelegate,DrawViewDelegate>

-(id)initWithFrame:(CGRect)frame;
-(void)setPicture:(UIImage *)setImage;
-(void) setMove:(BOOL) isMove;
-(void) redo;
-(void) undo;
-(void) setLineScale:(float) scale;
-(void) resetAllMask;
-(UIImage *)getReusltImage;



@property (nonatomic) BOOL isMove; //移动时最好禁用其他操作
@property (nonatomic) BOOL isDraw; //YES是直接画mark NO是添加生长点
@property (nonatomic) BOOL isDelete; //YES则调用删除点，NO是再判断上面的Draw

@end
