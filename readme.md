# HexChat 2.20

HexChat is an IRC client for Windows and UNIX-like operating systems.
See [IRCHelp.org](http://irchelp.org) for information about IRC in general.
For more information on HexChat please read our [documentation](https://hexchat.readthedocs.org/en/latest/index.html):
- [Downloads](http://hexchat.github.io/downloads.html)
- [FAQ](https://hexchat.readthedocs.org/en/latest/faq.html)
- [Changelog](https://hexchat.readthedocs.org/en/latest/changelog.html)
- [Python API](https://hexchat.readthedocs.org/en/latest/script_python.html)
- [Perl API](https://hexchat.readthedocs.org/en/latest/script_perl.html)

## Features

### Core Features
- **Multi-platform**: Works on Windows, Linux, macOS, and other UNIX-like systems
- **GTK2 Interface**: Clean, customizable graphical user interface
- **Plugin Support**: Extensible with Lua, Perl, and Python scripting
- **SSL/TLS Support**: Secure connections with OpenSSL
- **DCC Support**: Direct file transfers and chat
- **Multiple Networks**: Connect to multiple IRC networks simultaneously
- **Themes and Customization**: 18+ built-in themes with automatic installation
- **IRC User Mode Display**: Proper display of user roles (@op, %halfop, +voice, ~owner) in chat

### New in Version 2.20

#### 🎨 Comprehensive Theme System (MAJOR)
- **18+ Built-in Themes**: Professional themes including Dracula, Monokai, Solarized, Cyberpunk, and more
- **Automatic Installation**: Themes are automatically copied to user directories on first run
- **Cross-platform Support**: Works seamlessly on Linux, Windows, and macOS
- **Theme Manager**: Easy theme switching via View → Theme menu
- **Color Palette Fixes**: IRC user mode colors preserved for proper visibility

#### 🖼️ Image URL Context Menu (NEW!)
- **Smart Detection**: Automatically detects image URLs (.jpg, .png, .gif, .bmp, .webp)
- **Context Menu**: Right-click on image URLs to see "Open Image" option
- **External Viewer**: Opens images in your system's default image viewer
- **Seamless Integration**: Works with existing URL handling system

#### 🔧 IRC Functionality Improvements
- **User Mode Prefixes**: IRC user roles (@op, %halfop, +voice, ~owner) now display correctly in chat messages
- **Theme Compatibility**: Fixed theme colors that were breaking IRC user mode visibility
- **Enhanced Build System**: Meson/Ninja build system with improved cross-platform support

#### Plugin Ecosystem
- **Lua Support**: LuaJIT integration for high-performance scripting
- **Perl Support**: Full Perl API for advanced scripting
- **Python Support**: Python scripting capabilities (configurable)
- **Built-in Plugins**:
  - **Checksum**: DCC file verification
  - **FiSH**: Encrypted messaging
  - **Sysinfo**: System information display

## Building from Source

### Prerequisites

#### Ubuntu/Debian
```bash
# Install build tools and dependencies
sudo apt update
sudo apt install build-essential meson ninja-build pkg-config

# Install required libraries
sudo apt install libgtk2.0-dev libglib2.0-dev libssl-dev liblua5.1-dev libperl-dev python3-dev

# Install Python dependencies for plugins
sudo apt install python3-cffi
```

#### Other Linux Distributions
- **Fedora/RHEL/CentOS**: `sudo dnf install gtk2-devel glib2-devel openssl-devel lua-devel perl-devel python3-devel meson ninja-build`
- **Arch Linux**: `sudo pacman -S gtk2 glib2 openssl lua perl python meson ninja`
- **openSUSE**: `sudo zypper install gtk2-devel glib2-devel libopenssl-devel lua-devel perl-devel python3-devel meson ninja`

#### macOS
```bash
# Install Homebrew if not already installed
/bin/bash -c "$(curl -fsSL https://raw.githubusercontent.com/Homebrew/install/HEAD/install.sh)"

# Install dependencies
brew install gtk+ glib openssl lua perl python meson ninja pkg-config
pip3 install cffi
```

#### Windows
- Use MSYS2 or similar environment
- Install GTK2, GLib, OpenSSL, and other dependencies via pacman
- Follow the [Windows build guide](https://hexchat.readthedocs.org/en/latest/building.html#windows)

### System Requirements
- GTK+ 2.24 or later
- GLib 2.36 or later
- OpenSSL 1.0.1 or later
- Meson 0.53.0 or later
- Ninja build system
- GCC or compatible C compiler

### Quick Build
```bash
# Configure the build
meson setup build

# Build the project (use -j$(nproc) for parallel compilation)
ninja -C build

# Install system-wide (optional, requires root)
sudo ninja -C build install
```

### Running
```bash
# Run from build directory
./build/src/fe-gtk/hexchat

# Check version
./build/src/fe-gtk/hexchat --version
```

## Configuration

HexChat stores configuration files in:
- Linux/macOS: `~/.config/hexchat/`
- Windows: `%APPDATA%\HexChat\`

## Themes and Customization

HexChat includes a comprehensive theme system with automatic theme installation for all users.

### Automatic Theme Installation
- **System-wide Themes**: 18+ professionally designed themes are included in the build
- **First-run Setup**: Themes are automatically copied to each user's config directory on first launch
- **No Manual Setup**: Users get immediate access to a full theme collection without downloading or configuring

### Available Themes
The build includes these themes:
- **Dark Themes**: Dracula, Monokai, Solarized Dark, Cyberpunk Neon, Midnight Purple, Ocean Blue, Retro Terminal, Forest Green, High Contrast
- **Light Themes**: Solarized Light, GitHub Light, Minimal Light, Sunset Warm, Pastel Dreams, Rainbow Pride
- **Default Themes**: Default Dark and Default Light for basic usage

### Theme Management
- **Theme Menu**: Access themes via View → "_Theme" menu
- **Theme Manager**: Browse and switch themes easily
- **Custom Themes**: Users can add their own `.theme` files to `~/.config/hexchat/themes/`

### Palette Maintenance Script
The repository includes `customize_theme_colors.py`, a helper script that ensures theme palettes and UI colors are properly configured for optimal readability.

Run it to refresh existing themes:
```bash
python3 customize_theme_colors.py
```

This script updates palette colors to prevent issues like black text on dark backgrounds.

## Contributing

Contributions are welcome! Please:
1. Fork the repository
2. Create a feature branch
3. Make your changes
4. Test thoroughly
5. Submit a pull request

## License

HexChat is released under the GPL v2 with the additional exemption that compiling, linking, and/or using OpenSSL is allowed. You may provide binary packages linked to the OpenSSL libraries, provided that all other requirements of the GPL are met.

See file COPYING for details.

---

<sub>
X-Chat ("xchat") Copyright (c) 1998-2010 By Peter Zelezny.  
HexChat ("hexchat") Copyright (c) 2009-2024 By Berke Viktor.  
HexChat 2.20 Copyright (c) 2025-2026 By Kristopher Craig.
</sub>
