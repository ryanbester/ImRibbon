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

#ifdef WIN32
    static long long old_wnd_proc;

    static LRESULT borderlessWindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
        switch (uMsg) {
            case WM_NCACTIVATE:
            case WM_NCPAINT:
                // Stop curving corners on window restore
                return DefWindowProcW(hwnd, uMsg, wParam, lParam);
            case WM_NCCALCSIZE: {
                // Window resizing
                RECT &rect = *reinterpret_cast<RECT *>(lParam);
                RECT client = rect;

                CallWindowProc(reinterpret_cast<WNDPROC>(old_wnd_proc), hwnd, uMsg, wParam, lParam);

                if (IsMaximized(hwnd)) {
                    WINDOWINFO window_info = {0};
                    window_info.cbSize = sizeof(WINDOWINFO);
                    GetWindowInfo(hwnd, &window_info);

                    rect = RECT{
                            .left = static_cast<LONG>(client.left + window_info.cyWindowBorders),
                            .top = static_cast<LONG>(client.top + window_info.cyWindowBorders),
                            .right = static_cast<LONG>(client.right - window_info.cyWindowBorders),
                            .bottom = static_cast<LONG>(client.bottom - window_info.cyWindowBorders)
                    };
                } else {
                    rect = client;
                }

                return WVR_REDRAW;
            }
            case WM_NCHITTEST: {
                // Handle window resizing and moving
                POINT cursor = {GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam)};

                const POINT border{
                        static_cast<LONG>((GetSystemMetrics(SM_CXFRAME) + GetSystemMetrics(SM_CXPADDEDBORDER))),
                        static_cast<LONG>((GetSystemMetrics(SM_CYFRAME) + GetSystemMetrics(SM_CXPADDEDBORDER)))
                };

                RECT window;
                if (!GetWindowRect(hwnd, &window)) {
                    return HTNOWHERE;
                }

                bool top_border = cursor.y <= (window.top + border.y);
                bool bottom_border = cursor.y >= (window.bottom - border.y);
                bool left_border = cursor.x <= (window.left + border.x);
                bool right_border = cursor.x >= (window.right - border.x);

                // Corners
                if (top_border && left_border) return HTTOPLEFT;
                if (top_border && right_border) return HTTOPRIGHT;
                if (bottom_border && left_border) return HTBOTTOMLEFT;
                if (bottom_border && right_border) return HTBOTTOMRIGHT;

                // Edges
                if (top_border) return HTTOP;
                if (bottom_border) return HTBOTTOM;
                if (left_border) return HTLEFT;
                if (right_border) return HTRIGHT;

                // Title bar bounds checking
                if ((float) cursor.y >= ((float) window.top + GImRibbon.TitleBarBoundsMin.y)
                    && (float) cursor.y <= ((float) window.top + GImRibbon.TitleBarBoundsMax.y)) {
                    if ((float) cursor.x >= ((float) window.left + GImRibbon.TitleBarBoundsMin.x)
                        && (float) cursor.x <= ((float) window.left + GImRibbon.TitleBarBoundsMax.x)) {
                        return HTCAPTION;
                    }
                }

                return HTCLIENT;
            }
            default:
                break;
        }

        return CallWindowProc(reinterpret_cast<WNDPROC>(old_wnd_proc), hwnd, uMsg, wParam, lParam);
    }

#endif

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
