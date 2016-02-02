//
//  DrawViewModel.h
//  cutImageIOS
//
//  Created by vk on 15/8/31.
//  Copyright (c) 2015年 Clover. All rights reserved.
//
#import <Foundation/Foundation.h>
#import <UIKit/UIKit.h>

@interface DrawViewModel : NSObject

+ (id)viewModelWithColor:(UIColor *)color Path:(UIBezierPath *)path Width:(CGFloat)width;

@property (strong, nonatomic) UIColor *color;

@property (strong, nonatomic) UIBezierPath *path;

@property (assign, nonatomic) CGFloat width;

@end

