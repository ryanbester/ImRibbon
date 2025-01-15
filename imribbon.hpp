// SPDX-License-Identifier: MIT
// SPDX-FileCopyrightText: 2025 Ryan Bester
//
// ImRibbon - Ribbon library for ImGui
// Copyright (c) 2025 Ryan Bester

#ifndef IMRIBBON
#define IMRIBBON

#include <glad/glad.h>

#include <GLFW/glfw3.h>

#if defined(WIN32)

#define GLFW_EXPOSE_NATIVE_WIN32

#endif

#ifdef IMRIBBON_GLFW

#include <GLFW/glfw3native.h>

#endif

#include <imgui.h>
#include <backends/imgui_impl_glfw.h>
#include <backends/imgui_impl_opengl3.h>
#include <imgui_internal.h>

#include <functional>

#if defined(WIN32)

#define WIN32_LEAN_AND_MEAN
#define NOMINMAX

#include <windows.h>
#include <windowsx.h>
#include <ShellScalingApi.h>
#include <dwmapi.h>

#endif

#ifdef IMRIBBON_GLFW
#define IMRIBBON_GLFW_FUNC(decl) decl
#else
#define IMRIBBON_GLFW_FUNC(decl)
#endif

#include <string>

namespace ImRibbon {

    struct RibbonGroup {
        std::string title;
        const ImVec2 &size;
    };

    void SetBorderlessWindow(bool borderless); // Must be called before the window is created

    IMRIBBON_GLFW_FUNC(void SetupWindow(GLFWwindow *win));

    bool BeginRibbonGroup(const char *title, const ImVec2 &size);

    void EndRibbonGroup();

}

#endif
