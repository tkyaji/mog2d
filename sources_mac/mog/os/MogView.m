#import "MogView.h"

@implementation MogView {
    NSMutableDictionary<NSNumber *, NSNumber *> *_touchIdDict;
    unsigned int _nextTouchId;
}

- (instancetype)initWithCoder:(NSCoder *)decoder {
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
    self = [super initWithCoder:decoder];
    NSOpenGLContext *context = [[NSOpenGLContext alloc] initWithFormat:pixelFormat shareContext:nil];
    [self setOpenGLContext:context];
    if (self) {
        [self initGL];
    }
    
    [self.window makeFirstResponder:self];
    
    return self;
}

- (instancetype)initWithFrame:(NSRect)frameRect {
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
    self = [super initWithFrame:frameRect];
    NSOpenGLContext *context = [[NSOpenGLContext alloc] initWithFormat:pixelFormat shareContext:nil];
    [self setOpenGLContext:context];
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
    float scaleFactor = [NSScreen mainScreen].backingScaleFactor;
    self.layer.contentsScale = scaleFactor;
    
    glGenFramebuffers(1, &_frameBuffer);
    glBindFramebuffer(GL_FRAMEBUFFER, self.frameBuffer);
    
    glGenRenderbuffers(1, &_colorBuffer);
    glBindRenderbuffer(GL_RENDERBUFFER, self.colorBuffer);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_RGBA8, _glWidth, _glHeight);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_RENDERBUFFER, self.colorBuffer);
    
    /*
     glGenRenderbuffers(1, &_depthBuffer);
     glBindRenderbuffer(GL_RENDERBUFFER, self.depthBuffer);
     glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8_OES, _glWidth, _glHeight);
     glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, self.depthBuffer);
     glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_STENCIL_ATTACHMENT, GL_RENDERBUFFER, self.depthBuffer);
     */
    
    glGetRenderbufferParameteriv(GL_RENDERBUFFER, GL_RENDERBUFFER_WIDTH, &_glWidth);
    glGetRenderbufferParameteriv(GL_RENDERBUFFER, GL_RENDERBUFFER_HEIGHT, &_glHeight);
    
    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
        NSLog(@"framebuffer is invalid.");
    }
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
