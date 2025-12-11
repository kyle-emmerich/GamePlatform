#import <Cocoa/Cocoa.h>
#import <Metal/Metal.h>
#import <MetalKit/MetalKit.h>
#import "MacPlatformWindow.h"
#include "Core/Engine.h"
#include "MacKeyCodeMapping.h"

@interface GameView : MTKView
@property (nonatomic, assign) MacPlatformWindow* platformWindow;
@end

@implementation GameView

- (BOOL)acceptsFirstResponder {
    return YES;
}

- (void)keyDown:(NSEvent *)event {
    if (_platformWindow) {
        KeyCode key = MacKeyCodeToKeyCode([event keyCode]);
        if (key != KeyCode::None) {
            _platformWindow->KeyInputReceived.Fire(_platformWindow->Id, key, InputState::Begin);
        }
        
        NSString* chars = [event characters];
        if ([chars length] > 0) {
            std::string text([chars UTF8String]);
            bool hasControlChars = false;
            for (unsigned char c : text) {
                if (c < 32 || c == 127) {
                    hasControlChars = true;
                    break;
                }
            }
            
            if (!hasControlChars) {
                _platformWindow->ReceivedTextInput.Fire(text);
            }
        }
    }
}

- (void)keyUp:(NSEvent *)event {
    if (_platformWindow) {
        KeyCode key = MacKeyCodeToKeyCode([event keyCode]);
        if (key != KeyCode::None) {
            _platformWindow->KeyInputReceived.Fire(_platformWindow->Id, key, InputState::End);
        }
    }
}

- (void)flagsChanged:(NSEvent *)event {
    if (_platformWindow) {
        // Handle modifiers
        // This is a bit complex because flagsChanged doesn't tell us which key changed, just the new state.
        // We can infer it by checking the key code.
        KeyCode key = MacKeyCodeToKeyCode([event keyCode]);
        if (key != KeyCode::None) {
            // Determine if it's a press or release based on the flag bit
            NSUInteger flags = [event modifierFlags];
            bool isPressed = false;
            
            switch (key) {
                case KeyCode::LeftShift:
                case KeyCode::RightShift:
                    isPressed = (flags & NSEventModifierFlagShift) != 0;
                    break;
                case KeyCode::LeftControl:
                case KeyCode::RightControl:
                    isPressed = (flags & NSEventModifierFlagControl) != 0;
                    break;
                case KeyCode::LeftAlt:
                case KeyCode::RightAlt:
                    isPressed = (flags & NSEventModifierFlagOption) != 0;
                    break;
                case KeyCode::LeftMeta:
                case KeyCode::RightMeta:
                    isPressed = (flags & NSEventModifierFlagCommand) != 0;
                    break;
                case KeyCode::CapsLock:
                    isPressed = (flags & NSEventModifierFlagCapsLock) != 0;
                    break;
                default:
                    break;
            }
            
            _platformWindow->KeyInputReceived.Fire(_platformWindow->Id, key, isPressed ? InputState::Begin : InputState::End);
        }
    }
}

@end

@interface WindowDelegate : NSObject <NSWindowDelegate>
@property (nonatomic, assign) MacPlatformWindow* platformWindow;
@end

@implementation WindowDelegate
- (BOOL)windowShouldClose:(NSWindow*)sender {
    if (_platformWindow) {
        _platformWindow->Closed.Fire();
    }
    return YES;
}

- (void)windowDidResize:(NSNotification*)notification {
    if (_platformWindow) {
        _platformWindow->Resized.Fire();
    }
}

- (void)windowDidBecomeKey:(NSNotification*)notification {
    if (_platformWindow) {
        _platformWindow->Focused.Fire();
    }
}

- (void)windowDidResignKey:(NSNotification*)notification {
    if (_platformWindow) {
        _platformWindow->Unfocused.Fire();
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
        [NSApplication sharedApplication];
        [NSApp setActivationPolicy:NSApplicationActivationPolicyRegular];
        [NSApp finishLaunching];
        [NSApp activateIgnoringOtherApps:YES];

        // Create Menu Bar
        NSMenu* menubar = [[NSMenu alloc] init];
        [NSApp setMainMenu:menubar];
        
        NSMenuItem* appMenuItem = [[NSMenuItem alloc] init];
        [menubar addItem:appMenuItem];
        
        NSMenu* appMenu = [[NSMenu alloc] init];
        [appMenuItem setSubmenu:appMenu];
        
        [appMenu addItemWithTitle:[NSString stringWithFormat:@"Quit %@", [NSString stringWithUTF8String:title.c_str()]]
                           action:@selector(terminate:)
                    keyEquivalent:@"q"];

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
        [window setReleasedWhenClosed:NO];
        
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
        GameView* view = [[GameView alloc] initWithFrame:contentRect];
        view.device = MTLCreateSystemDefaultDevice();
        view.platformWindow = this;
        [view setPaused:YES];
        [view setEnableSetNeedsDisplay:NO];
        [window setContentView:view];
        [window makeFirstResponder:view];
        
        nsWindow = (__bridge void*)window;
        nsView = (__bridge void*)view;
        windowHandle = (__bridge void*)view;
        
        Closed.Connect([this]() {
            shouldClose = true;
        });
        
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
            GameView* view = (__bridge GameView*)nsView;
            view.platformWindow = nullptr;
            [view release];
            nsView = nullptr;
        }
    }
}

bool MacPlatformWindow::PollEvents() {
    if (!nsWindow) return false;
    if (shouldClose) return false;

    @autoreleasepool {
        NSEvent* event;
        while ((event = [NSApp nextEventMatchingMask:NSEventMaskAny
                                           untilDate:[NSDate distantPast]
                                              inMode:NSDefaultRunLoopMode
                                             dequeue:YES])) {
            @try {
                [NSApp sendEvent:event];
            }
            @catch (NSException *exception) {
                NSLog(@"Exception handling event: %@", exception);
            }
        }
        
        return !shouldClose;
    }
}

void* MacPlatformWindow::GetNativeWindowHandle() const {
    return windowHandle;
}

Math::Rect<int> MacPlatformWindow::GetInternalBounds() const {
    if (!nsWindow) {
        return Math::Rect<int>(0, 0, 0, 0);
    }
    
    @autoreleasepool {
        NSWindow* window = (__bridge NSWindow*)nsWindow;
        NSRect contentRect = [[window contentView] frame];
        return Math::Rect<int>(
            static_cast<int>(contentRect.origin.x),
            static_cast<int>(contentRect.origin.y),
            static_cast<int>(contentRect.size.width),
            static_cast<int>(contentRect.size.height)
        );
    }
}

Math::Rect<int> MacPlatformWindow::GetExternalBounds() const {
    if (!nsWindow) {
        return Math::Rect<int>(0, 0, 0, 0);
    }
    
    @autoreleasepool {
        NSWindow* window = (__bridge NSWindow*)nsWindow;
        NSRect frame = [window frame];
        return Math::Rect<int>(
            static_cast<int>(frame.origin.x),
            static_cast<int>(frame.origin.y),
            static_cast<int>(frame.size.width),
            static_cast<int>(frame.size.height)
        );
    }
}
