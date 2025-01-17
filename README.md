# ImRibbon

Ribbon library for ImGui

## Features

- Designed similar to ImGui's API
- Custom Title Bar, incorporating Quick Access buttons

## Usage

Simply include ImRibbon.hpp and ImRibbon.cpp in your project, and define the suitable platform directive (see below).

For macOS, include macos.h and macos.mm, and enable support for Objective-C/C++.

## Platform Directives

Preprocessor directives for specifying windowing platform.

Currently, ImRibbon only provides full custom Title Bar support for GLFW.

- `IMRIBBON_GLFW`: GLFW

## Deferred Controls

Since Ribbon commands can be repeated across tabs, and of course on the Quick Access bar, as well as being
user-customisable, most of the ImRibbon control creation functions only draw the controls at a later call to
DrawControls().

Basic example:

```cpp
void execute_copy() { }
void execute_paste() { }

ImRibbon::RegisterCommand(
        "copy", // command ID
        "Copy", // label
        "copy.png", // icon
        execute_copy // click handler can be specified here, or when calling ImRibbon::Button()
);
ImRibbon::RegisterCommand(
        "paste", // command ID
        "Paste", // label
        "paste.png" // icon
);

// ImGui render loop
while(...) {
    if (ImRibbon::BeginQuickAccessBar()) {
        ImRibbon::AddQuickAccessItems(); // Adds Quick Access items in user-customised order
        ImRibbon::EndQuickAccessBar();
    }
    
    if (ImRibbon::BeginRibbon()) {
        // tab and group creation...
        ImRibbon::Button("copy"); // command ID
        if (ImRibbon::Button("paste")) {
            execute_paste();
            // Click will be consumed here, no other click handlers will be called
        }
        // ...
        ImRibbon::DrawControls(); // Adds controls in user-customised order
        ImRibbon::EndRibbon();
    }
}
```
