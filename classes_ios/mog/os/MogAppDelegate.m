#import "MogAppDelegate.h"
#import "MogViewController.h"

@interface MogAppDelegate ()
@end

@implementation MogAppDelegate {
    MogViewController *_mogViewController;
    NSMutableArray<NSObject<UIApplicationDelegate> *> *_delegatePlugins;
}

- (instancetype)init {
    if (!(self = [super init])) return self;

    _delegatePlugins = [NSMutableArray new];
    NSBundle* bundle = [NSBundle mainBundle];
    NSString* path = [bundle pathForResource:@"Mog2d-Info" ofType:@"plist"];
    NSDictionary* dict = [NSDictionary dictionaryWithContentsOfFile:path];
    NSArray *plugins =[NSArray arrayWithArray:[dict objectForKey:@"DelegatePlugins"]];
    for (NSString *className in plugins) {
        Class cls = NSClassFromString(className);
        [_delegatePlugins addObject:[cls new]];
    }
    
    return self;
}

- (BOOL)application:(UIApplication *)application didFinishLaunchingWithOptions:(NSDictionary *)launchOptions {
    self.window = [[UIWindow alloc] initWithFrame:[[UIScreen mainScreen] bounds]];
    _mogViewController = [MogViewController new];
    self.window.rootViewController = _mogViewController;
    [self.window makeKeyAndVisible];
    
    for (NSObject<UIApplicationDelegate> *delegatePlugin in _delegatePlugins) {
        if (![delegatePlugin respondsToSelector:@selector(application:didFinishLaunchingWithOptions:)]) continue;
        if (![delegatePlugin application:application didFinishLaunchingWithOptions:launchOptions]) return NO;
    }
    return YES;
}

- (void)applicationDidBecomeActive:(UIApplication *)application {
    [_mogViewController applicationDidBecomeActive:application];
    
    for (NSObject<UIApplicationDelegate> *delegatePlugin in _delegatePlugins) {
        if (![delegatePlugin respondsToSelector:@selector(applicationDidBecomeActive:)]) continue;
        [delegatePlugin applicationDidBecomeActive:application];
    }
}

- (void)applicationWillResignActive:(UIApplication *)application {
    [_mogViewController applicationWillResignActive:application];
    
    for (NSObject<UIApplicationDelegate> *delegatePlugin in _delegatePlugins) {
        if (![delegatePlugin respondsToSelector:@selector(applicationWillResignActive:)]) continue;
        [delegatePlugin applicationWillResignActive:application];
    }
}

- (void)applicationWillTerminate:(UIApplication *)application {
    [_mogViewController applicationWillTerminate:application];
    
    for (NSObject<UIApplicationDelegate> *delegatePlugin in _delegatePlugins) {
        if (![delegatePlugin respondsToSelector:@selector(applicationWillTerminate:)]) continue;
        [delegatePlugin applicationWillTerminate:application];
    }
}

- (void)applicationDidReceiveMemoryWarning:(UIApplication *)application {
    [_mogViewController applicationDidReceiveMemoryWarning:application];
    
    for (NSObject<UIApplicationDelegate> *delegatePlugin in _delegatePlugins) {
        if (![delegatePlugin respondsToSelector:@selector(applicationDidReceiveMemoryWarning:)]) continue;
        [delegatePlugin applicationDidReceiveMemoryWarning:application];
    }
}



- (BOOL)application:(UIApplication *)application willFinishLaunchingWithOptions:(NSDictionary *)launchOptions  {
    for (NSObject<UIApplicationDelegate> *delegatePlugin in _delegatePlugins) {
        if (![delegatePlugin respondsToSelector:@selector(application:willFinishLaunchingWithOptions:)]) continue;
        if (![delegatePlugin application:application willFinishLaunchingWithOptions:launchOptions]) return NO;
    }
    return YES;
}

- (BOOL)application:(UIApplication *)app openURL:(NSURL *)url options:(NSDictionary<UIApplicationOpenURLOptionsKey, id> *)options {
    for (NSObject<UIApplicationDelegate> *delegatePlugin in _delegatePlugins) {
        if (![delegatePlugin respondsToSelector:@selector(application:openURL:options:)]) continue;
        if (![delegatePlugin application:app openURL:url options:options]) return NO;
    }
    return YES;
}

- (void)applicationSignificantTimeChange:(UIApplication *)application {
    for (NSObject<UIApplicationDelegate> *delegatePlugin in _delegatePlugins) {
        if (![delegatePlugin respondsToSelector:@selector(applicationSignificantTimeChange:)]) continue;
        [delegatePlugin applicationSignificantTimeChange:application];
    }
}

- (void)application:(UIApplication *)application willChangeStatusBarOrientation:(UIInterfaceOrientation)newStatusBarOrientation duration:(NSTimeInterval)duration {
    for (NSObject<UIApplicationDelegate> *delegatePlugin in _delegatePlugins) {
        if (![delegatePlugin respondsToSelector:@selector(application:willChangeStatusBarOrientation:duration:)]) continue;
        [delegatePlugin application:application willChangeStatusBarOrientation:newStatusBarOrientation duration:duration];
    }
}

- (void)application:(UIApplication *)application didChangeStatusBarOrientation:(UIInterfaceOrientation)oldStatusBarOrientation {
    for (NSObject<UIApplicationDelegate> *delegatePlugin in _delegatePlugins) {
        if (![delegatePlugin respondsToSelector:@selector(application:didChangeStatusBarOrientation:)]) continue;
        [delegatePlugin application:application didChangeStatusBarOrientation:oldStatusBarOrientation];
    }
}

- (void)application:(UIApplication *)application willChangeStatusBarFrame:(CGRect)newStatusBarFrame {
    for (NSObject<UIApplicationDelegate> *delegatePlugin in _delegatePlugins) {
        if (![delegatePlugin respondsToSelector:@selector(application:willChangeStatusBarFrame:)]) continue;
        [delegatePlugin application:application willChangeStatusBarFrame:newStatusBarFrame];
    }
}

- (void)application:(UIApplication *)application didChangeStatusBarFrame:(CGRect)oldStatusBarFrame {
    for (NSObject<UIApplicationDelegate> *delegatePlugin in _delegatePlugins) {
        if (![delegatePlugin respondsToSelector:@selector(application:didChangeStatusBarFrame:)]) continue;
        [delegatePlugin application:application didChangeStatusBarFrame:oldStatusBarFrame];
    }
}

- (void)application:(UIApplication *)application didRegisterUserNotificationSettings:(UIUserNotificationSettings *)notificationSettings  {
    for (NSObject<UIApplicationDelegate> *delegatePlugin in _delegatePlugins) {
        if (![delegatePlugin respondsToSelector:@selector(application:didRegisterUserNotificationSettings:)]) continue;
        [delegatePlugin application:application didRegisterUserNotificationSettings:notificationSettings];
    }
}

- (void)application:(UIApplication *)application didRegisterForRemoteNotificationsWithDeviceToken:(NSData *)deviceToken {
    for (NSObject<UIApplicationDelegate> *delegatePlugin in _delegatePlugins) {
        if (![delegatePlugin respondsToSelector:@selector(application:didRegisterForRemoteNotificationsWithDeviceToken:)]) continue;
        [delegatePlugin application:application didRegisterForRemoteNotificationsWithDeviceToken:deviceToken];
    }
}

- (void)application:(UIApplication *)application didFailToRegisterForRemoteNotificationsWithError:(NSError *)error {
    for (NSObject<UIApplicationDelegate> *delegatePlugin in _delegatePlugins) {
        if (![delegatePlugin respondsToSelector:@selector(application:didFailToRegisterForRemoteNotificationsWithError:)]) continue;
        [delegatePlugin application:application didFailToRegisterForRemoteNotificationsWithError:error];
    }
}

- (void)application:(UIApplication *)application didReceiveRemoteNotification:(NSDictionary *)userInfo fetchCompletionHandler:(void (^)(UIBackgroundFetchResult result))completionHandler {
    for (NSObject<UIApplicationDelegate> *delegatePlugin in _delegatePlugins) {
        if (![delegatePlugin respondsToSelector:@selector(application:didReceiveRemoteNotification:fetchCompletionHandler:)]) continue;
        [delegatePlugin application:application didReceiveRemoteNotification:userInfo fetchCompletionHandler:completionHandler];
    }
}

- (void)application:(UIApplication *)application performFetchWithCompletionHandler:(void (^)(UIBackgroundFetchResult result))completionHandler {
    for (NSObject<UIApplicationDelegate> *delegatePlugin in _delegatePlugins) {
        if (![delegatePlugin respondsToSelector:@selector(application:performFetchWithCompletionHandler:)]) continue;
        [delegatePlugin application:application performFetchWithCompletionHandler:completionHandler];
    }
}

- (void)application:(UIApplication *)application performActionForShortcutItem:(UIApplicationShortcutItem *)shortcutItem completionHandler:(void(^)(BOOL succeeded))completionHandler {
    for (NSObject<UIApplicationDelegate> *delegatePlugin in _delegatePlugins) {
        if (![delegatePlugin respondsToSelector:@selector(application:performActionForShortcutItem:completionHandler:)]) continue;
        [delegatePlugin application:application performActionForShortcutItem:shortcutItem completionHandler:completionHandler];
    }
}

- (void)application:(UIApplication *)application handleEventsForBackgroundURLSession:(NSString *)identifier completionHandler:(void (^)())completionHandler {
    for (NSObject<UIApplicationDelegate> *delegatePlugin in _delegatePlugins) {
        if (![delegatePlugin respondsToSelector:@selector(application:handleEventsForBackgroundURLSession:completionHandler:)]) continue;
        [delegatePlugin application:application handleEventsForBackgroundURLSession:identifier completionHandler:completionHandler];
    }
}

- (void)application:(UIApplication *)application handleWatchKitExtensionRequest:(NSDictionary *)userInfo reply:(void(^)(NSDictionary *replyInfo))reply {
    for (NSObject<UIApplicationDelegate> *delegatePlugin in _delegatePlugins) {
        if (![delegatePlugin respondsToSelector:@selector(application:handleWatchKitExtensionRequest:reply:)]) continue;
        [delegatePlugin application:application handleWatchKitExtensionRequest:userInfo reply:reply];
    }
}

- (void)applicationShouldRequestHealthAuthorization:(UIApplication *)application {
    for (NSObject<UIApplicationDelegate> *delegatePlugin in _delegatePlugins) {
        if (![delegatePlugin respondsToSelector:@selector(applicationShouldRequestHealthAuthorization:)]) continue;
        [delegatePlugin applicationShouldRequestHealthAuthorization:application];
    }
}

- (void)applicationDidEnterBackground:(UIApplication *)application {
    for (NSObject<UIApplicationDelegate> *delegatePlugin in _delegatePlugins) {
        if (![delegatePlugin respondsToSelector:@selector(applicationDidEnterBackground:)]) continue;
        [delegatePlugin applicationDidEnterBackground:application];
    }
}

- (void)applicationWillEnterForeground:(UIApplication *)application {
    for (NSObject<UIApplicationDelegate> *delegatePlugin in _delegatePlugins) {
        if (![delegatePlugin respondsToSelector:@selector(applicationWillEnterForeground:)]) continue;
        [delegatePlugin applicationWillEnterForeground:application];
    }
}

- (void)applicationProtectedDataWillBecomeUnavailable:(UIApplication *)application {
    for (NSObject<UIApplicationDelegate> *delegatePlugin in _delegatePlugins) {
        if (![delegatePlugin respondsToSelector:@selector(applicationProtectedDataWillBecomeUnavailable:)]) continue;
        [delegatePlugin applicationProtectedDataWillBecomeUnavailable:application];
    }
}

- (void)applicationProtectedDataDidBecomeAvailable:(UIApplication *)application {
    for (NSObject<UIApplicationDelegate> *delegatePlugin in _delegatePlugins) {
        if (![delegatePlugin respondsToSelector:@selector(applicationProtectedDataDidBecomeAvailable:)]) continue;
        [delegatePlugin applicationProtectedDataDidBecomeAvailable:application];
    }
}

@end
