#import "MogView.h"

@implementation MogView {
    NSMutableDictionary<NSNumber *, NSNumber *> *_touchIdDict;
    unsigned int _nextTouchId;
}

+ (Class)layerClass {
    return [CAEAGLLayer class];
}

- (void)dealloc {
    if (self.frameBuffer > 0) {
        glDeleteFramebuffers(1, &_frameBuffer);
    }
    if (self.colorBuffer > 0) {
        glDeleteFramebuffers(1, &_colorBuffer);
    }
    /*
    if (self.depthBuffer > 0) {
        glDeleteFramebuffers(1, &_depthBuffer);
    }
     */
}

- (instancetype)initWithFrame:(CGRect)frame {
    self = [super initWithFrame:frame];
    if (!self) return self;
    
    _nextTouchId = 1;
    _touchIdDict = [NSMutableDictionary<NSNumber *, NSNumber *> new];
    
    [self initGL];
    [self setUserInteractionEnabled:YES];
    [self setMultipleTouchEnabled:YES];
    
    return self;
}

- (void)initGL {
    float scaleFactor = [UIScreen mainScreen].scale;
    if ([[UIScreen mainScreen] respondsToSelector:@selector(nativeScale)]) {
        scaleFactor = [UIScreen mainScreen].nativeScale;
    }
    self.contentScaleFactor = scaleFactor;
    self.layer.contentsScale = scaleFactor;
    
    CAEAGLLayer* glLayer = (CAEAGLLayer *)self.layer;
    glLayer.opaque = YES;
    glLayer.drawableProperties = @{kEAGLDrawablePropertyRetainedBacking:[NSNumber numberWithBool:FALSE],
                                   kEAGLDrawablePropertyColorFormat:kEAGLColorFormatRGBA8};
    
    self.glContext = [[EAGLContext alloc] initWithAPI:kEAGLRenderingAPIOpenGLES2];
    [EAGLContext setCurrentContext:self.glContext];
    
    glGenFramebuffers(1, &_frameBuffer);
    glBindFramebuffer(GL_FRAMEBUFFER, self.frameBuffer);
    
    glGenRenderbuffers(1, &_colorBuffer);
    glBindRenderbuffer(GL_RENDERBUFFER, self.colorBuffer);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_RGBA8_OES, _glWidth, _glHeight);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_RENDERBUFFER, self.colorBuffer);

    /*
    glGenRenderbuffers(1, &_depthBuffer);
    glBindRenderbuffer(GL_RENDERBUFFER, self.depthBuffer);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8_OES, _glWidth, _glHeight);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, self.depthBuffer);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_STENCIL_ATTACHMENT, GL_RENDERBUFFER, self.depthBuffer);
     */

    [self.glContext renderbufferStorage:GL_RENDERBUFFER fromDrawable:(CAEAGLLayer*)self.layer];

    glGetRenderbufferParameteriv(GL_RENDERBUFFER, GL_RENDERBUFFER_WIDTH, &_glWidth);
    glGetRenderbufferParameteriv(GL_RENDERBUFFER, GL_RENDERBUFFER_HEIGHT, &_glHeight);
    
    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
        NSLog(@"framebuffer is invalid.");
    }
}

- (void)touchesBegan:(NSSet *)touches withEvent:(UIEvent *)event {
    for (UITouch *touch in touches) {
        NSNumber *p = [NSNumber numberWithLong:(long)(intptr_t)touch];
        if ([_touchIdDict objectForKey:p] == nil) {
            [_touchIdDict setObject:[NSNumber numberWithUnsignedInteger:_nextTouchId] forKey:p];
            _nextTouchId++;
        }
        NSNumber *touchId = _touchIdDict[p];
        CGPoint location = [touch locationInView:self];
        [self.touchEventDelegate fireTouchEvent:touchId.unsignedIntValue location:location action:TouchActionDown];
    }
}

- (void)touchesMoved:(NSSet *)touches withEvent:(UIEvent *)event {
    for (UITouch *touch in touches) {
        NSNumber *p = [NSNumber numberWithLong:(long)(intptr_t)touch];
        NSNumber *touchId = _touchIdDict[p];
        CGPoint location = [touch locationInView:self];
        [self.touchEventDelegate fireTouchEvent:touchId.unsignedIntValue location:location action:TouchActionMove];
    }
}

- (void)touchesEnded:(NSSet *)touches withEvent:(UIEvent *)event {
    for (UITouch *touch in touches) {
        NSNumber *p = [NSNumber numberWithLong:(long)(intptr_t)touch];
        NSNumber *touchId = _touchIdDict[p];
        CGPoint location = [touch locationInView:self];
        [self.touchEventDelegate fireTouchEvent:touchId.unsignedIntValue location:location action:TouchActionUp];
        
        if ([_touchIdDict objectForKey:p] != nil) {
            [_touchIdDict removeObjectForKey:p];
        }
        if (_touchIdDict.count == 0) {
            _nextTouchId = 1;
        }
    }
}

- (void)touchesCancelled:(NSSet *)touches withEvent:(UIEvent *)event {
    [self touchesEnded:touches withEvent:event];
}

@end
