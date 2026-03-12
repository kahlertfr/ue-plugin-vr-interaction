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
│       │   ├── VRInteractionModule.h
│       │   ├── VRLogManager.h        # Subsystem for collecting log messages
│       │   └── VRLogDisplayActor.h   # Abstract base actor for log display Blueprints
│       └── Private/
│           ├── VRInteractionModule.cpp
│           ├── VRLogManager.cpp
│           └── VRLogDisplayActor.cpp
├── Resources/                 # Plugin resources (icons, etc.)
│   └── README.md              # Instructions for adding plugin icon
└── Content/                   # Blueprints and assets (to be added)
```

## Features

- Compatible with Unreal Engine 5.6
- OpenXR support enabled
- Runtime module for VR interaction
- Support for Windows, Linux, Mac, and Android platforms
- **In-scene VR log display** – push log messages from any C++ class or Blueprint and show them on a formatted text surface inside the virtual scene

## Installation

1. Copy the plugin folder to your project's `Plugins` directory
2. Regenerate project files
3. Build your project
4. Enable the plugin in the Unreal Engine Editor (Edit > Plugins > Virtual Reality > VR Interaction)

## Usage

Assets and blueprints can be added to the `Content/` directory to extend the plugin's functionality.

---

## In-Scene VR Log Display

The plugin provides two classes that work together to display log messages on a text surface inside the VR scene.

### Classes

| Class | Type | Purpose |
|-------|------|---------|
| `UVRLogManager` | `UGameInstanceSubsystem` | Stores log messages; any code can push messages to it |
| `AVRLogDisplayActor` | `AActor` (Abstract, Blueprintable) | Base class for the Blueprint that renders the log text |

---

### Step 1 – Declare the module dependency

> **This step is required for every plugin or game module that uses `UVRLogManager` from C++.**
> Skipping it produces a linker error such as:
> `unresolved external symbol "__declspec(dllimport) … Z_Construct_UClass_UVRLogManager_NoRegister"`

#### 1a – Build.cs

In the consuming module's `.Build.cs` add `"VRInteraction"` to the dependency list:

```csharp
// YourPlugin/Source/YourPlugin/YourPlugin.Build.cs
PublicDependencyModuleNames.AddRange(new string[]
{
    "Core", "CoreUObject", "Engine",
    "VRInteraction"   // ← required to link against UVRLogManager
});
```

Use `PrivateDependencyModuleNames` instead if you only reference `UVRLogManager` from `.cpp` files (not from public headers).

#### 1b – .uplugin (plugin-to-plugin only)

If the consuming module lives inside another UE plugin (not directly in the game project), also declare the dependency in that plugin's `.uplugin` descriptor so the engine loads VRInteraction first:

```json
// YourPlugin/YourPlugin.uplugin
{
    "Plugins": [
        {
            "Name": "VRInteraction",
            "Enabled": true
        }
    ]
}
```

---

### Step 2 – Push log messages from C++

Obtain the subsystem via the `UGameInstance` and call `AddMessage`:

```cpp
#include "VRLogManager.h"

// Inside any Actor, Component, or other UObject that has a World context:
if (UGameInstance* GI = GetGameInstance())
{
    if (UVRLogManager* Log = GI->GetSubsystem<UVRLogManager>())
    {
        Log->AddMessage(TEXT("Calibration complete."));
    }
}
```

From Blueprint: `Get Game Instance → Get Subsystem (VRLogManager) → Add Message`.

---

### Step 3 – Create the display Blueprint

1. In the **Content Browser** right-click → **Blueprint Class**.
2. Search for `VRLogDisplayActor` and select it as the parent class.
3. Name it, e.g. `BP_VRLogDisplay`.

#### Add a text surface

Choose one of the two common approaches:

**Option A – TextRenderComponent (simple 3D text)**

1. In the Blueprint editor click **Add Component** and search for `Text Render`.
2. Rename it `LogText`.
3. Adjust `World Size`, font, and color as desired.

**Option B – WidgetComponent (UMG widget)**

1. Add a `Widget Component` and assign a UMG widget that contains a `Text` block named `LogText`.
2. Set `Widget Space` to `World` for in-scene rendering.

#### Implement `OnLogTextUpdated`

In the **Event Graph** of `BP_VRLogDisplay`:

1. Right-click → search for **On Log Text Updated** (the Blueprint implementable event).
2. Drag from the `New Text` pin and connect it to the `Set Text` node of your `LogText` component.

Example graph (Option A):

```
Event OnLogTextUpdated (NewText)
    └─► LogText (TextRenderComponent) → Set Text (Value = NewText)
```

---

### Step 4 – Configure the actor

Select `BP_VRLogDisplay` in the **Details** panel to tune the formatting:

| Property | Default | Description |
|----------|---------|-------------|
| `Max Display Lines` | 10 | Maximum number of lines shown on screen at once. Older lines scroll off. |
| `Max Line Width` | 40 | Maximum characters per line. Longer messages are word-wrapped automatically. |

To cap how many messages `UVRLogManager` keeps in memory, configure `MaxStoredMessages` on the subsystem at runtime (e.g. in your GameMode's `BeginPlay`):

```cpp
GetGameInstance()->GetSubsystem<UVRLogManager>()->MaxStoredMessages = 50;
```

The default is **100**. When the limit is reached the oldest message is dropped before the new one is stored.

---

### Step 5 – Place the actor in the level

Drag `BP_VRLogDisplay` from the Content Browser into the level and position it where it should appear in the VR scene (e.g. attached to a wall or floating near the player).

The actor subscribes to `UVRLogManager::OnLogUpdated` automatically on `BeginPlay` and calls `UpdateLogDisplay` (which calls `OnLogTextUpdated`) every time a new message is added or the log is cleared.

---

### FormatMessages helper

`AVRLogDisplayActor::FormatMessages` is a static `BlueprintCallable` function you can also call independently:

```cpp
FText Formatted = AVRLogDisplayActor::FormatMessages(MyMessages, /*MaxLines=*/5, /*MaxWidth=*/30);
```

It:
1. Word-wraps each message at `InMaxLineWidth` characters (preserving existing newlines as hard breaks).
2. Keeps only the **last** `InMaxDisplayLines` lines so the output always fits the display.
