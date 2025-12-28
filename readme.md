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
- **Themes and Customization**: Extensive theming and configuration options

### New in Version 2.20

#### 🖼️ Image URL Context Menu (NEW!)
- **Smart Detection**: Automatically detects image URLs (.jpg, .png, .gif, .bmp, .webp)
- **Context Menu**: Right-click on image URLs to see "Open Image" option
- **External Viewer**: Opens images in your system's default image viewer
- **Seamless Integration**: Works with existing URL handling system

#### Enhanced Build System
- **Meson Build System**: Modern, fast build configuration
- **Parallel Compilation**: Optimized for multi-core systems
- **Comprehensive Dependencies**: Full GTK2, OpenSSL, and plugin support

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

# Build the project
ninja -C build

# Install (optional)
ninja -C build install
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
HexChat ("hexchat") Copyright (c) 2009-2025 By Berke Viktor.  
HexChat 2.20 Custom Build Copyright (c) 2025-2026 By Kristopher Craig.
</sub>
