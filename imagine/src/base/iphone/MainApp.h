#define Fixed MacTypes_Fixed
#define Rect MacTypes_Rect
#import <UIKit/UIKit.h>
#undef Fixed
#undef Rect

#include <config.h>
#import "GameListViewController.h"
#import "SettingViewController.h"
#import "EmuGameViewController.h"

#define IPHONE_VKEYBOARD

@interface MainApp : NSObject <UIApplicationDelegate, UIPopoverControllerDelegate, UITextFieldDelegate>
{
    UINavigationController* gameVC;
    GameListViewController* gameListVC;
    SettingViewController* settingVC;
    UIPopoverController * popoverVC;
    
    UIWindow *window;
    
    MDGameViewController* emuGameVC;
}

-(void)showSettingPopup;
-(void)showGameList;

@end

void showSettingPopup();
void showGameList();
