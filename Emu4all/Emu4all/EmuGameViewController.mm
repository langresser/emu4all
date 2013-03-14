//
//  SettingView.m
//  MD
//
//  Created by 王 佳 on 12-8-20.
//  Copyright (c) 2012年 Gingco.Net New Media GmbH. All rights reserved.
//

#import "EmuGameViewController.h"
#import "EAGLView.h"
#include "EmuSystem.hh"
#import "UIDevice+Util.h"

@implementation MDGameViewController
-(void)loadView
{
    CGRect rect = [[UIScreen mainScreen] bounds];
#ifdef USE_IOS_JOYSTICK
    UIView* view = [[UIView alloc]initWithFrame:rect];
    self.view = view;

    controlView = [[EmuControllerView alloc]initWithFrame:rect];
    [controlView addEmuWindow:glView];
#endif
    
    UIView* view = [[UIView alloc]initWithFrame:rect];
    self.view = view;
    
    glView = [[EAGLView alloc]initWithFrame:rect];
    glView.transform = CGAffineTransformConcat(glView.transform, CGAffineTransformMakeRotation(- 3.1415 / 2));
    glView.transform = CGAffineTransformConcat(glView.transform, CGAffineTransformMakeTranslation((rect.size.height - rect.size.width) / 2, (rect.size.width - rect.size.height) / 2));
    [self.view addSubview:glView];
}

-(void)viewDidLoad
{
    [super viewDidLoad];
    
#ifdef USE_IOS_JOYSTICK
    currentOrientation = UIInterfaceOrientationLandscapeLeft;
    CGSize size = [UIScreen mainScreen].bounds.size;
    int width = size.width > size.height ? size.width : size.height;
    int height = size.width > size.height ? size.height : size.width;
    controlView.frame = CGRectMake(0, 0, width, height);
    [controlView changeUI:currentOrientation];
#endif
    
    [[NSNotificationCenter defaultCenter] addObserver:self selector:@selector(showSettingPopup) name:@"showsetting" object:nil];
    [[NSNotificationCenter defaultCenter] addObserver:self selector:@selector(appActivatedDidFinish:) name:kDJAppActivateDidFinish object:nil];
}

- (void)appActivatedDidFinish:(NSNotification *)notice;
{
    NSDictionary* resultDic = [notice object];
    NSLog(@"%@", resultDic);
    NSNumber *result = [resultDic objectForKey:@"result"];
    if ([result boolValue]) {
        NSNumber *awardAmount = [resultDic objectForKey:@"awardAmount"];
        NSString *identifier = [resultDic objectForKey:@"identifier"];
        NSLog(@"app identifier = %@", identifier);
        
        extern int g_currentMB;
        g_currentMB += [awardAmount floatValue];
        [gameListVC updateMBInfo];
    }
}

static uint iOSInterfaceOrientationToGfx(UIInterfaceOrientation orientation)
{
	switch(orientation)
	{
		case UIInterfaceOrientationPortrait: return Gfx::VIEW_ROTATE_0;
		case UIInterfaceOrientationLandscapeLeft: return Gfx::VIEW_ROTATE_90;
		case UIInterfaceOrientationLandscapeRight: return Gfx::VIEW_ROTATE_270;
		case UIInterfaceOrientationPortraitUpsideDown: return Gfx::VIEW_ROTATE_180;
		default : return 255; // TODO: handle Face-up/down
	}
}

-(void)willRotateToInterfaceOrientation:(UIInterfaceOrientation)toInterfaceOrientation duration:(NSTimeInterval)duration
{
    [super willRotateToInterfaceOrientation:toInterfaceOrientation duration:duration];
    
#ifdef USE_IOS_JOYSTICK
    if (toInterfaceOrientation == UIInterfaceOrientationLandscapeLeft) {
        currentOrientation = UIInterfaceOrientationLandscapeRight;
        
        CGSize size = [UIScreen mainScreen].bounds.size;
        int width = size.width > size.height ? size.width : size.height;
        int height = size.width > size.height ? size.height : size.width;
        controlView.frame = CGRectMake(0, 0, width, height);
        [controlView changeUI:UIInterfaceOrientationLandscapeRight];
    } else if (toInterfaceOrientation == UIInterfaceOrientationLandscapeRight) {
        currentOrientation = UIInterfaceOrientationLandscapeLeft;
        
        CGSize size = [UIScreen mainScreen].bounds.size;
        int width = size.width > size.height ? size.width : size.height;
        int height = size.width > size.height ? size.height : size.width;
        controlView.frame = CGRectMake(0, 0, width, height);
        [controlView changeUI:UIInterfaceOrientationLandscapeLeft];
    }
#endif
    
//    uint o = iOSInterfaceOrientationToGfx(toInterfaceOrientation);
//	Gfx::preferedOrientation = o;
//	Gfx::setOrientation(Gfx::preferedOrientation);
}

- (BOOL)shouldAutorotateToInterfaceOrientation:(UIInterfaceOrientation)interfaceOrientation {
    return UIInterfaceOrientationIsLandscape(interfaceOrientation);
}

- (BOOL)shouldAutorotate
{
    return YES;
}

- (NSUInteger)supportedInterfaceOrientations
{
    return UIInterfaceOrientationMaskAll;
}

-(void)showSettingPopup
{
    if (settingVC == nil) {
        settingVC = [[SettingViewController alloc]initWithNibName:nil bundle:nil];
    }
    
    [self presentModalViewController:settingVC animated:YES];
}

-(void)showGameList
{
    if (gameListVC == nil) {
        gameListVC = [[GameListViewController alloc]init];
    }
    
    [self presentModalViewController:gameListVC animated:NO];
}
@end
