# VRInteraction Plugin for Unreal Engine 5.6

UE Plugin with basic VR interaction

## Plugin Structure

This plugin provides a foundation for VR interaction in Unreal Engine 5.6 projects.

### Directory Structure

```
VRInteraction/
├── VRInteraction.uplugin     # Plugin descriptor file
├── Source/                    # C++ source code
│   └── VRInteraction/
│       ├── VRInteraction.Build.cs
│       ├── Public/
│       │   └── VRInteractionModule.h
│       └── Private/
│           └── VRInteractionModule.cpp
├── Resources/                 # Plugin resources (icons, etc.)
│   └── README.md              # Instructions for adding plugin icon
└── Content/                   # Blueprints and assets (to be added)
```

## Features

- Compatible with Unreal Engine 5.6
- OpenXR support enabled
- Runtime module for VR interaction
- Support for Windows, Linux, Mac, and Android platforms

## Installation

1. Copy the plugin folder to your project's `Plugins` directory
2. Regenerate project files
3. Build your project
4. Enable the plugin in the Unreal Engine Editor (Edit > Plugins > Virtual Reality > VR Interaction)

## Usage

Assets and blueprints can be added to the `Content/` directory to extend the plugin's functionality.
