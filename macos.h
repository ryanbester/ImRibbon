// SPDX-License-Identifier: MIT
// SPDX-FileCopyrightText: 2025 Ryan Bester
//
// ImRibbon - Ribbon library for ImGui
// Copyright (c) 2025 Ryan Bester

#ifdef __APPLE__

#define GLFW_EXPOSE_NATIVE_COCOA

#ifdef IMRIBBON_GLFW

#include <GLFW/glfw3.h>
#include <GLFW/glfw3native.h>

namespace ImRibbon {
    void MacSetupWindow(GLFWwindow *win, bool borderless);

    void MacHandleTitlebarDoubleClick(GLFWwindow *win);

    void MacSetWindowMovable(GLFWwindow *win, bool movable);
}

#endif

#endif
