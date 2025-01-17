// SPDX-License-Identifier: MIT
// SPDX-FileCopyrightText: 2025 Ryan Bester
//
// ImRibbon - Ribbon library for ImGui
// Copyright (c) 2025 Ryan Bester

#include "imribbon.hpp"

#include <iostream>
#include <sstream>
#include <utility>

#ifdef __APPLE__

#include "macos.h"

#endif

namespace ImRibbon {
    ImRibbonContext GImRibbon = ImRibbonContext{};

    void InitImRibbon() {
        GImRibbon.Style = ImRibbonStyle();

        StyleColorsDark(&GImRibbon.Style);

        InitImRibbonSettingsHandler();
    }

    void *RibbonSettingsHandler_ReadOpen(ImGuiContext *ctx, ImGuiSettingsHandler *handler, const char *name) {
        return (void *) name;
    }

    void RibbonSettingsHandler_ReadLine(ImGuiContext *ctx, ImGuiSettingsHandler *handler, void *entry, const char *line) {
        const std::string name = reinterpret_cast<const char *>(entry);
        if (name == "QuickAccess") {
            std::string line_str(line);
            if (line_str.starts_with("Items=")) {
                GImRibbon.QuickAccessItems.clear();

                std::istringstream stream(line_str.substr(6));
                std::string tmp{};

                for (char i; stream >> i;) {
                    tmp += i;
                    if (stream.peek() == ',') {
                        stream.ignore();
                        GImRibbon.QuickAccessItems.push_back(tmp);
                        tmp.clear();
                    }
                }

                GImRibbon.QuickAccessItems.push_back(tmp);
            }
        }

        // TODO: Other options
    }

    void RibbonSettingsHandler_WriteAll(ImGuiContext *ctx, ImGuiSettingsHandler *handler, ImGuiTextBuffer *buf) {
        buf->appendf("[ImRibbon][QuickAccess]\n");

        std::ostringstream items;
        for (size_t i = 0; i < GImRibbon.QuickAccessItems.size(); ++i) {
            items << GImRibbon.QuickAccessItems[i];
            if (i + 1 < GImRibbon.QuickAccessItems.size()) {
                items << ",";
            }
        }

        buf->appendf("Items=%s\n", items.str().c_str());
    }

    void InitImRibbonSettingsHandler() {
        ImGuiSettingsHandler handler;
        handler.TypeName = "ImRibbon";
        handler.TypeHash = ImHashStr("ImRibbon");
        handler.ReadOpenFn = RibbonSettingsHandler_ReadOpen;
        handler.ReadLineFn = RibbonSettingsHandler_ReadLine;
        handler.WriteAllFn = RibbonSettingsHandler_WriteAll;

        ImGui::AddSettingsHandler(&handler);
    }

    void RegisterCommand(const char *cmd_id, const char *label, const char *icon_path, std::function<void()> click_handler) {
        RibbonCommand cmd{
                label,
                icon_path != nullptr ? icon_path : "",
                std::move(click_handler)
        };
        GImRibbon.Commands[cmd_id] = cmd;
    }

    int GetContentAreaStartY() {
        return GImRibbon.TitleBarHeight + GImRibbon.QuickAccessHeight + GImRibbon.MenuBarHeight; // TODO: + GImRibbon.RibbonHeight;
    }

    void ExecuteRibbonCmd(const char *cmd_id) {
        GImRibbon.ClickedCmd = cmd_id;
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
        GImRibbon.Borderless = borderless;
#ifdef IMRIBBON_GLFW
#ifdef __APPLE__
        glfwWindowHint(GLFW_DECORATED, GLFW_TRUE);
#else
        glfwWindowHint(GLFW_DECORATED, borderless ? GLFW_FALSE : GLFW_TRUE);
#endif
#endif
    }

#ifdef IMRIBBON_GLFW

    void SetupWindow(GLFWwindow *win) {
        GImRibbon.Window = win;

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

#ifdef __APPLE__
        MacSetupWindow(win, GImRibbon.Borderless);
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

#pragma region Title Bar

    bool BeginTitleBar(int height) {
        assert(GImRibbon.WithinTitleBar == false);

        ImGuiViewport *viewport = ImGui::GetMainViewport();
        auto start = ImVec2(viewport->Pos.x, viewport->Pos.y);

        ImGui::SetNextWindowPos(start);
        ImGui::SetNextWindowSize(ImVec2(viewport->Size.x, (float) height));

        ImGui::PushStyleVar(ImGuiStyleVar_WindowMinSize, ImVec2(0.0f, 0.0f));
        ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0);

        ImGui::Begin("##ImRibbonTitleBar", nullptr,
                     ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoDocking);

        ImGui::PopStyleVar(2);

        ImGui::GetWindowDrawList()->AddRectFilled(start, ImVec2(viewport->Size.x, (float) height),
                                                  ImGui::GetColorU32(GetStyle().Colors[ImRibbonCol_TitleBarBg]));

        height = (int) ImGui::GetWindowHeight();

        GImRibbon.TitleBarHeight = height;
        GImRibbon.TitleBarBoundsMax = ImVec2(start.x + viewport->Size.x, start.y + (float) height);

#ifdef __APPLE__
        // Allow space for MacOS window controls
        start.x += 68;
#endif

        GImRibbon.TitleBarBoundsMin = start;

        GImRibbon.WithinTitleBar = true;
        return GImRibbon.WithinTitleBar;
    }

    bool TitleBarButton(const char *label) {
        assert(GImRibbon.WithinTitleBar == true);

        auto cursor_pos = ImVec2(GImRibbon.TitleBarBoundsMin.x + GetStyle().TitleBarPadding.x,
                                 GImRibbon.TitleBarBoundsMin.y + GetStyle().TitleBarPadding.y);
        ImGui::SetCursorPos(cursor_pos);

        const ImVec2 label_size = ImGui::CalcTextSize(label, nullptr, true);
        ImVec2 size = ImGui::CalcItemSize(ImVec2{}, label_size.x + ImGui::GetStyle().FramePadding.x * 2.0f,
                                          label_size.y + ImGui::GetStyle().FramePadding.y * 2.0f);

        size.y = (float) GImRibbon.TitleBarHeight - (GetStyle().TitleBarPadding.y * 2);
        if (size.x < size.y) size.x = (float) GImRibbon.TitleBarHeight - (GetStyle().TitleBarPadding.x * 2);

        bool clicked = ImGui::Button(label, size);
        GImRibbon.TitleBarBoundsMin.x = ImGui::GetItemRectMax().x;

        return clicked;
    }

    bool TitleBarButtonRight(const char *label) {
        assert(GImRibbon.WithinTitleBar == true);

        const ImVec2 label_size = ImGui::CalcTextSize(label, nullptr, true);
        ImVec2 size = ImGui::CalcItemSize(ImVec2{}, label_size.x + ImGui::GetStyle().FramePadding.x * 2.0f,
                                          label_size.y + ImGui::GetStyle().FramePadding.y * 2.0f);

        size.y = (float) GImRibbon.TitleBarHeight - (GetStyle().TitleBarPadding.y * 2);
        if (size.x < size.y) size.x = (float) GImRibbon.TitleBarHeight - (GetStyle().TitleBarPadding.x * 2);

        auto cursor_pos = ImVec2(GImRibbon.TitleBarBoundsMax.x - size.x - GetStyle().TitleBarPadding.x,
                                 GImRibbon.TitleBarBoundsMin.y + GetStyle().TitleBarPadding.y);
        ImGui::SetCursorPos(cursor_pos);

        bool clicked = ImGui::Button(label, size);
        GImRibbon.TitleBarBoundsMax.x = ImGui::GetItemRectMin().x;

        return clicked;
    }

    void TitleBarText(const char *label) {
        assert(GImRibbon.WithinTitleBar == true);

        ImGui::AlignTextToFramePadding();


        auto cursor_pos = ImVec2(GImRibbon.TitleBarBoundsMin.x + GetStyle().TitleBarPadding.x,
                                 GImRibbon.TitleBarBoundsMin.y + GetStyle().TitleBarPadding.y);
        ImGui::SetCursorPos(cursor_pos);

        ImGui::TextUnformatted(label);

        const ImVec2 size = ImGui::GetItemRectSize();
        GImRibbon.TitleBarBoundsMin.x += size.x;
    }

    WindowAction_ ShowDefaultControls() {
        WindowAction_ action = WindowAction_None;

        // TODO: Better icons
        if (TitleBarButtonRight("X")) action = WindowAction_Close;
        if (TitleBarButtonRight("+")) action = WindowAction_Maximize;
        if (TitleBarButtonRight("_")) action = WindowAction_Minimize;

        return action;
    }

    void EndTitleBar() {
        assert(GImRibbon.WithinTitleBar == true);

        ImGui::End();

        GImRibbon.WithinTitleBar = false;
    }

#pragma endregion

#pragma region Quick Access Bar

    bool BeginQuickAccessBar() {
        assert(GImRibbon.WithinQuickAccess == false);

        if (GImRibbon.WithinTitleBar) {
            // QA embedded in title bar
            GImRibbon.WithinQuickAccess = true;
        } else {
            ImGuiViewport *viewport = ImGui::GetMainViewport();

            int height = 22;

            auto start = ImVec2(viewport->Pos.x, viewport->Pos.y + (float) GImRibbon.TitleBarHeight);

            ImGui::SetNextWindowPos(start);
            ImGui::SetNextWindowSize(ImVec2(viewport->Size.x, (float) height));

            ImGui::Begin("##ImRibbonQABar", nullptr,
                         ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoDocking);

            height = (int) ImGui::GetWindowHeight();

            GImRibbon.QuickAccessHeight = height;

            GImRibbon.WithinQuickAccess = true;
        }

        return GImRibbon.WithinQuickAccess;
    }

    const std::vector<std::string> &GetQuickAccessItems() {
        return GImRibbon.QuickAccessItems;
    }

    void AddQuickAccessItems() {
        for (const auto &cmd_id: GImRibbon.QuickAccessItems) {
            auto cmd_it = GImRibbon.Commands.find(cmd_id);
            std::string label{cmd_id};
            if (cmd_it != GImRibbon.Commands.end()) {
                label = cmd_it->second.Label;
            }

            if (GImRibbon.WithinTitleBar) {
                // Use title bar buttons if in embedded in title bar
                if (TitleBarButton(label.c_str())) {
                    GImRibbon.ClickedCmd = cmd_id;
                }
            } else {
                if (ImGui::Button(label.c_str())) {
                    GImRibbon.ClickedCmd = cmd_id;
                }
                ImGui::SameLine();
            }
        }
    }

    void EndQuickAccessBar() {
        assert(GImRibbon.WithinQuickAccess == true);

        if (!GImRibbon.WithinTitleBar) {
            ImGui::End();
        }

        GImRibbon.WithinQuickAccess = false;
    }

#pragma endregion

#pragma region Menu Bar

    bool BeginMenuBar() {
        assert(GImRibbon.WithinMenuBar == false);

        ImGuiViewport *viewport = ImGui::GetMainViewport();

        // Set height for first run
        int height = 22;
        if (GImRibbon.MenuBarHeight != 0) {
            height = GImRibbon.MenuBarHeight;
        }

        auto start = ImVec2(viewport->Pos.x, viewport->Pos.y + (float) GImRibbon.TitleBarHeight + (float) GImRibbon.QuickAccessHeight);

        ImGui::SetNextWindowPos(start);
        ImGui::SetNextWindowSize(ImVec2(viewport->Size.x, (float) height));

        ImGui::PushStyleVar(ImGuiStyleVar_WindowMinSize, ImVec2(0.0f, 0.0f));
        ImGui::Begin("##ImRibbonMenuBar", nullptr,
                     ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoDocking | ImGuiWindowFlags_MenuBar);
        ImGui::PopStyleVar();

        GImRibbon.WithinMenuBar = ImGui::BeginMenuBar();

        height = (int)(ImGui::GetStyle().FramePadding.y * 2.0f + ImGui::GetFontSize()); // Estimate height of menu bar

        GImRibbon.MenuBarHeight = height;

        return GImRibbon.WithinMenuBar;
    }

    void EndMenuBar() {
        assert(GImRibbon.WithinMenuBar == true);

        ImGui::EndMenuBar();
        ImGui::End();

        GImRibbon.WithinMenuBar = false;
    }

#pragma endregion

}
