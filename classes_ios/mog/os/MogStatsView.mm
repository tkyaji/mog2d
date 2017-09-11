#import <Foundation/Foundation.h>
#import <UIKit/UIKit.h>
#import "MogStatsView.h"
#import "NativeClass.h"
#import "mog/os/IOSHelper.h"

using namespace mog;

bool MogStatsView::enabled = false;

CGRect __alignmentToFrame(Alignment alignment, CGSize viewSize) {
    CGFloat x = 0;
    CGFloat y = 0;
    CGRect screenBounds = [UIScreen mainScreen].bounds;
    
    switch (alignment) {
        case Alignment::MiddleLeft:
        case Alignment::MiddleCenter:
        case Alignment::MiddleRight:
            y = (screenBounds.size.height - viewSize.height) * 0.5;
            break;
            
        case Alignment::BottomLeft:
        case Alignment::BottomCenter:
        case Alignment::BottomRight:
            y = screenBounds.size.height - viewSize.height;
            break;
            
        default:
            break;
    }
    
    switch (alignment) {
        case Alignment::TopCenter:
        case Alignment::MiddleCenter:
        case Alignment::BottomCenter:
            x = (screenBounds.size.width - viewSize.width) * 0.5;
            break;
            
        case Alignment::TopRight:
        case Alignment::MiddleRight:
        case Alignment::BottomRight:
            x = screenBounds.size.width - viewSize.width;
            break;
            
        default:
            break;
    }
    
    return CGRectMake(x, y, viewSize.width, viewSize.height);
}

bool MogStatsView::isEnabled() {
    return MogStatsView::enabled;
}

void MogStatsView::setStats(float delta, int drawCall, int instants) {
    if (!MogStatsView::enabled) return;
    UITextView *textView = (__bridge UITextView *)Engine::getInstance()->getNativeObject("STATS_VIEW")->getObject();
    textView.text = [NSString stringWithFormat:@" %.3lf / %3lf\n DRAW CALL : %d\n INSTANTS  : %d", 1/delta, delta, drawCall, instants];
}

void MogStatsView::setEnable(bool enable) {
    if (MogStatsView::enabled == enable) return;
    if (enable) {
        MogStatsView::initStatsView();
    } else {
        UITextView *textView = (__bridge UITextView *)Engine::getInstance()->getNativeObject("STATS_VIEW")->getObject();
        [textView removeFromSuperview];
        Engine::getInstance()->removeNativeObject("STATS_VIEW");
    }
    MogStatsView::enabled = enable;
}

void MogStatsView::setAlignment(Alignment alignment) {
    if (!MogStatsView::enabled) return;
    UITextView *textView = (__bridge UITextView *)Engine::getInstance()->getNativeObject("STATS_VIEW")->getObject();
    [textView setFrame:__alignmentToFrame(alignment, textView.frame.size)];
}

void MogStatsView::initStatsView() {
    UITextView *textView = [[UITextView alloc] initWithFrame:__alignmentToFrame(Alignment::BottomLeft, CGSizeMake(155, 50))];
    textView.backgroundColor = [UIColor colorWithRed:0 green:0 blue:0 alpha:0.5f];
    textView.font = [UIFont fontWithName:@"Courier" size:12];
    textView.textColor = [UIColor colorWithRed:1 green:1 blue:1 alpha:0.7f];
    textView.editable = NO;
    textView.selectable = NO;
    textView.userInteractionEnabled = NO;
    
    UIView *view = IOSHelper::getUIView();
    [view addSubview:textView];
    Engine::getInstance()->setNativeObject("STATS_VIEW", NativeObject::create((__bridge void *)textView));
}
