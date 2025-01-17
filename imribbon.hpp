// SPDX-License-Identifier: MIT
// SPDX-FileCopyrightText: 2025 Ryan Bester
//
// ImRibbon - Ribbon library for ImGui
// Copyright (c) 2025 Ryan Bester
//
// This API is designed mostly in line with ImGui, from Styling, to the Begin() and End() functions.

#ifndef IMRIBBON
#define IMRIBBON

#include <glad/glad.h>

#include <GLFW/glfw3.h>

#if defined(WIN32)

#define GLFW_EXPOSE_NATIVE_WIN32

#elif defined(__APPLE__)

#define GLFW_EXPOSE_NATIVE_COCOA

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
#define IMRIBBON_GLFW_DECL(decl) decl
#else
#define IMRIBBON_GLFW_DECL(decl)
#endif

#include <string>

namespace ImRibbon {

#pragma region Enums

//-----------------------------------------------------------------------------
// [SECTION] ENUMS
//-----------------------------------------------------------------------------

    enum ImRibbonCol_ {
        ImRibbonCol_TitleBarBg,
        ImRibbonCol_COUNT
    };

    enum WindowAction_ {
        WindowAction_None,
        WindowAction_Minimize,
        WindowAction_Maximize,
        WindowAction_Close
    };

#pragma endregion

#pragma region Structures

//-----------------------------------------------------------------------------
// [SECTION] STRUCTURES
//-----------------------------------------------------------------------------

    struct ImRibbonStyle {
        ImVec2 TitleBarPadding{0.0f, 0.0f};
        ImVec4 Colors[ImRibbonCol_COUNT];
    };

    struct ImRibbonContext {
        IMRIBBON_GLFW_DECL(GLFWwindow *Window{nullptr});
        bool Borderless{false};

        ImRibbonStyle Style{};

        bool WithinTitleBar{false};
        ImVec2 TitleBarBoundsMin{}; // Title bar bounds for window dragging
        ImVec2 TitleBarBoundsMax{}; // Title bar bounds for window dragging
        int TitleBarHeight{0}; // Calculated external height of the title bar

        bool WithinQuickAccess{false};
        int QuickAccessHeight{0}; // Calculate external height of the quick access bar
        std::vector<std::string> QuickAccessItems{};
    };

    struct RibbonGroup {
        std::string title;
        const ImVec2 &size;
    };

#pragma endregion

    void InitImRibbon(); // Must be called after ImGui::CreateContext()
    void InitImRibbonSettingsHandler();

#pragma region Windowing

//-----------------------------------------------------------------------------
// [SECTION] WINDOWING
//-----------------------------------------------------------------------------

    void SetBorderlessWindow(bool borderless); // Must be called before the window is created
    IMRIBBON_GLFW_DECL(void SetupWindow(GLFWwindow *win));

#pragma endregion

#pragma region Styling

//-----------------------------------------------------------------------------
// [SECTION] STYLING
//-----------------------------------------------------------------------------

    ImRibbonStyle &GetStyle();
    void StyleColorsDark(ImRibbonStyle *dst = nullptr);
    void StyleColorsLight(ImRibbonStyle *dst = nullptr);
    void StyleColorsClassic(ImRibbonStyle *dst = nullptr);

#pragma endregion

#pragma region Title Bar

//-----------------------------------------------------------------------------
// [SECTION] TITLE BAR
//-----------------------------------------------------------------------------

    bool BeginTitleBar(int height = 22);
    bool TitleBarButton(const char *label);
    bool TitleBarButtonRight(const char *label);
    void TitleBarText(const char *label);
    WindowAction_ ShowDefaultControls();
    void EndTitleBar();

#pragma endregion

#pragma region Quick Access Bar

//-----------------------------------------------------------------------------
// [SECTION] QUICK ACCESS BAR
//-----------------------------------------------------------------------------

    bool BeginQuickAccessBar();
    void EndQuickAccessBar();

#pragma endregion

}

#endif
