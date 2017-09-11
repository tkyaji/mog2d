#import "MogAppDelegate.h"
#import "MogViewController.h"

@interface MogAppDelegate ()
@end

@implementation MogAppDelegate {
    MogViewController *_mogViewController;
}

- (instancetype)init {
    if (!(self = [super init])) return self;
    
    return self;
}

- (void)applicationDidFinishLaunching:(NSNotification *)aNotification {
    /*
    CGRect frame = CGRectMake(0, 0, 500, 400);
    NSWindow *window = [[NSWindow alloc] initWithContentRect:frame
                                                   styleMask:NSWindowStyleMaskClosable
                                                     backing:NSBackingStoreBuffered defer:YES];
    _mogViewController = [[MogViewController alloc] initWithFrame:frame];
    
    window.contentViewController = _mogViewController;
    [window makeKeyAndOrderFront:self];
     */
}


- (void)applicationWillTerminate:(NSNotification *)aNotification {
    // Insert code here to tear down your application
}


@end
