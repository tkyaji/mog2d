#import <Foundation/Foundation.h>
#import <UIKit/UIKit.h>
#import "MogTouchEventDelegate.h"
#import "mog/core/opengl.h"

@interface MogView : UIView

@property (nonatomic) EAGLContext* glContext;
@property (nonatomic) GLuint frameBuffer;
@property (nonatomic) GLuint colorBuffer;
//@property (nonatomic) GLuint depthBuffer;
@property (nonatomic) GLint glWidth;
@property (nonatomic) GLint glHeight;
@property (nonatomic) NSObject<MogTouchEventDelegate> *touchEventDelegate;

@end
