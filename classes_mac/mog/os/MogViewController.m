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

- (void)viewDidLoad {
    [super viewDidLoad];
    
    CGRect frame = CGRectMake(0, 0, self.parentView.frame.size.width, self.parentView.frame.size.height);
    MogView *mogView = [[MogView alloc] initWithFrame:frame];
    [mogView setTranslatesAutoresizingMaskIntoConstraints:YES];
    [mogView setAutoresizingMask:NSViewWidthSizable|NSViewHeightSizable];
    [self.parentView addSubview:mogView];
    
    _engineController = [[MogEngineController alloc] initWithMog:self view:mogView];
    mogView.touchEventDelegate = _engineController;
    [_engineController initEngine];
    [_engineController startEngine];
    _touchIdDict = [NSMutableDictionary<NSNumber *, NSNumber *> new];
    _nextTouchId = 1;
}

@end
