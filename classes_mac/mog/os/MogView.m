#import "MogView.h"
#import <OpenGL/OpenGL.h>

@implementation MogView {
}

- (instancetype)initWithFrame:(NSRect)frame {
    NSOpenGLPixelFormatAttribute attrs[] = {
        NSOpenGLPFAAllowOfflineRenderers,
        NSOpenGLPFAAccelerated,
        NSOpenGLPFADoubleBuffer,
        NSOpenGLPFAColorSize, 32,
        NSOpenGLPFADepthSize, 32,
        NSOpenGLPFAMultisample,
        NSOpenGLPFASampleBuffers, 1,
        NSOpenGLPFASamples, 4,
        NSOpenGLPFANoRecovery,
        NSOpenGLPFAOpenGLProfile, NSOpenGLProfileVersionLegacy,
        0,
    };
    
    NSOpenGLPixelFormat *pixelFormat = [[NSOpenGLPixelFormat alloc] initWithAttributes:attrs];
    self = [super initWithFrame:frame pixelFormat:pixelFormat];
    if (self) {
        [self initGL];
    }
    return self;
}

- (void)prepareOpenGL {
    [super prepareOpenGL];
    
    self.glContext = [self openGLContext];
    
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);
    [self.glContext flushBuffer];
}

- (void)initGL {
    [self setWantsBestResolutionOpenGLSurface:YES];
    self.glContext = self.openGLContext;
    self.scaleFactor = [NSScreen mainScreen].backingScaleFactor;
    
    self.glWidth = self.frame.size.width * self.scaleFactor;
    self.glHeight = self.frame.size.height * self.scaleFactor;
}

- (void)mouseDown:(NSEvent *)event {
    NSNumber *touchId = @1;
    CGPoint location = [self convertPoint:event.locationInWindow fromView:nil];
    location.y = self.frame.size.height - location.y;
    [self.touchEventDelegate fireTouchEvent:touchId.unsignedIntValue location:location action:TouchActionDown];
}

- (void)mouseDragged:(NSEvent *)event {
    NSNumber *touchId = @1;
    CGPoint location = [self convertPoint:event.locationInWindow fromView:nil];
    location.y = self.frame.size.height - location.y;
    [self.touchEventDelegate fireTouchEvent:touchId.unsignedIntValue location:location action:TouchActionMove];
}

- (void)mouseUp:(NSEvent *)event {
    NSNumber *touchId = @1;
    CGPoint location = [self convertPoint:event.locationInWindow fromView:nil];
    location.y = self.frame.size.height - location.y;
    [self.touchEventDelegate fireTouchEvent:touchId.unsignedIntValue location:location action:TouchActionUp];
}

- (void)mouseEntered:(NSEvent *)event {
}

- (void)mouseExited:(NSEvent *)event {
}

@end
