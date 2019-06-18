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

- (void)viewDidLoad {
    _mogView = (MogView *)self.view;
    _engineController = [[MogEngineController alloc] initWithMog:self view:_mogView];
    _mogView.touchEventDelegate = _engineController;
    [_engineController startEngine];
}

@end
