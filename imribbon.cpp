// SPDX-License-Identifier: MIT
// SPDX-FileCopyrightText: 2025 Ryan Bester
//
// ImRibbon - Ribbon library for ImGui
// Copyright (c) 2025 Ryan Bester

#include "imribbon.hpp"

#include <iostream>

namespace ImRibbon {


    void SetBorderlessWindow(bool borderless) {

    }

#ifdef IMRIBBON_GLFW

    void SetupWindow(GLFWwindow *win) {

    }

#endif

    bool BeginRibbonGroup(const char *title, const ImVec2 &size) {

        return true;
    }

    void EndRibbonGroup() {

    }

}
