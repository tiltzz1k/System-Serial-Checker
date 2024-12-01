# System Serial Checker with Gradient Text

Overview:
This repository contains a C++ application that retrieves and displays system hardware information such as Baseboard Serial Number, Disk Serial Number, and System UUID using Windows Management Instrumentation (WMI). The output is enhanced with a color gradient effect, including ASCII art and hardware information, providing a visually appealing way to present system data.

# Features

WMI Integration:

Queries system information using Win32_BaseBoard, Win32_DiskDrive, and Win32_ComputerSystemProduct classes.
Retrieves:
* Baseboard Serial Number.
* Disk Drive Serial Number.
* System UUID.
* 
# Gradient Text Output:

Applies a gradient effect to ASCII art, section titles, and retrieved serial numbers.
Smooth transition from light blue to purple for an aesthetic console output.

# ASCII Art Header:
Displays a customizable ASCII art title with gradient coloring for branding or visual flair.

C
# Compatibility:
* Designed for Windows x64 systems.
