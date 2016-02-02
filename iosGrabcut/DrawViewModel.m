//
//  DrawViewModel.m
//  cutImageIOS
//
//  Created by vk on 15/8/31.
//  Copyright (c) 2015年 Clover. All rights reserved.
//
#import "DrawViewModel.h"

@implementation DrawViewModel

+ (id)viewModelWithColor:(UIColor *)color Path:(UIBezierPath *)path Width:(CGFloat)width
{
    DrawViewModel *drawViewModel = [[DrawViewModel alloc] init];
   
    drawViewModel.color = color;
    drawViewModel.path = path;
    drawViewModel.width = width;
   
    
    
    return drawViewModel;
}
@end