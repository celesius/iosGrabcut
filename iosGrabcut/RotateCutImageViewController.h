//
//  RotateCutImageViewController.h
//  cutImageIOS
//
//  Created by vk on 15/9/9.
//  Copyright (c) 2015年 quxiu8. All rights reserved.
//

#import <UIKit/UIKit.h>

@interface RotateCutImageViewController : UIViewController <UIGestureRecognizerDelegate>

-(void) setImageRect:(CGRect) setRect andImage:(UIImage *)setImage;

@end
