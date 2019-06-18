#import <Foundation/Foundation.h>
#import <Cocoa/Cocoa.h>
#import <OpenGL/OpenGL.h>
#import "MogTouchEventDelegate.h"
#import "mog/core/opengl.h"

@interface MogView : NSOpenGLView

@property (nonatomic) NSOpenGLContext* glContext;
@property (nonatomic) GLuint frameBuffer;
@property (nonatomic) GLuint colorBuffer;
//@property (nonatomic) GLuint depthBuffer;
@property (nonatomic) GLint glWidth;
@property (nonatomic) GLint glHeight;
@property (nonatomic) NSObject<MogTouchEventDelegate> *touchEventDelegate;

@end
