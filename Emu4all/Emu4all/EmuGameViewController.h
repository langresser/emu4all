//
//  SettingView.h
//  MD
//
//  Created by 王 佳 on 12-8-20.
//  Copyright (c) 2012年 Gingco.Net New Media GmbH. All rights reserved.
//
#pragma once
#import "iosUtil.h"
#import "SettingViewController.h"
#import "GameListViewController.h"
#import "EmuControlView.h"
#import "EAGLView.h"

@interface MDGameViewController : UIViewController
{
    GameListViewController* gameListVC;
    SettingViewController* settingVC;
    
#ifdef USE_IOS_JOYSTICK
    EmuControllerView* controlView;
    int currentOrientation;
#endif
    
    EAGLView* glView;
}

-(void)showGameList;
-(void)showSettingPopup;
@end

