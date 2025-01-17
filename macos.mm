// SPDX-License-Identifier: MIT
// SPDX-FileCopyrightText: 2025 Ryan Bester
//
// ImRibbon - Ribbon library for ImGui
// Copyright (c) 2025 Ryan Bester

#ifdef __APPLE__

#include "macos.h"

namespace ImRibbon {
    void MacSetupWindow(GLFWwindow *win, bool borderless) {
        NSWindow *cocoa_win = glfwGetCocoaWindow(win);

        cocoa_win.titleVisibility = NSWindowTitleHidden;

        if (borderless) {
            cocoa_win.titlebarAppearsTransparent = YES;
            cocoa_win.styleMask |= NSWindowStyleMaskFullSizeContentView;

            [cocoa_win setOpaque:NO];
            [cocoa_win setHasShadow:YES];
            [cocoa_win setBackgroundColor:[NSColor colorWithWhite:0 alpha:0.001f]];
        }
    }

    void MacHandleTitlebarDoubleClick(GLFWwindow *win) {
        NSWindow *cocoa_win = glfwGetCocoaWindow(win);

        NSString *action = [[NSUserDefaults standardUserDefaults] stringForKey:@"AppleActionOnDoubleClick"];

        if (action == nil || [action isEqualToString:@"None"]) {
            // Do nothing
        } else if ([action isEqualToString:@"Minimize"]) {
            if ([cocoa_win isMiniaturizable]) {
                [cocoa_win miniaturize:nil];
            }
        } else if ([action isEqualToString:@"Maximize"]) {
            CFRunLoopPerformBlock(CFRunLoopGetMain(), kCFRunLoopCommonModes, ^{
                if ([cocoa_win isZoomable]) {
                    [cocoa_win zoom:nil];
                }
            });
        }
    }

    void MacSetWindowMovable(GLFWwindow *win, bool movable) {
        NSWindow *cocoa_win = glfwGetCocoaWindow(win);

        [cocoa_win setMovable:movable];
    }
}

#endif
