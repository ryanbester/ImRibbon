// SPDX-License-Identifier: MIT
// SPDX-FileCopyrightText: 2025 Ryan Bester
//
// ImRibbon - Ribbon library for ImGui
// Copyright (c) 2025 Ryan Bester

#include "imribbon.hpp"

#include <iostream>

namespace ImRibbon {
    ImRibbonContext GImRibbon = ImRibbonContext{};

    void InitImRibbon() {
        GImRibbon.Style = ImRibbonStyle();

        StyleColorsDark(&GImRibbon.Style);
    }

#pragma region Windowing

    void SetBorderlessWindow(bool borderless) {

#ifdef IMRIBBON_GLFW
        glfwWindowHint(GLFW_DECORATED, borderless ? GLFW_FALSE : GLFW_TRUE);
#endif
    }

#ifdef IMRIBBON_GLFW

    void SetupWindow(GLFWwindow *win) {
#ifdef WIN32
        auto hwnd = glfwGetWin32Window(win);

        auto style = GetWindowLong(hwnd, GWL_STYLE);
        style |= WS_OVERLAPPEDWINDOW;
        style &= ~WS_POPUP;

        SetWindowLong(hwnd, GWL_STYLE, style);

        auto ex_style = GetWindowLong(hwnd, GWL_EXSTYLE);
        ex_style |= WS_EX_LAYERED;

        SetWindowLong(hwnd, GWL_EXSTYLE, ex_style);

        old_wnd_proc = SetWindowLongPtr(hwnd, GWLP_WNDPROC, reinterpret_cast<LONG_PTR>(borderlessWindowProc));
#endif
    }

#endif

#pragma endregion

#pragma region Styling

    ImRibbonStyle &GetStyle() {
        return GImRibbon.Style;
    }

    void StyleColorsDark(ImRibbonStyle *dst) {
        ImRibbonStyle *style = dst ? dst : &GetStyle();
        ImVec4 *colors = style->Colors;

        colors[ImRibbonCol_TitleBarBg] = ImGui::GetStyleColorVec4(ImGuiCol_TitleBgActive);
    }

    void StyleColorsLight(ImRibbonStyle *dst) {
        ImRibbonStyle *style = dst ? dst : &GetStyle();
        ImVec4 *colors = style->Colors;

        colors[ImRibbonCol_TitleBarBg] = ImGui::GetStyleColorVec4(ImGuiCol_TitleBgActive);
    }

    void StyleColorsClassic(ImRibbonStyle *dst) {
        ImRibbonStyle *style = dst ? dst : &GetStyle();
        ImVec4 *colors = style->Colors;

        colors[ImRibbonCol_TitleBarBg] = ImGui::GetStyleColorVec4(ImGuiCol_TitleBgActive);
    }

#pragma endregion

        return true;
    }

    void EndRibbonGroup() {

    }

}
