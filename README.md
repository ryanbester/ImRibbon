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
