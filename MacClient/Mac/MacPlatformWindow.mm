#import "MacPlatformWindow.h"
#import <Cocoa/Cocoa.h>
#import <Metal/Metal.h>
#import <MetalKit/MetalKit.h>
#include "Core/Engine.h"

@interface WindowDelegate : NSObject <NSWindowDelegate>
@property (nonatomic, assign) MacPlatformWindow* platformWindow;
@end

@implementation WindowDelegate
- (BOOL)windowShouldClose:(NSWindow*)sender {
    if (_platformWindow) {
        _platformWindow->Closed.Invoke();
    }
    return YES;
}

- (void)windowDidResize:(NSNotification*)notification {
    if (_platformWindow) {
        _platformWindow->Resized.Invoke();
    }
}

- (void)windowDidBecomeKey:(NSNotification*)notification {
    if (_platformWindow) {
        _platformWindow->Focused.Invoke();
    }
}

- (void)windowDidResignKey:(NSNotification*)notification {
    if (_platformWindow) {
        _platformWindow->Unfocused.Invoke();
    }
}
@end

MacPlatformWindow::MacPlatformWindow(Engine* engine) : PlatformWindow(engine) {
}

MacPlatformWindow::~MacPlatformWindow() {
    Shutdown();
}

bool MacPlatformWindow::Initialize(const std::string& title, int width, int height) {
    @autoreleasepool {
        // Create window
        NSRect contentRect = NSMakeRect(0, 0, width, height);
        NSWindowStyleMask styleMask = NSWindowStyleMaskTitled | 
                                      NSWindowStyleMaskClosable | 
                                      NSWindowStyleMaskMiniaturizable | 
                                      NSWindowStyleMaskResizable;
        
        NSWindow* window = [[NSWindow alloc] initWithContentRect:contentRect
                                                       styleMask:styleMask
                                                         backing:NSBackingStoreBuffered
                                                           defer:NO];
        
        if (!window) {
            return false;
        }
        
        [window setTitle:[NSString stringWithUTF8String:title.c_str()]];
        [window center];
        [window makeKeyAndOrderFront:nil];
        
        // Create delegate
        WindowDelegate* delegate = [[WindowDelegate alloc] init];
        delegate.platformWindow = this;
        [window setDelegate:delegate];
        
        // Create Metal view
        MTKView* view = [[MTKView alloc] initWithFrame:contentRect];
        view.device = MTLCreateSystemDefaultDevice();
        [window setContentView:view];
        
        nsWindow = (__bridge void*)window;
        nsView = (__bridge void*)view;
        windowHandle = (__bridge void*)view;
        
        return true;
    }
}

void MacPlatformWindow::Shutdown() {
    if (nsWindow) {
        @autoreleasepool {
            NSWindow* window = (__bridge NSWindow*)nsWindow;
            WindowDelegate* delegate = (WindowDelegate*)[window delegate];
            [delegate release];
            [window close];
            [window release];
            nsWindow = nullptr;
        }
    }
    
    if (nsView) {
        @autoreleasepool {
            MTKView* view = (__bridge MTKView*)nsView;
            [view release];
            nsView = nullptr;
        }
    }
}

bool MacPlatformWindow::PollEvents() {
    @autoreleasepool {
        NSEvent* event;
        while ((event = [NSApp nextEventMatchingMask:NSEventMaskAny
                                           untilDate:[NSDate distantPast]
                                              inMode:NSDefaultRunLoopMode
                                             dequeue:YES])) {
            [NSApp sendEvent:event];
            [NSApp updateWindows];
        }
        
        NSWindow* window = (__bridge NSWindow*)nsWindow;
        return window && ![window isClosed];
    }
}

void* MacPlatformWindow::GetNativeWindowHandle() const {
    return windowHandle;
}

Rect<int> MacPlatformWindow::GetInternalBounds() const {
    if (!nsWindow) {
        return Rect<int>(0, 0, 0, 0);
    }
    
    @autoreleasepool {
        NSWindow* window = (__bridge NSWindow*)nsWindow;
        NSRect contentRect = [[window contentView] frame];
        return Rect<int>(
            static_cast<int>(contentRect.origin.x),
            static_cast<int>(contentRect.origin.y),
            static_cast<int>(contentRect.size.width),
            static_cast<int>(contentRect.size.height)
        );
    }
}

Rect<int> MacPlatformWindow::GetExternalBounds() const {
    if (!nsWindow) {
        return Rect<int>(0, 0, 0, 0);
    }
    
    @autoreleasepool {
        NSWindow* window = (__bridge NSWindow*)nsWindow;
        NSRect frame = [window frame];
        return Rect<int>(
            static_cast<int>(frame.origin.x),
            static_cast<int>(frame.origin.y),
            static_cast<int>(frame.size.width),
            static_cast<int>(frame.size.height)
        );
    }
}
