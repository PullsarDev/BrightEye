# Bright Eye Plugin

**Bright Eye** is a lightweight and free Unreal Engine plugin designed for level designers and artists who need an easily accessible light source while working within the editor. It attaches a light source directly to the viewport camera, allowing users to quickly illuminate dark areas without the need to place temporary lights. By providing customizable shortcuts and settings, Bright Eye offers a flexible workflow that adapts to the user's needs, streamlining the level design process.

## Table of Contents

---
**Table of Contents**:
<img align="right" width="55%" height="70%" src="https://github.com/user-attachments/assets/e4fb7f54-fd5a-4b17-9fc8-a88f56716d3b">
<br>

> 1. [Introduction to Bright Eye](#bright-eye-plugin)
> 2. [Features](#features)
> 3. [Installation](#installation)  
> 4. [Usage](#usage)
> 5. [Contribution](#contribution)
> 6. [License](#license)
> 7. [Contact](#contact)
> 8. [Credits](#credits)
> 9. [Disclaimer](#disclaimer)
---

## Features

- **Viewport Light Source**: Toggle a spotlight attached to the camera to enhance visibility.
- **Customizable Settings**: Adjust brightness, distance, and color of the light.
- **User-Friendly Interface**: Easily control the light with a dedicated control panel and shortcuts.
- **Two Modes**: Choose between a fixed light or one that follows the camera with a slight delay.
- **Customizable Activation Behavior**: Choose between holding the key to keep the light active or toggling it on and off with a single press.

## Installation

There are two ways to install the **Bright Eye** plugin:

### Method 1: Install to a Project

This method installs the plugin only for a specific project, making it available only within that particular project.

1. **Download the Plugin**: Go to the [Releases](https://github.com/PullsarDev/BrightEye/releases/tag/V1.0) page and download the binary ZIP file for your Unreal Engine version from the **Assets** section at the bottom of the page.
2. **Unzip the Files**: Extract the downloaded ZIP file.
3. **Move the Plugin**: Copy the `BrightEye` folder into your project's `Plugins` directory:
```
YourProject/
│
├── Binaries/
│
├── Content/
│
├── Intermediate/      
│
├── Plugins/
│   └── BrightEye/
└── 
```
4. **Enable the Plugin**: Open your project in Unreal Engine, go to `Edit` > `Plugins`, and enable the **Bright Eye** plugin.
5. **Restart the Editor**: After enabling the plugin, restart Unreal Engine to apply the changes.

### Method 2: Install to the Engine


This method installs the plugin directly into the Unreal Engine installation, making it available for all projects that use this engine version.

1. **Download the Plugin**: Go to the [Releases](https://github.com/PullsarDev/BrightEye/releases/tag/V1.0) page and download the binary ZIP file for your Unreal Engine version from the **Assets** section at the bottom of the page.
2. **Unzip the Files**: Extract the downloaded ZIP file.
3. **Move the Plugin**: Copy the `BrightEye` folder into your Unreal Engine installation's `Plugins` directory:
```
UE_Path/
├── Engine/
│   └── Plugins/
│         └── Editor/
│               └── BrightEye
```
4. **Enable the Plugin**: Open any project in Unreal Engine, go to `Edit` > `Plugins`, and enable the **Bright Eye** plugin.
5. **Restart the Editor**: After enabling the plugin, restart Unreal Engine to apply the changes.

## Usage

### 1. Using The Light Source
- Press the `T` key to toggle the light on and off while in the viewport.
- Press `Ctrl + T` to open the **Control Panel**.

#### Adjusting Light Settings
Once the **Control Panel** is open, you can adjust the following settings:

- **Brightness**: Controls the intensity of the light source.
- **Radius**: Adjusts the radius of the light's influence in the scene.
- **Distance**: Sets the distance from the camera where the light source will be end.

#### Additional Features in the Control Panel
- The **Sync Mode** button in the top-left corner of the panel allows you to toggle between **Sync Mode** and **Lag Mode**. In **Lag Mode**, the light source follows the camera's rotation with a slight delay, creating a more dynamic lighting effect.
- The **Light Color** button in the top-right corner of the panel lets you change the color of the light source, allowing you to match the lighting to your scene's needs.

### 2. Advanced Settings
Open the **Editor Preferences**, find the **Plugins** section, and select the **Bright Eye** tab. Here, you can fine-tune the tool's behavior:

- **Brightness**: Adjust the default brightness level of the light.
- **Radius**: Set the default radius of the light's effect.
- **Distance**: Define the default distance from the camera to the light source.
- **Max Brightness**: Set the maximum brightness value that can be applied.
- **Max Distance**: Set the maximum distance the light can be from the camera.
- **Color**: Choose the default color of the light source.
- **Activate Light on Press**: If enabled, the light will stay active only while holding down the assigned key. If disabled, the light toggles on with one press and off with another.
- **Hide Panel when Idle**: If enabled, the control panel will automatically hide when not in use.
- **Smooth Camera Rotation**: Enable this option to smooth out the camera's rotation when using the light source in **Lag Mode**.
- **Rotation Delay Factor**: Adjust the delay factor that determines how much the light lags behind the camera's rotation in **Lag Mode**.

### 3. Changing Keyboard Shortcuts
To customize the keyboard shortcuts for Bright Eye:

- Open the **Editor Preferences** and navigate to the **Keyboard Shortcuts** section.
- Find the **Bright Eye Actions** section, or type "Bright Eye" into the Search Bar.
- Here, you can modify the shortcuts for toggling the light and opening the Control Panel, or add new shortcuts as needed.

**Note:** Bright Eye is an editor-only plugin and does not affect gameplay or runtime performance.

## Contribution

Contributions are welcome! If you'd like to report a bug, suggest a feature, or submit a pull request, please check the [Issues](https://github.com/PullsarDev/BrightEye/issues) section first.

## License

This project is licensed under the MIT License. See the [LICENSE](LICENSE) file for details.

## Contact

For any questions or further information, feel free to contact me:

- **Email**: akpinarab@gmail.com
- **GitHub**: [github.com/PullsarDev](https://github.com/PullsarDev)
- **Linkedin**: [linkedin.com/akpinarab](https://www.linkedin.com/in/akpinarab/)

## Credits

This plugin uses icons from [Flaticon](https://www.flaticon.com) under the [Flaticon License](https://www.flaticon.com/media/license/license.pdf).

## Disclaimer

Unreal Engine and its logo are trademarks or registered trademarks of Epic Games, Inc. in the United States of America and elsewhere. All rights reserved. This plugin is not affiliated with or endorsed by Epic Games.
