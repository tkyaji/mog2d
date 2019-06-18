#import "MogViewController.h"
#import "MogView.h"
#import "MogEngineController.h"

@interface MogViewController ()
@end


@implementation MogViewController {
    MogEngineController *_engineController;
    NSMutableDictionary<NSNumber *, NSNumber *> *_touchIdDict;
    unsigned int _nextTouchId;
}

- (instancetype)init {
    self = [super init];
    if (!self) return self;
    
    MogView *mogView = [[MogView alloc] initWithFrame:[[UIScreen mainScreen] bounds]];
    self.view = mogView;
    _engineController = [[MogEngineController alloc] initWithMog:self view:mogView];
    mogView.touchEventDelegate = _engineController;
    _touchIdDict = [NSMutableDictionary<NSNumber *, NSNumber *> new];
    _nextTouchId = 1;
    
    return self;
}

- (void)didReceiveMemoryWarning {
    [super didReceiveMemoryWarning];
}

- (void)applicationWillResignActive:(UIApplication *)application {
    [_engineController stopEngine];
}

- (void)applicationDidBecomeActive:(UIApplication *)application {
    [_engineController startEngine];
}

- (void)applicationWillTerminate:(UIApplication *)application {
    [_engineController terminateEngine];
}

- (void)applicationDidReceiveMemoryWarning:(UIApplication *)application {
    [_engineController didReceiveMemoryWarning];
}

@end
