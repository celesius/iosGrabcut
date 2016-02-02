//
//  ViewController.m
//  cutImageIOS
//
//  Created by vk on 15/8/21.
//  Copyright (c) 2015年 Clover. All rights reserved.
//  方形编辑区域

#import "ViewController.h"
#import <Foundation/Foundation.h>
#import "UIImage+IF.h"
#import "RotateCutImageViewController.h"
//#import "CustomPopAnimation.h"
//#import "ImageShowView.h"
#import "ImageEditView.h"
#define LINESTEP 5
#define DEFLINEWIDTH 10

@interface ViewController ()

@property (nonatomic, strong) UIImage* photoImg;
@property (nonatomic, strong) ImageEditView  *appImageView;
//@property (nonatomic, strong) UIImageView *showImgView;
@property (nonatomic, strong) UIButton *openPhotoAlbum;
@property (nonatomic, strong) UIButton *addCalculatePoint;
@property (nonatomic, strong) UIButton *addMaskPoint;
@property (nonatomic, strong) UIButton *deleteMaskPoint;
@property (nonatomic, strong) UIButton *moveImg;
@property (nonatomic, strong) UIButton *undoButton; //前进
@property (nonatomic, strong) UIButton *redoButton; //返回
@property (nonatomic, strong) UIButton *nextStep;


@property (nonatomic, strong) NSMutableArray *pointArray;  //同时发送的只能有一组array, 删除，添加，选取都是这一个array
@property (nonatomic, strong) UIButton *sysTestButton;
@property (nonatomic, strong) UIButton *resetDrawButton;
@property (nonatomic) CGRect orgRect;
//@property (nonatomic, strong) Bridge2OpenCV *b2opcv;
@property (nonatomic) CGSize imgWindowSize;
@property (nonatomic) CGAffineTransform orgTrf;
@property (nonatomic) BOOL isMove; //移动时最好禁用其他操作
@property (nonatomic) BOOL isDraw; //YES是直接画mark NO是添加生长点
@property (nonatomic) BOOL isDelete; //YES则调用删除点，NO是再判断上面的Draw

@property (nonatomic) int setLineWidth;
@property (nonatomic, strong) UIGestureRecognizer *panGestureRecognizer;
@property (nonatomic, strong) UIPinchGestureRecognizer *pinchGestureRecognizer;

@property (nonatomic, strong)  RotateCutImageViewController *rotateCutImageViewController;

@property (nonatomic, strong) dispatch_queue_t getFinnalImageQueue;

@end

@implementation ViewController

- (void)viewDidLoad {
    [super viewDidLoad];
    self.navigationController.delegate = self;
    self.navigationController.navigationBar.translucent = YES;
    self.navigationController.navigationBarHidden = YES;
    self.rotateCutImageViewController = [[RotateCutImageViewController alloc]init];
    
    float smallButtonWidth = 50;
    float bigButtonWidth = 100;
    self.setLineWidth = DEFLINEWIDTH;              //线宽默认是10
    CGRect mainScreen = [[UIScreen mainScreen] applicationFrame];
    NSLog(@" mainScreen.size.height = %f  mainScreen.size.width = %f ",mainScreen.size.height, mainScreen.size.width);
    NSLog(@" mainScreen.origin.x    = %f  mainScreen.origin.y   = %f  ",mainScreen.origin.x,mainScreen.origin.y);
    CGPoint screenCenter = CGPointMake(mainScreen.size.width/2, mainScreen.size.height/2 + 20);
    self.pointArray = [[NSMutableArray alloc]init];
    
    //上半部分遮挡view
   
    self.appImageView = [[ImageEditView alloc]initWithFrame:CGRectMake(0, 0, mainScreen.size.width, mainScreen.size.width)];
    self.appImageView.bounds =CGRectMake(0, 0, mainScreen.size.width, mainScreen.size.width);
    self.appImageView.center = CGPointMake( screenCenter.x, screenCenter.y - ((mainScreen.size.height - mainScreen.size.width)/4) );
   
    NSLog(@"self.appImageView.frame.size.width  = %f",self.appImageView.frame.size.width);
    NSLog(@"self.appImageView.frame.size.height = %f",self.appImageView.frame.size.height);
    
    self.orgRect = self.appImageView.frame;
    //self.appImageView.backgroundColor = [UIColor clearColor];
    
    UIImage *imageInit =  [UIImage imageWithColor:self.appImageView.backgroundColor andRect:CGRectMake(0, 0, self.appImageView.frame.size.width, self.appImageView.frame.size.width)]; // [self.appImageView image];
    self.imgWindowSize = CGSizeMake(self.appImageView.frame.size.width, self.appImageView.frame.size.height);
    self.orgTrf = self.appImageView.transform;
    //[self.b2opcv setCalculateImage:imageInit andWindowSize:self.imgWindowSize];
    
    [self creatPan];
    //生成一个遮挡平面，这样可以得到小图的剪切
    //float tmpHeight =   self.showImgView.frame.origin.y;
    float tmpHeight =   self.appImageView.frame.origin.y;
    UIImageView *upKeepOutView = [[UIImageView alloc]initWithFrame:CGRectMake(0, 0, mainScreen.size.width, tmpHeight)];
    upKeepOutView.backgroundColor = [UIColor grayColor];
    //[upKeepOutView.layer setCornerRadius:5];
    UIImageView *bottomKeepOutView = [[UIImageView alloc]initWithFrame:CGRectMake(0, tmpHeight + self.appImageView.frame.size.height, mainScreen.size.width, mainScreen.size.height - tmpHeight - self.appImageView.frame.size.height + 20)];
    bottomKeepOutView.backgroundColor = [UIColor grayColor];
    //[bottomKeepOutView.layer setCornerRadius:5];
    
    //打开相册按键
    self.openPhotoAlbum = [UIButton buttonWithType:UIButtonTypeCustom];
    self.openPhotoAlbum.frame = CGRectMake(mainScreen.size.width - 100, mainScreen.size.height - 30, 100, 50);
    self.openPhotoAlbum.backgroundColor = [UIColor whiteColor];
    [self.openPhotoAlbum.layer setCornerRadius:5];
    [self.openPhotoAlbum setTitle:@"打开相册" forState:UIControlStateNormal];
    [self.openPhotoAlbum setTitleColor:[UIColor blackColor] forState:UIControlStateNormal];
    [self.openPhotoAlbum setTitleColor:[UIColor grayColor] forState:UIControlStateHighlighted];
    [self.openPhotoAlbum addTarget:self action:@selector(takePictureClick:) forControlEvents:UIControlEventTouchUpInside];
    //测试用按键，图片位置复位
    self.sysTestButton = [UIButton buttonWithType:UIButtonTypeCustom];
    self.sysTestButton.frame =CGRectMake(0, mainScreen.size.height - 30, 100, 50);
    self.sysTestButton.backgroundColor = [UIColor whiteColor];
    [self.sysTestButton.layer setCornerRadius:5];
    [self.sysTestButton setTitle:@"重置位置" forState:UIControlStateNormal];
    [self.sysTestButton setTitleColor:[UIColor blackColor] forState:UIControlStateNormal];
    [self.sysTestButton setTitleColor:[UIColor grayColor] forState:UIControlStateHighlighted];
    [self.sysTestButton addTarget:self action:@selector(resetPosion:) forControlEvents:UIControlEventTouchUpInside];
    /**
     *  重画按键，点击后消除所有当前图像所有Mask
     */
    self.resetDrawButton = [UIButton buttonWithType:UIButtonTypeCustom];
    self.resetDrawButton.frame = CGRectMake((mainScreen.size.width/2) - 50, mainScreen.size.height - 30, 100, 50);
    self.resetDrawButton.backgroundColor = [UIColor whiteColor];
    [self.resetDrawButton.layer setCornerRadius:5];
    [self.resetDrawButton setTitle:@"重置绘制" forState:UIControlStateNormal];
    [self.resetDrawButton setTitleColor:[UIColor blackColor] forState:UIControlStateNormal];
    [self.resetDrawButton setTitleColor:[UIColor grayColor] forState:UIControlStateHighlighted];
    [self.resetDrawButton addTarget:self action:@selector(resetDraw:) forControlEvents:UIControlEventTouchUpInside];
    
    //添加计算点按键
    self.addCalculatePoint = [UIButton buttonWithType:UIButtonTypeCustom];
    self.addCalculatePoint.frame = CGRectMake(0, (screenCenter.y - ((mainScreen.size.height - mainScreen.size.width)/4) - (mainScreen.size.width/2) ) + mainScreen.size.width , smallButtonWidth, 50);
    self.addCalculatePoint.center = CGPointMake(mainScreen.size.width/5,self.addCalculatePoint.center.y);
    self.addCalculatePoint.backgroundColor = [UIColor yellowColor];
    [self.addCalculatePoint.layer setCornerRadius:5];
    [self.addCalculatePoint setTitle:@"选取" forState:UIControlStateNormal];
    [self.addCalculatePoint setTitleColor:[UIColor blackColor] forState:UIControlStateNormal];
    [self.addCalculatePoint setTitleColor:[UIColor grayColor] forState:UIControlStateHighlighted];
    [self.addCalculatePoint addTarget:self action:@selector(cutImageCut:) forControlEvents:UIControlEventTouchUpInside];
    //添加移动图片按键
    self.moveImg= [UIButton buttonWithType:UIButtonTypeCustom];
    self.moveImg.frame = CGRectMake(mainScreen.size.width - smallButtonWidth, (screenCenter.y - ((mainScreen.size.height - mainScreen.size.width)/4) - (mainScreen.size.width/2) ) + mainScreen.size.width, smallButtonWidth , 50);
    self.moveImg.center = CGPointMake(mainScreen.size.width/5*4,self.moveImg.center.y);
    self.moveImg.backgroundColor = [UIColor whiteColor];
    [self.moveImg.layer setCornerRadius:5];
    [self.moveImg setTitle:@"移动" forState:UIControlStateNormal];
    [self.moveImg setTitleColor:[UIColor blackColor] forState:UIControlStateNormal];
    [self.moveImg setTitleColor:[UIColor grayColor] forState:UIControlStateHighlighted];
    [self.moveImg addTarget:self action:@selector(enableMoveImg:) forControlEvents:UIControlEventTouchUpInside];
    //添加单独添加Mask按键
    self.addMaskPoint = [UIButton buttonWithType:UIButtonTypeCustom];
    self.addMaskPoint.frame = CGRectMake(0, (screenCenter.y - ((mainScreen.size.height - mainScreen.size.width)/4) - (mainScreen.size.width/2) ) + mainScreen.size.width, smallButtonWidth, 50);
    self.addMaskPoint.center = CGPointMake(mainScreen.size.width/5*2, self.addMaskPoint.center.y);
    self.addMaskPoint.backgroundColor = [UIColor whiteColor];
    [self.addMaskPoint.layer setCornerRadius:5];
    [self.addMaskPoint setTitle:@"添加" forState:UIControlStateNormal];
    [self.addMaskPoint setTitleColor:[UIColor blackColor] forState:UIControlStateNormal];
    [self.addMaskPoint setTitleColor:[UIColor grayColor] forState:UIControlStateHighlighted];
    [self.addMaskPoint addTarget:self action:@selector(addMaskPointFoo:) forControlEvents:UIControlEventTouchUpInside];
    //添加删除Mark的按键
    self.deleteMaskPoint = [UIButton buttonWithType:UIButtonTypeCustom];
    self.deleteMaskPoint.frame = CGRectMake(0, (screenCenter.y - ((mainScreen.size.height - mainScreen.size.width)/4) - (mainScreen.size.width/2) ) + mainScreen.size.width, smallButtonWidth, 50);
    self.deleteMaskPoint.center = CGPointMake(mainScreen.size.width/5*3, self.deleteMaskPoint.center.y);
    self.deleteMaskPoint.backgroundColor = [UIColor whiteColor];
    [self.deleteMaskPoint.layer setCornerRadius:5];
    [self.deleteMaskPoint setTitle:@"删除" forState:UIControlStateNormal];
    [self.deleteMaskPoint setTitleColor:[UIColor blackColor] forState:UIControlStateNormal];
    [self.deleteMaskPoint setTitleColor:[UIColor grayColor] forState:UIControlStateHighlighted];
    [self.deleteMaskPoint addTarget:self action:@selector(deleteMaskPointFoo:) forControlEvents:UIControlEventTouchUpInside];
    /**
     *  增加回退按钮
     */
    self.redoButton = [UIButton buttonWithType:UIButtonTypeCustom];
    self.redoButton.frame = CGRectMake(0, (screenCenter.y - ((mainScreen.size.height - mainScreen.size.width)/4) - (mainScreen.size.width/2) ) + mainScreen.size.width , smallButtonWidth, 50);
    self.redoButton.center = CGPointMake(mainScreen.size.width/5,self.addCalculatePoint.center.y + 60);
    self.redoButton.backgroundColor = [UIColor whiteColor];
    [self.redoButton.layer setCornerRadius:5];
    [self.redoButton  setTitle:@"返回" forState:UIControlStateNormal];
    [self.redoButton  setTitleColor:[UIColor blackColor] forState:UIControlStateNormal];
    [self.redoButton  setTitleColor:[UIColor grayColor] forState:UIControlStateHighlighted];
    [self.redoButton  addTarget:self action:@selector(redoButtonFoo) forControlEvents:UIControlEventTouchUpInside];
    /**
     *  增加前进按钮
     */
    self.undoButton = [UIButton buttonWithType:UIButtonTypeCustom];
    self.undoButton.frame = CGRectMake(mainScreen.size.width - smallButtonWidth, (screenCenter.y - ((mainScreen.size.height - mainScreen.size.width)/4) - (mainScreen.size.width/2) ) + mainScreen.size.width, smallButtonWidth , 50);
    self.undoButton.center = CGPointMake(mainScreen.size.width/5*4,self.moveImg.center.y + 60);
    self.undoButton.backgroundColor = [UIColor whiteColor];
    [self.undoButton.layer setCornerRadius:5];
    [self.undoButton setTitle:@"前进" forState:UIControlStateNormal];
    [self.undoButton setTitleColor:[UIColor blackColor] forState:UIControlStateNormal];
    [self.undoButton setTitleColor:[UIColor grayColor] forState:UIControlStateHighlighted];
    [self.undoButton addTarget:self action:@selector(undoButtonFoo) forControlEvents:UIControlEventTouchUpInside];
    
    self.nextStep = [UIButton buttonWithType:UIButtonTypeCustom];
    self.nextStep.frame = CGRectMake( mainScreen.size.width - 100, self.orgRect.origin.y - 50, 100, 50);
    self.nextStep.backgroundColor = [UIColor clearColor];
    [self.nextStep.layer setCornerRadius:5];
    [self.nextStep setTitle:@"下一步 >" forState:UIControlStateNormal];
    [self.nextStep setTitleColor:[UIColor blackColor] forState:UIControlStateNormal];
    [self.nextStep setTitleColor:[UIColor grayColor] forState:UIControlStateHighlighted];
    [self.nextStep addTarget:self action:@selector(nextStepFoo:) forControlEvents:UIControlEventTouchUpInside];
   
    //[self.view addSubview:self.showImgView];
    [self.view addSubview:self.appImageView];
    [self.view addSubview:upKeepOutView];
    [self.view addSubview:bottomKeepOutView];
    [self.view addSubview:self.openPhotoAlbum];
    [self.view addSubview:self.sysTestButton];
    [self.view addSubview:self.addCalculatePoint];
    [self.view addSubview:self.addMaskPoint];
    [self.view addSubview:self.deleteMaskPoint];
    [self.view addSubview:self.moveImg];
    [self.view addSubview:self.resetDrawButton];
    [self.view addSubview:self.redoButton];
    [self.view addSubview:self.undoButton];
    [self.view addSubview:self.nextStep];
    
    self.view.backgroundColor = [UIColor grayColor];
    
    self.isMove = NO;
    self.isDraw = NO;       //默认是添加
    self.isDelete  = NO;
    
    /**
     *  得到最终的剪切图的线程建立
     *
     *  @param "com.clover.cutImageIOS" <#"com.clover.cutImageIOS" description#>
     *  @param NULL                     <#NULL description#>
     *
     *  @return <#return value description#>
     */
    self.getFinnalImageQueue = dispatch_queue_create("com.clover.cutImageIOS", NULL);
    
    // Do any additional setup after loading the view, typically from a nib.
}

-(void) resultImageReady:(UIImage *)sendImage
{
    dispatch_async(dispatch_get_global_queue(0,0), ^{
        //耗时处理
        dispatch_async( dispatch_get_main_queue(), ^{
            //同步显示
            [self.appImageView setPicture:sendImage];
        });
    });
}
/**
 *  将试图恢复到初始位置,并且添加动画
 *
 *  @param sender 按键sender
 */
-(void) resetPosion:(id)sender
{
    [UIView animateWithDuration:.5 animations:^{
        self.appImageView.transform = CGAffineTransformMake(1, 0.0, 0.0, 1, 0, 0);
        self.appImageView.frame = self.orgRect;
    }];
}
/**
 *  重置所有绘制
 *
 *  @param sender <#sender description#>
 */
-(void) resetDraw:(id)sender
{
    self.appImageView.transform =CGAffineTransformScale(self.orgTrf, 1, 1);
    self.appImageView.frame = self.orgRect;
    self.setLineWidth = DEFLINEWIDTH;
    [self.appImageView resetAllMask];
}
/**
 *  回退操作
 */
-(void) redoButtonFoo
{
    [self.appImageView redo];
}
/**
 *  前进操作
 */
-(void) undoButtonFoo
{
    [self.appImageView undo];
}

-(void) cutImageCut:(id)sender
{
    [self.appImageView setMove:NO];
    [self.appImageView setUserInteractionEnabled:YES];
    self.appImageView.isDraw = NO;
    self.appImageView.isDelete = NO;
    self.addCalculatePoint.backgroundColor  = [UIColor yellowColor];
    self.addMaskPoint.backgroundColor  = [UIColor whiteColor];
    self.deleteMaskPoint.backgroundColor  = [UIColor whiteColor];
    self.moveImg.backgroundColor  = [UIColor whiteColor];
    self.isDraw = NO;
    self.isMove = NO;
    self.isDelete= NO;
    [self.appImageView removeGestureRecognizer:self.panGestureRecognizer];
    [self.appImageView removeGestureRecognizer:self.pinchGestureRecognizer];
}

-(void) addMaskPointFoo:(id)sender          //直接添加种子点
{
    [self.appImageView setMove:NO];
    [self.appImageView setUserInteractionEnabled:YES];
    self.appImageView.isDraw = YES;
    self.appImageView.isDelete = NO;
    self.addCalculatePoint.backgroundColor  = [UIColor whiteColor];
    self.addMaskPoint.backgroundColor       = [UIColor yellowColor];
    self.deleteMaskPoint.backgroundColor    = [UIColor whiteColor];
    self.moveImg.backgroundColor            = [UIColor whiteColor];
    self.isDraw = YES;
    self.isMove = NO;
    self.isDelete= NO;
    [self.appImageView removeGestureRecognizer:self.panGestureRecognizer];
    [self.appImageView removeGestureRecognizer:self.pinchGestureRecognizer];
}

-(void) deleteMaskPointFoo:(id)sender
{
    [self.appImageView setMove:NO];
    [self.appImageView setUserInteractionEnabled:YES];
    self.appImageView.isDraw = NO;
    self.appImageView.isDelete = YES;
    self.addCalculatePoint.backgroundColor  = [UIColor whiteColor];
    self.addMaskPoint.backgroundColor       = [UIColor whiteColor];
    self.deleteMaskPoint.backgroundColor    = [UIColor yellowColor];
    self.moveImg.backgroundColor            = [UIColor whiteColor];
    self.isDelete = YES;
    self.isMove = NO;
    [self.appImageView removeGestureRecognizer:self.panGestureRecognizer];
    [self.appImageView removeGestureRecognizer:self.pinchGestureRecognizer];
}

-(void) enableMoveImg:(id)sender
{
    [self.appImageView setMove:YES];
    [self.appImageView setUserInteractionEnabled:YES];
    self.addCalculatePoint.backgroundColor  = [UIColor whiteColor];
    self.addMaskPoint.backgroundColor       = [UIColor whiteColor];
    self.deleteMaskPoint.backgroundColor    = [UIColor whiteColor];
    self.moveImg.backgroundColor            = [UIColor yellowColor];
    self.isMove = YES;
   
    [self.appImageView addGestureRecognizer:self.panGestureRecognizer];
    [self.appImageView addGestureRecognizer:self.pinchGestureRecognizer];
}

-(void) nextStepFoo:(id) sender
{
    //RotateCutImageViewController *aa = [[RotateCutImageViewController alloc]init];
    dispatch_async(self.getFinnalImageQueue, ^{
        UIImage *setImage = [self.appImageView getReusltImage];
        [self.rotateCutImageViewController setImageRect:self.orgRect andImage:setImage];
    });
    
    [self presentViewController:self.rotateCutImageViewController animated:YES completion:nil];
}

/**
 *  开打相册功能函数
 *
 *  @param sender <#sender description#>
 */
-(void)takePictureClick:(id)sender
{
    /*注：使用，需要实现以下协议：UIImagePickerControllerDelegate,
     UINavigationControllerDelegate
     */
    UIImagePickerController *picker = [[UIImagePickerController alloc]init];
    //设置图片源(相簿)
    picker.sourceType = UIImagePickerControllerSourceTypeSavedPhotosAlbum;
    //设置代理
    picker.delegate = self;
    //设置可以编辑
    picker.allowsEditing = YES;
    //打开拾取器界面
    [self presentViewController:picker animated:YES completion:nil];
}

//完成选择图片
-(void)imagePickerController:(UIImagePickerController *)picker  didFinishPickingMediaWithInfo:(NSDictionary *)info
{
    //加载图片
    UIImage *image = [info objectForKey:@"UIImagePickerControllerEditedImage"];
    [self.appImageView  setPicture:image];
    //重置绘制线宽
    self.setLineWidth = DEFLINEWIDTH;
    //选择框消失
    [picker dismissViewControllerAnimated:YES completion:nil];
}
//取消选择图片

-(void)imagePickerControllerDidCancel:(UIImagePickerController *)picker
{
    [picker dismissViewControllerAnimated:YES completion:nil];
}
/**
 *  初始化手势动作，其中不添加
 */
-(void) creatPan
{
    self.panGestureRecognizer = [[UIPanGestureRecognizer alloc]
                                                    initWithTarget:self
                                                    action:@selector(handlePan:)];
    self.panGestureRecognizer.delegate = self;
    self.pinchGestureRecognizer = [[UIPinchGestureRecognizer alloc]
                                                        initWithTarget:self
                                                        action:@selector(handlePinch:)];
    self.pinchGestureRecognizer.delegate = self;
}

- (void) handlePan:(UIPanGestureRecognizer*) recognizer
{
    if(self.isMove){
        if (recognizer.state==UIGestureRecognizerStateChanged) {
            CGPoint translation = [recognizer translationInView:self.view];
            recognizer.view.center = CGPointMake(recognizer.view.center.x + translation.x,
                                                 recognizer.view.center.y + translation.y);
            [recognizer setTranslation:CGPointZero inView:self.view];
        }
        else if(recognizer.state == UIGestureRecognizerStateEnded){
            CGAffineTransform show =  recognizer.view.transform;
            CGAffineTransform show2 =  self.orgTrf;
        }
    }
}

- (void) handlePinch:(UIPinchGestureRecognizer*) recognizer
{
    if(self.isMove){
        if (recognizer.state==UIGestureRecognizerStateChanged) {
            recognizer.view.transform=CGAffineTransformScale(recognizer.view.transform, recognizer.scale, recognizer.scale);
            recognizer.scale = 1; //不重置为1则变化会太快
        }
        else if(recognizer.state == UIGestureRecognizerStateEnded){
            if(recognizer.view.transform.a > 2.0){
                [UIView animateWithDuration:.25 animations:^{
                    recognizer.view.transform= CGAffineTransformMake(2.0, 0.0, 0.0, 2.0, 0, 0) ;//取消一切形变
                }];
            }
            else if( recognizer.view.transform.a < 0.6 ){
                [UIView animateWithDuration:.25 animations:^{
                    recognizer.view.transform= CGAffineTransformMake(0.6, 0.0, 0.0, 0.6, 0, 0) ;//取消一切形变
                }];
            }
            
            [self.appImageView setLineScale:recognizer.view.transform.a];
            NSLog(@"view.transform.a = %f",recognizer.view.transform.a);
            CGAffineTransform show =  recognizer.view.transform;
            CGAffineTransform show2 =  self.orgTrf;
        }
    }
}

- (void)didReceiveMemoryWarning {
    [super didReceiveMemoryWarning];
    // Dispose of any resources that can be recreated.
}

-(void) addPoint2Array:(CGPoint)aPoint
{
    if( aPoint.x >= 0 && aPoint.x < self.orgRect.size.width && aPoint.y >= 0 && aPoint.y < self.orgRect.size.height ){
    [self.pointArray addObject: [NSValue valueWithCGPoint:aPoint]];
    }
}

@end
