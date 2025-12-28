#!/usr/bin/env python3
import os
import glob

# Standard mIRC color palette
mirc_colors = [
    "#FFFFFF", "#000000", "#000080", "#008000", "#FF0000", "#800000", "#800080", "#FFA500",
    "#FFFF00", "#00FF00", "#008080", "#00FFFF", "#0000FF", "#FF00FF", "#808080", "#C0C0C0",
    "#470000", "#472100", "#474700", "#324700", "#004700", "#00472C", "#004747", "#002747",
    "#000047", "#2E0047", "#470047", "#470024", "#CCCCCC", "#FFFFFF", "#FFFFFF", "#FFFFFF"
]

theme_dir = os.path.expanduser("~/.config/hexchat/themes")
if not os.path.exists(theme_dir):
    print("Theme directory not found")
    exit(1)

for theme_file in glob.glob(os.path.join(theme_dir, "*.theme")):
    print(f"Updating {theme_file}")
    
    with open(theme_file, 'r') as f:
        content = f.read()
    
    # Add text colors if not already present
    lines = content.strip().split('\n')
    text_color_lines = []
    for i, color in enumerate(mirc_colors):
        text_color_lines.append(f"text_color_{i}={color}")
    
    # Check if text colors are already in the file
    has_text_colors = any("text_color_" in line for line in lines)
    
    if not has_text_colors:
        # Add text colors before the last line (which should be empty or a comment)
        if lines and lines[-1].strip() == "":
            lines = lines[:-1]  # Remove empty line
        lines.extend(text_color_lines)
        lines.append("")  # Add empty line at end
        
        with open(theme_file, 'w') as f:
            f.write('\n'.join(lines))
        print(f"  Added text colors to {os.path.basename(theme_file)}")
    else:
        print(f"  Text colors already present in {os.path.basename(theme_file)}")

print("Theme update complete!")
