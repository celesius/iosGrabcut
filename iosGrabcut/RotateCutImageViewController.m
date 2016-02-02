//
//  RotateCutImageViewController.m
//  cutImageIOS
//
//  Created by vk on 15/9/9.
//  Copyright (c) 2015年 Clover. All rights reserved.
//

#import "RotateCutImageViewController.h"
#import "KTOneFingerRotationGestureRecognizer.h"

@interface RotateCutImageViewController ()

@property (nonatomic, strong) UIButton *backStep;
@property (nonatomic, strong) UIButton *saveImageButton;
@property (nonatomic, strong) UIImageView *showImgView;
@property (nonatomic, assign) CGRect showImgViewRect;
@property (nonatomic, strong) UIRotationGestureRecognizer *rotationGestureRecognizer;
@property (nonatomic) CGAffineTransform orgShowImgViewTransform;

@end

@implementation RotateCutImageViewController

- (void)viewDidLoad {
    [super viewDidLoad];
    [self setModalTransitionStyle:UIModalTransitionStyleFlipHorizontal];

    self.view.backgroundColor = [UIColor grayColor];
    [self.view.layer setCornerRadius:5.0];
    
    CGRect mainScreen = [[UIScreen mainScreen] applicationFrame];
    
    self.backStep = [UIButton buttonWithType:UIButtonTypeCustom];
    self.backStep.frame = CGRectMake(mainScreen.origin.x, mainScreen.origin.y, 100, 50);
    self.backStep.backgroundColor = [UIColor clearColor];
    [self.backStep.layer setCornerRadius:5];
    [self.backStep setTitle:@"< 上一步" forState:UIControlStateNormal];
    [self.backStep setTitleColor:[UIColor colorWithRed:25.0/255.0 green:25.0/255.0 blue:112.0/255.0 alpha:1.0] forState:UIControlStateNormal];
    [self.backStep setTitleColor:[UIColor grayColor] forState:UIControlStateHighlighted];
    [self.backStep addTarget:self action:@selector(backStepFoo:) forControlEvents:UIControlEventTouchUpInside];
   
    self.saveImageButton = [UIButton buttonWithType:UIButtonTypeCustom];
    self.saveImageButton.frame = CGRectMake(0, 0, 100, 50);
    self.saveImageButton.center = CGPointMake(mainScreen.size.width/2, mainScreen.size.height - 50);
    self.saveImageButton.backgroundColor = [UIColor whiteColor];
    [self.saveImageButton.layer setCornerRadius:5];
    [self.saveImageButton setTitle:@"保存图像" forState:UIControlStateNormal];
    [self.saveImageButton setTitleColor:[UIColor blackColor] forState:UIControlStateNormal];
    [self.saveImageButton setTitleColor:[UIColor grayColor] forState:UIControlStateHighlighted];
    [self.saveImageButton addTarget:self action:@selector(SaveImageButtonFoo:) forControlEvents:UIControlEventTouchUpInside];
    
    
    self.showImgView = [[UIImageView alloc]init];
    self.orgShowImgViewTransform = self.showImgView.transform;
  //  self.showImgView.backgroundColor = [UIColor greenColor];
    self.showImgView.backgroundColor = [UIColor clearColor];
   
    //[self creatPan];
    [self.showImgView setUserInteractionEnabled:YES];
    [self addRotationGestureToView:self.showImgView];
    [self addTapGestureToView:self.showImgView numberOfTaps:1];
    
    
    [self.view addSubview:self.backStep];
    [self.view addSubview:self.showImgView];
    [self.view addSubview:self.saveImageButton];
    //[self setModalTransitionStyle:UIModalTransitionStylePartialCurl];
    // Do any additional setup after loading the view.
}

- (void)didReceiveMemoryWarning {
    [super didReceiveMemoryWarning];
    // Dispose of any resources that can be recreated.
}

-(void)backStepFoo:(id)sender
{
 //   [self.navigationController popViewControllerAnimated:YES];
    [self dismissViewControllerAnimated:YES completion:nil];
}

-(void) setImageRect:(CGRect) setRect andImage:(UIImage *)setImage;
{
    self.showImgView.frame = setRect;
    dispatch_async( dispatch_get_main_queue() , ^{
        self.showImgView.image = setImage;
    });
}

-(void) viewWillAppear:(BOOL)animated
{
    self.showImgView.transform = self.orgShowImgViewTransform;
}

-(void) creatPan
{
    self.rotationGestureRecognizer  = [[UIRotationGestureRecognizer alloc]
                                                    initWithTarget:self
                                                    action:@selector(handleRotation:)];
    self.rotationGestureRecognizer.delegate = self;
    
    [self.view addGestureRecognizer:self.rotationGestureRecognizer];
}

- (void) handleRotation:(UIPanGestureRecognizer*) recognizer
{
    NSLog(@"rotationGestureRecognizer");
}

- (void)addRotationGestureToView:(UIView *)view
{
   KTOneFingerRotationGestureRecognizer *rotation = [[KTOneFingerRotationGestureRecognizer alloc] initWithTarget:self action:@selector(rotating:)];
   [view addGestureRecognizer:rotation];
//   [rotation release];
}

- (void)addTapGestureToView:(UIView *)view numberOfTaps:(NSInteger)numberOfTaps
{
   UITapGestureRecognizer *tap = [[UITapGestureRecognizer alloc] initWithTarget:self action:@selector(tapped:)];
   [tap setNumberOfTapsRequired:numberOfTaps];
   [view addGestureRecognizer:tap];
//   [tap release];
}
- (BOOL)shouldAutorotateToInterfaceOrientation:(UIInterfaceOrientation)toInterfaceOrientation
{
   return (UI_USER_INTERFACE_IDIOM() == UIUserInterfaceIdiomPad);
}

- (void)rotating:(KTOneFingerRotationGestureRecognizer *)recognizer
{
   UIView *view = [recognizer view];
   [view setTransform:CGAffineTransformRotate([view transform], [recognizer rotation])];
}

- (void)tapped:(UITapGestureRecognizer *)recognizer
{
   UIView *view = [recognizer view];
   [view setTransform:CGAffineTransformMakeRotation(0)];
}


- (void)saveImageToPhotos:(UIImage*)savedImage
{
    UIImageWriteToSavedPhotosAlbum(savedImage, self, @selector(image:didFinishSavingWithError:contextInfo:), NULL);
}
// 指定回调方法
- (void)image: (UIImage *) image didFinishSavingWithError: (NSError *) error contextInfo: (void *) contextInfo
{
    NSString *msg = nil ;
    if(error != NULL){
        msg = @"保存图片失败" ;
    }else{
        msg = @"保存图片成功" ;
    }
    UIAlertView *alert = [[UIAlertView alloc] initWithTitle:@"保存图片结果提示"
                                                    message:msg
                                                   delegate:self
                                          cancelButtonTitle:@"确定"
                                          otherButtonTitles:nil];
    [alert show];
}

-(void)SaveImageButtonFoo:(id)sender
{
    [self saveImageToPhotos:self.showImgView.image];
}

/*
#pragma mark - Navigation

// In a storyboard-based application, you will often want to do a little preparation before navigation
- (void)prepareForSegue:(UIStoryboardSegue *)segue sender:(id)sender {
    // Get the new view controller using [segue destinationViewController].
    // Pass the selected object to the new view controller.
}
*/

@end
