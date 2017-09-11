#import <Foundation/Foundation.h>

typedef void (^mog_callback)(void *, int);

@interface MogFunction : NSObject

@property (nonatomic) mog_callback callback;

- (instancetype)init __attribute__((unavailable("init is not available")));
- (instancetype)initWithCallback:(mog_callback)callback;
- (void)invoke;

@end
