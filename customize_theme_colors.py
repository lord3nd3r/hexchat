#!/usr/bin/env python3
import os

BRIGHT_BASE = [
    "#ffffff", "#ff5f52", "#f9c74f", "#90be6d", "#43aa8b", "#4d908e", "#577590",
    "#277da1", "#9d4edd", "#8338ec", "#7209b7", "#560bad", "#ff006e", "#ff9f1c",
    "#fb5607", "#ffb703", "#06d6a0", "#118ab2", "#073b4c"
]

DEFAULT_UI = {
    "bg_color": "#1e1e1e",
    "fg_color": "#f8f8f2",
    "selected_bg": "#44475a",
    "selected_fg": "#ffffff",
    "input_bg": "#2c2c2c",
    "input_fg": "#ffffff",
    "nicklist_bg": "#252525",
    "nicklist_fg": "#dddddd",
    "chanlist_bg": "#1e1e1e",
    "chanlist_fg": "#f8f8f2",
    "tree_bg": "#1e1e1e",
    "tree_fg": "#f8f8f2",
    "away_user": "#b8b8ff",
    "separator": "#6b6bff",
}

THEME_SPECS = {
    "dracula": {
        "accent": ["#f8f8f2", "#ff6e6e", "#50fa7b", "#bd93f9", "#ff79c6", "#8be9fd", "#ffb86c", "#ffd670"],
        "shift": 0,
        "ui": {
            "bg_color": "#181926",
            "fg_color": "#f8f8f2",
            "selected_bg": "#44475a",
            "selected_fg": "#ffffff",
            "input_bg": "#1f1f2a",
            "input_fg": "#f8f8f2",
            "nicklist_bg": "#10101a",
            "nicklist_fg": "#f8f8f2",
        },
    },
    "monokai": {
        "accent": ["#f8f8f2", "#ff6188", "#a9dc76", "#ffd866", "#78dce8", "#ab9df2", "#ff966c", "#ffffff"],
        "shift": 1,
        "ui": {
            "bg_color": "#272822",
            "fg_color": "#f8f8f2",
            "selected_bg": "#49483e",
            "selected_fg": "#ffffff",
            "input_bg": "#383830",
            "input_fg": "#ffffff",
            "nicklist_bg": "#232320",
            "nicklist_fg": "#f8f8f2",
        },
    },
    "solarized-dark": {
        "accent": ["#93a1a1", "#dc322f", "#b58900", "#268bd2", "#2aa198", "#859900", "#cb4b16", "#eee8d5"],
        "shift": 2,
        "ui": {
            "bg_color": "#002b36",
            "fg_color": "#93a1a1",
            "selected_bg": "#073642",
            "selected_fg": "#fdf6e5",
            "input_bg": "#073642",
            "input_fg": "#fdf6e5",
            "nicklist_bg": "#001f27",
            "nicklist_fg": "#93a1a1",
        },
    },
    "solarized-light": {
        "accent": ["#586e75", "#cb4b16", "#839496", "#b58900", "#268bd2", "#d33682", "#2aa198", "#073642"],
        "shift": 3,
        "ui": {
            "bg_color": "#fdf6e3",
            "fg_color": "#586e75",
            "selected_bg": "#eee8d5",
            "selected_fg": "#073642",
            "input_bg": "#eee8d5",
            "input_fg": "#073642",
            "nicklist_bg": "#fdf6e3",
            "nicklist_fg": "#586e75",
            "separator": "#b58900",
        },
    },
    "github-light": {
        "accent": ["#24292f", "#cf222e", "#116329", "#bf8700", "#0969da", "#8250df", "#1b7c83", "#ffffff"],
        "shift": 4,
        "ui": {
            "bg_color": "#ffffff",
            "fg_color": "#24292f",
            "selected_bg": "#f6f8fa",
            "selected_fg": "#000000",
            "input_bg": "#f6f8fa",
            "input_fg": "#24292f",
            "nicklist_bg": "#f6f8fa",
            "nicklist_fg": "#24292f",
            "separator": "#d0d7de",
        },
    },
    "ocean-blue": {
        "accent": ["#0f172a", "#38bdf8", "#28a745", "#f97316", "#1d4ed8", "#a855f7", "#22d3ee", "#fb7185"],
        "shift": 5,
        "ui": {
            "bg_color": "#020c1b",
            "fg_color": "#e0f6ff",
            "selected_bg": "#0f172a",
            "selected_fg": "#a5f3fc",
            "input_bg": "#06132a",
            "input_fg": "#f0f9ff",
            "nicklist_bg": "#000f1f",
            "nicklist_fg": "#2dd4bf",
        },
    },
    "forest-green": {
        "accent": ["#0b3d0b", "#16a34a", "#4ade80", "#a3e635", "#22c55e", "#15803d", "#bbf7d0", "#facc15"],
        "shift": 6,
        "ui": {
            "bg_color": "#0f1f0f",
            "fg_color": "#d4f8d4",
            "selected_bg": "#134617",
            "selected_fg": "#d4f8d4",
            "input_bg": "#102513",
            "input_fg": "#f7ffe5",
            "nicklist_bg": "#0c120c",
            "nicklist_fg": "#b7ffb7",
        },
    },
    "midnight-purple": {
        "accent": ["#1a1a2e", "#e0d7ff", "#9d4edd", "#d946ef", "#9333ea", "#7c3aed", "#60a5fa", "#c084fc"],
        "shift": 7,
        "ui": {
            "bg_color": "#0b071a",
            "fg_color": "#ece2ff",
            "selected_bg": "#2e1b46",
            "selected_fg": "#ffffff",
            "input_bg": "#141026",
            "input_fg": "#f3e8ff",
            "nicklist_bg": "#05050f",
            "nicklist_fg": "#f6e9ff",
        },
    },
    "sunset-warm": {
        "accent": ["#2c1a0b", "#ff4500", "#ff8c00", "#f4a261", "#e76f51", "#ffb703", "#fb8500", "#ffffff"],
        "shift": 8,
        "ui": {
            "bg_color": "#2e0f00",
            "fg_color": "#ffe6c7",
            "selected_bg": "#ff8a00",
            "selected_fg": "#ffffff",
            "input_bg": "#321600",
            "input_fg": "#fff6e6",
            "nicklist_bg": "#1b0800",
            "nicklist_fg": "#ffe6c7",
        },
    },
    "minimal-light": {
        "accent": ["#ffffff", "#cbd5f5", "#94a3b8", "#475569", "#6366f1", "#ec4899", "#22d3ee", "#a3e635"],
        "shift": 9,
        "ui": {
            "bg_color": "#fdfdfd",
            "fg_color": "#0f172a",
            "selected_bg": "#e2e8f0",
            "selected_fg": "#0f172a",
            "input_bg": "#e5e7eb",
            "input_fg": "#0f172a",
            "nicklist_bg": "#f8fafc",
            "nicklist_fg": "#0f172a",
        },
    },
    "high-contrast": {
        "accent": ["#ffffff", "#ff0080", "#7c3aed", "#16a34a", "#eab308", "#0ea5e9", "#ef4444", "#f97316"],
        "shift": 10,
        "ui": {
            "bg_color": "#050505",
            "fg_color": "#ffffff",
            "selected_bg": "#111111",
            "selected_fg": "#00ffea",
            "input_bg": "#111111",
            "input_fg": "#ffffff",
            "nicklist_bg": "#050505",
            "nicklist_fg": "#ffffff",
        },
    },
    "retro-terminal": {
        "accent": ["#001308", "#00ff23", "#00ffff", "#ffff00", "#ff00ff", "#ff6b6b", "#00a8ff", "#c084fc"],
        "shift": 11,
        "ui": {
            "bg_color": "#000c03",
            "fg_color": "#c0f5c0",
            "selected_bg": "#003300",
            "selected_fg": "#f5ffef",
            "input_bg": "#002200",
            "input_fg": "#c5ffb5",
            "nicklist_bg": "#010a01",
            "nicklist_fg": "#c5ffb5",
        },
    },
    "pastel-dreams": {
        "accent": ["#fff1f3", "#ffb3c1", "#a7f3d0", "#bae6fd", "#c084fc", "#f472b6", "#fb923c", "#fef08a"],
        "shift": 12,
        "ui": {
            "bg_color": "#fffaf0",
            "fg_color": "#4c1d95",
            "selected_bg": "#dbeafe",
            "selected_fg": "#4c1d95",
            "input_bg": "#fef3c7",
            "input_fg": "#4c1d95",
            "nicklist_bg": "#fffaf0",
            "nicklist_fg": "#4c1d95",
        },
    },
    "rainbow-pride": {
        "accent": ["#e40303", "#ff8c00", "#ffed00", "#008026", "#004dff", "#750787", "#f72585", "#00b4d8"],
        "shift": 13,
        "ui": {
            "bg_color": "#020217",
            "fg_color": "#ffffff",
            "selected_bg": "#0f0f2f",
            "selected_fg": "#00b4d8",
            "input_bg": "#040424",
            "input_fg": "#ffffff",
            "nicklist_bg": "#01010f",
            "nicklist_fg": "#ffed00",
        },
    },
    "cyberpunk-neon": {
        "accent": ["#0efbff", "#ff0180", "#b5179e", "#ffb703", "#2ec4b6", "#fdffb6", "#ff9f1c", "#8338ec"],
        "shift": 14,
        "ui": {
            "bg_color": "#010005",
            "fg_color": "#faedf5",
            "selected_bg": "#1b0037",
            "selected_fg": "#ff8ee5",
            "input_bg": "#0e001c",
            "input_fg": "#faedf5",
            "nicklist_bg": "#040014",
            "nicklist_fg": "#00ffff",
        },
    },
    "test": {
        "accent": ["#ffffff", "#f72585", "#7209b7", "#3a0ca3", "#4361ee", "#4cc9f0", "#ffb703", "#ff006e"],
        "shift": 16,
        "ui": {
            "bg_color": "#07070f",
            "fg_color": "#ffffff",
            "selected_bg": "#151530",
            "selected_fg": "#00ffff",
            "input_bg": "#0b0b1f",
            "input_fg": "#fefefe",
            "nicklist_bg": "#05050c",
            "nicklist_fg": "#ffffff",
        },
    },
}


def hex_to_rgb(color: str):
    color = color.lstrip('#')
    if len(color) == 3:
        color = ''.join([c * 2 for c in color])
    return tuple(int(color[i : i + 2], 16) for i in (0, 2, 4))


def rgb_to_hex(rgb):
    return '#{0:02x}{1:02x}{2:02x}'.format(*rgb)


def brightness(color):
    r, g, b = hex_to_rgb(color)
    return (r * 299 + g * 587 + b * 114) / 1000


def is_dark(color):
    return brightness(color) < 140


def mix_colors(a, b, ratio):
    r1, g1, b1 = hex_to_rgb(a)
    r2, g2, b2 = hex_to_rgb(b)
    r = int(r1 + (r2 - r1) * ratio)
    g = int(g1 + (g2 - g1) * ratio)
    b = int(b1 + (b2 - b1) * ratio)
    return rgb_to_hex((min(255, r), min(255, g), min(255, b)))


def build_palette(accent, shift):
    palette = []
    for i in range(32):
        if i < len(accent):
            palette.append(accent[i])
        else:
            palette.append(BRIGHT_BASE[(i + shift) % len(BRIGHT_BASE)])
    return palette


def normalize_palette(palette, fg_color, bg_is_dark):
    adjusted = []
    for color in palette:
        if color.lower() == '#000000':
            adjusted.append(fg_color)
            continue
        if bg_is_dark and brightness(color) < 60:
            adjusted.append(mix_colors(color, fg_color, 0.65))
            continue
        adjusted.append(color)
    return adjusted


def sanitize_ui(ui):
    merged = DEFAULT_UI.copy()
    merged.update(ui)
    return merged


def load_theme_lines(path):
    with open(path, 'r') as f:
        return f.read().splitlines()


def save_theme_lines(path, lines):
    with open(path, 'w') as f:
        f.write('\n'.join(lines) + '\n')


def process_theme(path, spec):
    palette = build_palette(spec.get('accent', BRIGHT_BASE), spec.get('shift', 0))
    ui_colors = sanitize_ui(spec.get('ui', {}))
    bg_dark = is_dark(ui_colors['bg_color'])
    palette = normalize_palette(palette, ui_colors['fg_color'], bg_dark)

    lines = load_theme_lines(path)
    base_lines = []
    existing_text_colors = {}
    
    # Extract existing text colors
    for line in lines:
        stripped = line.strip()
        if stripped.startswith('text_color_'):
            parts = stripped.split('=', 1)
            if len(parts) == 2:
                key = parts[0]
                value = parts[1]
                existing_text_colors[key] = value
        else:
            base_lines.append(line)

    # Use standard IRC colors for text_color_0-15 to preserve user mode visibility
    irc_mode_colors = {
        'text_color_0': '#FFFFFF',  # White - Regular users
        'text_color_1': '#CCCCCC',  # Light Gray - Voice users
        'text_color_2': '#000080',  # Dark Blue - Halfop users  
        'text_color_3': '#008000',  # Dark Green - Op users
        'text_color_4': '#FF0000',  # Red - Admin users
        'text_color_5': '#800000',  # Dark Red - Owner users
        'text_color_6': '#800080',  # Purple
        'text_color_7': '#FFA500',  # Orange
        'text_color_8': '#FFFF00',  # Yellow
        'text_color_9': '#00FF00',  # Green
        'text_color_10': '#008080', # Teal
        'text_color_11': '#00FFFF', # Cyan
        'text_color_12': '#0000FF', # Blue
        'text_color_13': '#FF00FF', # Magenta
        'text_color_14': '#808080', # Gray
        'text_color_15': '#C0C0C0', # Light Gray
    }
    
    # Add IRC mode colors first (preserve user mode visibility)
    base_lines.append('# IRC user mode colors (preserved for visibility)')
    for i in range(16):
        key = f'text_color_{i}'
        # Always use standard IRC colors for mode visibility
        color = irc_mode_colors.get(key, '#FFFFFF')
        base_lines.append(f'{key}={color}')

    # Add theme-specific colors for text_color_16+
    base_lines.append('# Theme-specific vibrant text palette (auto-generated)')
    for idx, color in enumerate(palette):
        text_color_key = f'text_color_{idx + 16}'
        base_lines.append(f'{text_color_key}={color}')

    # Update UI colors
    for key, value in ui_colors.items():
        prefix = f'{key}='
        replaced = False
        for i, line in enumerate(base_lines):
            if line.strip().startswith(prefix):
                base_lines[i] = f'{key}={value}'
                replaced = True
                break
        if not replaced:
            base_lines.append(f'{key}={value}')

    save_theme_lines(path, base_lines)
    print(f'Updated {os.path.basename(path)}')


def main():
    theme_dir = os.path.expanduser('~/.config/hexchat/themes')
    if not os.path.isdir(theme_dir):
        print(f'Theme directory does not exist: {theme_dir}')
        return

    for filename in sorted(os.listdir(theme_dir)):
        if not filename.endswith('.theme'):
            continue
        path = os.path.join(theme_dir, filename)
        key = filename[:-6].lower()
        spec = THEME_SPECS.get(key, {})
        process_theme(path, spec)

    print('All themes refreshed!')


if __name__ == '__main__':
    main()
