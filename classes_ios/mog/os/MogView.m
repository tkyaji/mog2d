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
        glDeleteFramebuffersOES(1, &_frameBuffer);
    }
    if (self.colorBuffer > 0) {
        glDeleteFramebuffersOES(1, &_colorBuffer);
    }
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
    
    self.glContext = [[EAGLContext alloc] initWithAPI:kEAGLRenderingAPIOpenGLES1];
    [EAGLContext setCurrentContext:self.glContext];
    
    glGenFramebuffersOES(1, &_frameBuffer);
    glBindFramebufferOES(GL_FRAMEBUFFER_OES, self.frameBuffer);
    
    glGenRenderbuffersOES(1, &_colorBuffer);
    glBindRenderbufferOES(GL_RENDERBUFFER_OES, self.colorBuffer);
    
    [self.glContext renderbufferStorage:GL_RENDERBUFFER_OES fromDrawable:(CAEAGLLayer*)self.layer];
    glFramebufferRenderbufferOES(GL_FRAMEBUFFER_OES, GL_COLOR_ATTACHMENT0_OES, GL_RENDERBUFFER_OES, self.colorBuffer);
    
    glGetRenderbufferParameterivOES(GL_RENDERBUFFER_OES, GL_RENDERBUFFER_WIDTH_OES, &_glWidth);
    glGetRenderbufferParameterivOES(GL_RENDERBUFFER_OES, GL_RENDERBUFFER_HEIGHT_OES, &_glHeight);
    
    if (glCheckFramebufferStatusOES(GL_FRAMEBUFFER_OES) != GL_FRAMEBUFFER_COMPLETE_OES) {
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
