//
//  ImageShowView.h
//  cutImageIOS
//
//  Created by vk on 15/8/31.
//  Copyright (c) 2015年 quxiu8. All rights reserved.
//

#import <UIKit/UIKit.h>

@protocol DrawViewDelegate <NSObject>

-(void) setPointArray:(NSMutableArray *) pointArray andLineWide:(float) linaWidth;

@end


@interface DrawView: UIView
-(id) initWithFrame:(CGRect)frame andIsCutImage:(BOOL)isCutImage;
-(void) setPhotoImage:(UIImage *)setImage;
-(void) setMove:(BOOL) isMove;
-(void) redo;
-(void) undo;
-(void) resetDraw;
//-(void) setLineScale:(float) scale;
/**
 *  设置的线宽和颜色
 */
@property (assign, nonatomic) CGFloat lineWidth;
@property (strong, nonatomic) UIColor *lineColor;
@property (nonatomic, assign) float lineScale;
@property (nonatomic, weak) id<DrawViewDelegate> delegate;
@end
