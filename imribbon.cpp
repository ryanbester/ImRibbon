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

    void SetBorderlessWindow(bool borderless) {

#ifdef IMRIBBON_GLFW
        glfwWindowHint(GLFW_DECORATED, borderless ? GLFW_FALSE : GLFW_TRUE);
#endif
    }

#ifdef IMRIBBON_GLFW

    void SetupWindow(GLFWwindow *win) {

    }

#endif

    bool BeginRibbonGroup(const char *title, const ImVec2 &size) {
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


        return true;
    }

    void EndRibbonGroup() {

    }

}
