#include "theme.h"
#include "../common/cfgfiles.h"
#include "../common/hexchat.h"
#include "../common/hexchatc.h"
#include "fe-gtk.h"
#include "maingui.h"
#include "palette.h"
#include "xtext.h"
#include <glib.h>
#include <glib/gstdio.h>
#include <string.h>

HexChatTheme *current_theme = NULL;
static char *theme_config_dir = NULL;
static gboolean theme_pending_apply = FALSE;

static void theme_free(HexChatTheme *theme);
static HexChatTheme *theme_new(void);
static int theme_parse_line(HexChatTheme *theme, const char *line);
static void theme_set_defaults(HexChatTheme *theme);

static void theme_sync_global_palette(HexChatTheme *theme,
                                      GtkWidget *alloc_widget);

static void theme_sync_global_palette(HexChatTheme *theme,
                                      GtkWidget *alloc_widget) {
  int i;

  if (!theme)
    return;

  g_message("Theme: Syncing global palette - bg_color=#%02x%02x%02x "
            "fg_color=#%02x%02x%02x",
            theme->bg_color.red >> 8, theme->bg_color.green >> 8,
            theme->bg_color.blue >> 8, theme->fg_color.red >> 8,
            theme->fg_color.green >> 8, theme->fg_color.blue >> 8);

  /* 0-31: mIRC palette */
  for (i = 0; i < 32; i++)
    colors[i] = theme->text_colors[i];

  /* xtext specials */
  colors[COL_MARK_FG] = theme->selected_fg;
  colors[COL_MARK_BG] = theme->selected_bg;
  colors[COL_FG] = theme->fg_color;
  colors[COL_BG] = theme->bg_color;
  colors[COL_MARKER] = theme->separator;

  /* GUI extras (keep defaults; set away user to match theme) */
  colors[COL_AWAY] = theme->away_user;

  /* Allocate into current colormap so pixel values match updated RGBs */
  if (alloc_widget) {
    palette_realloc(alloc_widget);

    /* Sync back the allocated pixels to the theme object so they can be used
       directly in styles and other widgets */
    for (i = 0; i < 32; i++)
      theme->text_colors[i] = colors[i];

    theme->selected_fg = colors[COL_MARK_FG];
    theme->selected_bg = colors[COL_MARK_BG];
    theme->fg_color = colors[COL_FG];
    theme->bg_color = colors[COL_BG];
    theme->separator = colors[COL_MARKER];
    theme->away_user = colors[COL_AWAY];

    /* Also allocate the other UI colors that aren't in the global palette */
    GdkColormap *cmap = gtk_widget_get_colormap(alloc_widget);
    if (cmap) {
      gdk_colormap_alloc_color(cmap, &theme->input_bg, FALSE, TRUE);
      gdk_colormap_alloc_color(cmap, &theme->input_fg, FALSE, TRUE);
      gdk_colormap_alloc_color(cmap, &theme->nicklist_bg, FALSE, TRUE);
      gdk_colormap_alloc_color(cmap, &theme->nicklist_fg, FALSE, TRUE);
      gdk_colormap_alloc_color(cmap, &theme->chanlist_bg, FALSE, TRUE);
      gdk_colormap_alloc_color(cmap, &theme->chanlist_fg, FALSE, TRUE);
      gdk_colormap_alloc_color(cmap, &theme->tree_bg, FALSE, TRUE);
      gdk_colormap_alloc_color(cmap, &theme->tree_fg, FALSE, TRUE);
    }

    /* If the theme background is dark, avoid pure-black palette entries
       because they render as unreadable black text on dark backgrounds.
       Replace any pure-black entries in the mIRC palette with the
       theme foreground so text remains readable. */
    {
      /* compute background brightness (0-255) */
      int bg_b = (theme->bg_color.red >> 8) * 299 + (theme->bg_color.green >> 8) * 587 + (theme->bg_color.blue >> 8) * 114;
      bg_b = bg_b / 1000;
      if (bg_b < 128) {
        GdkColor fg = theme->fg_color;
        int j;
        for (j = 0; j < 32; j++) {
          if (colors[j].red == 0 && colors[j].green == 0 && colors[j].blue == 0) {
            colors[j] = fg;
            theme->text_colors[j] = fg;
          }
        }
        /* also adjust common duplicate black entries in extended set if any */
        if (colors[17].red == 0 && colors[17].green == 0 && colors[17].blue == 0)
          colors[17] = fg;
      }
    }
  }

  g_message("Theme: Global palette synced and allocated");
}

void theme_init(void) {
  g_message("Theme: Initializing theme system");
  theme_config_dir = g_build_filename(get_xdir(), "themes", NULL);
  if (!theme_config_dir) {
    g_warning("Theme: Failed to get config directory");
    return;
  }

  g_message("Theme: Creating theme directory: %s", theme_config_dir);
  if (g_mkdir_with_parents(theme_config_dir, 0755) != 0) {
    g_warning("Theme: Failed to create theme directory: %s", theme_config_dir);
  }

  /* If the user's theme directory has no .theme files, copy system
     installed themes into it (one-time first-run behavior). This looks
     through the system data dirs for "hexchat/themes" and copies any
     .theme files we find. */
  {
    gboolean has_user_themes = FALSE;
    GDir *udir = g_dir_open(theme_config_dir, 0, NULL);
    const char *ufn;
    if (udir) {
      while ((ufn = g_dir_read_name(udir))) {
        if (g_str_has_suffix(ufn, ".theme")) {
          has_user_themes = TRUE;
          break;
        }
      }
      g_dir_close(udir);
    }

    if (!has_user_themes) {
      gchar **sysdirs = g_get_system_data_dirs();
      int si;
      for (si = 0; sysdirs && sysdirs[si]; si++) {
        gchar *cand = g_build_filename(sysdirs[si], "hexchat", "themes", NULL);
        GDir *sdir = g_dir_open(cand, 0, NULL);
        const char *sfn;
        if (!sdir) {
          g_free(cand);
          continue;
        }

        g_message("Theme: No user themes found, copying system themes from %s", cand);
        while ((sfn = g_dir_read_name(sdir))) {
          if (!g_str_has_suffix(sfn, ".theme"))
            continue;

          gchar *src = g_build_filename(cand, sfn, NULL);
          gchar *dst = g_build_filename(theme_config_dir, sfn, NULL);
          if (g_file_test(dst, G_FILE_TEST_EXISTS)) {
            g_free(src);
            g_free(dst);
            continue;
          }

          gchar *contents = NULL;
          gsize length = 0;
          GError *err = NULL;
          if (g_file_get_contents(src, &contents, &length, &err)) {
            if (!g_file_set_contents(dst, contents, length, &err)) {
              g_warning("Theme: Failed to write theme %s: %s", dst,
                        err ? err->message : "unknown");
            } else {
              g_message("Theme: Installed user theme: %s", dst);
            }
            g_free(contents);
          } else if (err) {
            g_warning("Theme: Failed to read system theme %s: %s", src, err->message);
          }
          if (err)
            g_clear_error(&err);

          g_free(src);
          g_free(dst);
        }

        g_dir_close(sdir);
        g_free(cand);
      }
      g_strfreev(sysdirs);
    }
  }

  /* Create default dark theme if no theme is loaded */
  g_message("Theme: Creating default dark theme");
  theme_create_default_dark();
  g_message("Theme: Theme system initialized");
}

void theme_cleanup(void) {
  if (current_theme) {
    theme_free(current_theme);
    current_theme = NULL;
  }
  g_free(theme_config_dir);
  theme_config_dir = NULL;
}

char *theme_get_config_dir(void) { return theme_config_dir; }

static HexChatTheme *theme_new(void) {
  HexChatTheme *theme = g_new0(HexChatTheme, 1);
  theme_set_defaults(theme);
  return theme;
}

static void theme_free(HexChatTheme *theme) {
  if (!theme)
    return;

  g_free(theme->name);
  g_free(theme->author);
  g_free(theme->description);
  g_free(theme->version);
  g_free(theme->font_normal);
  g_free(theme->font_bold);
  g_free(theme->font_italic);

  g_free(theme);
}

static void theme_set_defaults(HexChatTheme *theme) {
  /* Default theme values */
  theme->name = g_strdup("Default");
  theme->author = g_strdup("HexChat");
  theme->description = g_strdup("Default theme");
  theme->version = g_strdup("1.0");

  /* Default colors - light theme */
  gdk_color_parse("#FFFFFF", &theme->bg_color);    /* White background */
  gdk_color_parse("#000000", &theme->fg_color);    /* Black text */
  gdk_color_parse("#4A90E2", &theme->selected_bg); /* Blue selection */
  gdk_color_parse("#FFFFFF", &theme->selected_fg); /* White selected text */
  gdk_color_parse("#F8F8F8", &theme->input_bg);    /* Light gray input */
  gdk_color_parse("#000000", &theme->input_fg);    /* Black input text */
  gdk_color_parse("#F5F5F5", &theme->nicklist_bg); /* Very light gray */
  gdk_color_parse("#000000", &theme->nicklist_fg); /* Black nicklist */
  gdk_color_parse("#FFFFFF", &theme->chanlist_bg); /* White */
  gdk_color_parse("#000000", &theme->chanlist_fg); /* Black */
  gdk_color_parse("#FFFFFF", &theme->tree_bg);     /* White */
  gdk_color_parse("#000000", &theme->tree_fg);     /* Black */
  gdk_color_parse("#888888", &theme->away_user);   /* Gray for away users */
  gdk_color_parse("#CCCCCC", &theme->separator);   /* Light gray separator */

  /* Default text colors - Standard mIRC color palette */
  gdk_color_parse("#FFFFFF", &theme->text_colors[0]);  /* White */
  gdk_color_parse("#CCCCCC", &theme->text_colors[1]);  /* Light Gray (instead of black for dark theme) */
  gdk_color_parse("#000080", &theme->text_colors[2]);  /* Dark Blue */
  gdk_color_parse("#008000", &theme->text_colors[3]);  /* Dark Green */
  gdk_color_parse("#FF0000", &theme->text_colors[4]);  /* Red */
  gdk_color_parse("#800000", &theme->text_colors[5]);  /* Dark Red */
  gdk_color_parse("#800080", &theme->text_colors[6]);  /* Purple */
  gdk_color_parse("#FFA500", &theme->text_colors[7]);  /* Orange */
  gdk_color_parse("#FFFF00", &theme->text_colors[8]);  /* Yellow */
  gdk_color_parse("#00FF00", &theme->text_colors[9]);  /* Green */
  gdk_color_parse("#008080", &theme->text_colors[10]); /* Teal */
  gdk_color_parse("#00FFFF", &theme->text_colors[11]); /* Cyan */
  gdk_color_parse("#0000FF", &theme->text_colors[12]); /* Blue */
  gdk_color_parse("#FF00FF", &theme->text_colors[13]); /* Magenta */
  gdk_color_parse("#808080", &theme->text_colors[14]); /* Gray */
  gdk_color_parse("#C0C0C0", &theme->text_colors[15]); /* Light Gray */
  /* Extended colors */
  gdk_color_parse("#470000", &theme->text_colors[16]); /* Dark Red */
  gdk_color_parse("#472100", &theme->text_colors[17]); /* Brown */
  gdk_color_parse("#474700", &theme->text_colors[18]); /* Olive */
  gdk_color_parse("#324700", &theme->text_colors[19]); /* Dark Green */
  gdk_color_parse("#004700", &theme->text_colors[20]); /* Green */
  gdk_color_parse("#00472C", &theme->text_colors[21]); /* Teal */
  gdk_color_parse("#004747", &theme->text_colors[22]); /* Dark Cyan */
  gdk_color_parse("#002747", &theme->text_colors[23]); /* Blue */
  gdk_color_parse("#000047", &theme->text_colors[24]); /* Dark Blue */
  gdk_color_parse("#2E0047", &theme->text_colors[25]); /* Dark Magenta */
  gdk_color_parse("#470047", &theme->text_colors[26]); /* Dark Purple */
  gdk_color_parse("#470024", &theme->text_colors[27]); /* Dark Red */
  gdk_color_parse("#CCCCCC", &theme->text_colors[28]); /* Light Gray */
  gdk_color_parse("#FFFFFF", &theme->text_colors[29]); /* White */
  gdk_color_parse("#FFFFFF", &theme->text_colors[30]); /* White */
  gdk_color_parse("#FFFFFF", &theme->text_colors[31]); /* White */

  /* Default fonts */
  theme->font_normal = g_strdup("Monospace 9");
  theme->font_bold = g_strdup("Monospace Bold 9");
  theme->font_italic = g_strdup("Monospace Italic 9");

  theme->border_width = 1;
  theme->padding = 2;
}

void theme_create_default_dark(void) {
  if (current_theme) {
    theme_free(current_theme);
  }

  current_theme = theme_new();

  /* Override with dark theme values */
  g_free(current_theme->name);
  g_free(current_theme->description);
  current_theme->name = g_strdup("Default Dark");
  current_theme->description = g_strdup("Modern dark theme for HexChat");

  /* Dark color scheme */
  gdk_color_parse("#1e1e1e",
                  &current_theme->bg_color); /* Dark gray background */
  gdk_color_parse("#f0f0f0", &current_theme->fg_color);    /* Light gray text */
  gdk_color_parse("#2d5aa0", &current_theme->selected_bg); /* Blue selection */
  gdk_color_parse("#ffffff",
                  &current_theme->selected_fg); /* White selected text */
  gdk_color_parse("#2a2a2a", &current_theme->input_bg); /* Darker gray input */
  gdk_color_parse("#f0f0f0", &current_theme->input_fg); /* Light input text */
  gdk_color_parse("#252525", &current_theme->nicklist_bg); /* Dark nicklist */
  gdk_color_parse("#cccccc",
                  &current_theme->nicklist_fg); /* Light nicklist text */
  gdk_color_parse("#1e1e1e", &current_theme->chanlist_bg); /* Same as main bg */
  gdk_color_parse("#f0f0f0",
                  &current_theme->chanlist_fg);        /* Light chanlist text */
  gdk_color_parse("#1e1e1e", &current_theme->tree_bg); /* Same as main bg */
  gdk_color_parse("#f0f0f0", &current_theme->tree_fg); /* Light tree text */
  gdk_color_parse("#666666", &current_theme->away_user); /* Dim gray for away */
  gdk_color_parse("#404040",
                  &current_theme->separator); /* Medium gray separator */

  /* Dark theme text colors - vibrant but readable on dark background */
  gdk_color_parse("#ffffff", &current_theme->text_colors[0]); /* White */
  gdk_color_parse("#ff6b6b", &current_theme->text_colors[1]); /* Coral red */
  gdk_color_parse("#51cf66", &current_theme->text_colors[2]); /* Mint green */
  gdk_color_parse("#ffd43b",
                  &current_theme->text_colors[3]); /* Bright yellow */
  gdk_color_parse("#74c0fc", &current_theme->text_colors[4]); /* Sky blue */
  gdk_color_parse("#da77f2", &current_theme->text_colors[5]); /* Light purple */
  gdk_color_parse("#4dabf7", &current_theme->text_colors[6]); /* Dodger blue */
  gdk_color_parse("#f8f9fa", &current_theme->text_colors[7]); /* Off white */
  gdk_color_parse("#ff922b", &current_theme->text_colors[8]); /* Orange */
  gdk_color_parse("#868e96", &current_theme->text_colors[9]); /* Gray */
  gdk_color_parse("#20c997", &current_theme->text_colors[10]); /* Teal */
  gdk_color_parse("#00d2d3", &current_theme->text_colors[11]); /* Cyan */
  gdk_color_parse("#339af0", &current_theme->text_colors[12]); /* Blue */
  gdk_color_parse("#e64980", &current_theme->text_colors[13]); /* Pink */
  gdk_color_parse("#adb5bd", &current_theme->text_colors[14]); /* Light gray */
  gdk_color_parse("#ced4da",
                  &current_theme->text_colors[15]); /* Lighter gray */
  /* Extended colors for dark theme */
  gdk_color_parse("#c92a2a", &current_theme->text_colors[16]); /* Dark red */
  gdk_color_parse("#a61e4d", &current_theme->text_colors[17]); /* Maroon */
  gdk_color_parse("#862e9c", &current_theme->text_colors[18]); /* Purple */
  gdk_color_parse("#5f3dc4", &current_theme->text_colors[19]); /* Indigo */
  gdk_color_parse("#1864ab", &current_theme->text_colors[20]); /* Dark blue */
  gdk_color_parse("#0b7285", &current_theme->text_colors[21]); /* Dark teal */
  gdk_color_parse("#087f5b", &current_theme->text_colors[22]); /* Dark green */
  gdk_color_parse("#2b8a3e", &current_theme->text_colors[23]); /* Green */
  gdk_color_parse("#5c940d", &current_theme->text_colors[24]); /* Lime */
  gdk_color_parse("#e67700", &current_theme->text_colors[25]); /* Dark orange */
  gdk_color_parse("#d9480f", &current_theme->text_colors[26]); /* Red orange */
  gdk_color_parse("#495057", &current_theme->text_colors[27]); /* Dark gray */
  gdk_color_parse("#868e96", &current_theme->text_colors[28]); /* Medium gray */
  gdk_color_parse("#adb5bd", &current_theme->text_colors[29]); /* Light gray */
  gdk_color_parse("#ced4da",
                  &current_theme->text_colors[30]); /* Very light gray */
  gdk_color_parse("#f8f9fa", &current_theme->text_colors[31]); /* Off white */

  /* Dark theme fonts */
  g_free(current_theme->font_normal);
  g_free(current_theme->font_bold);
  g_free(current_theme->font_italic);
  current_theme->font_normal = g_strdup("Monospace 10");
  current_theme->font_bold = g_strdup("Monospace Bold 10");
  current_theme->font_italic = g_strdup("Monospace Italic 10");

  /* Ensure global palette reflects this theme (replaces colors.conf) */
  theme_sync_global_palette(current_theme, NULL);

  /* Don't apply theme immediately - defer until GTK is ready */
  g_message("Theme: Default dark theme created, will apply when GTK is ready");
}

int theme_load_from_file(const char *filename) {
  char *full_path;
  GIOChannel *io;
  GIOStatus status;
  char *line;
  gsize len;
  HexChatTheme *theme;

  g_message("Theme: Loading theme from file: %s", filename);

  if (!filename || !*filename) {
    g_warning("Theme: Invalid filename");
    return 0;
  }

  /* Check if filename is already a full path or just a filename */
  if (g_str_has_prefix(filename, theme_config_dir)) {
    /* It's already a full path within our theme directory */
    full_path = g_strdup(filename);
  } else if (g_path_is_absolute(filename)) {
    /* It's an absolute path elsewhere */
    full_path = g_strdup(filename);
  } else {
    /* It's just a filename, build full path */
    full_path = g_build_filename(theme_config_dir, filename, NULL);
  }

  if (!full_path) {
    g_warning("Theme: Failed to build full path for %s", filename);
    return 0;
  }

  g_message("Theme: Full path: %s", full_path);

  io = g_io_channel_new_file(full_path, "r", NULL);
  if (!io) {
    g_warning("Theme: Failed to open file: %s", full_path);
    g_free(full_path);
    return 0;
  }

  theme = theme_new();
  if (!theme) {
    g_warning("Theme: Failed to create new theme");
    g_io_channel_unref(io);
    g_free(full_path);
    return 0;
  }

  while ((status = g_io_channel_read_line(io, &line, &len, NULL, NULL)) ==
         G_IO_STATUS_NORMAL) {
    if (!theme_parse_line(theme, line)) {
      g_warning("Theme: Failed to parse line: %s", line);
      /* Invalid line, skip */
    }
    g_free(line);
  }

  g_io_channel_unref(io);
  g_free(full_path);

  if (current_theme) {
    theme_free(current_theme);
  }
  current_theme = theme;

  /* Make theme palette authoritative for xtext + styles */
  theme_sync_global_palette(current_theme, parent_window);

  g_message("Theme: Theme loaded successfully, applying");
  theme_apply();
  return 1;
}

int theme_save_to_file(const char *filename) {
  /* TODO: Implement theme saving */
  return 0;
}

static int theme_parse_line(HexChatTheme *theme, const char *line) {
  char *key, *value;
  char *eq_pos;

  if (!line || !*line || *line == '#') {
    return 1; /* Comment or empty line */
  }

  eq_pos = strchr(line, '=');
  if (!eq_pos) {
    return 0; /* Invalid format */
  }

  key = g_strndup(line, eq_pos - line);
  value = g_strdup(eq_pos + 1);

  /* Trim whitespace */
  g_strstrip(key);
  g_strstrip(value);

  /* Parse key-value pairs */
  if (strcmp(key, "name") == 0) {
    g_free(theme->name);
    theme->name = g_strdup(value);
  } else if (strcmp(key, "author") == 0) {
    g_free(theme->author);
    theme->author = g_strdup(value);
  } else if (strcmp(key, "description") == 0) {
    g_free(theme->description);
    theme->description = g_strdup(value);
  } else if (strcmp(key, "version") == 0) {
    g_free(theme->version);
    theme->version = g_strdup(value);
  } else if (strcmp(key, "bg_color") == 0) {
    gdk_color_parse(value, &theme->bg_color);
  } else if (strcmp(key, "fg_color") == 0) {
    gdk_color_parse(value, &theme->fg_color);
  } else if (strcmp(key, "selected_bg") == 0) {
    gdk_color_parse(value, &theme->selected_bg);
  } else if (strcmp(key, "selected_fg") == 0) {
    gdk_color_parse(value, &theme->selected_fg);
  } else if (strcmp(key, "input_bg") == 0) {
    gdk_color_parse(value, &theme->input_bg);
  } else if (strcmp(key, "input_fg") == 0) {
    gdk_color_parse(value, &theme->input_fg);
  } else if (strcmp(key, "nicklist_bg") == 0) {
    gdk_color_parse(value, &theme->nicklist_bg);
  } else if (strcmp(key, "nicklist_fg") == 0) {
    gdk_color_parse(value, &theme->nicklist_fg);
  } else if (strcmp(key, "chanlist_bg") == 0) {
    gdk_color_parse(value, &theme->chanlist_bg);
  } else if (strcmp(key, "chanlist_fg") == 0) {
    gdk_color_parse(value, &theme->chanlist_fg);
  } else if (strcmp(key, "tree_bg") == 0) {
    gdk_color_parse(value, &theme->tree_bg);
  } else if (strcmp(key, "tree_fg") == 0) {
    gdk_color_parse(value, &theme->tree_fg);
  } else if (strcmp(key, "away_user") == 0) {
    gdk_color_parse(value, &theme->away_user);
  } else if (strcmp(key, "separator") == 0) {
    gdk_color_parse(value, &theme->separator);
  } else if (g_str_has_prefix(key, "text_color_")) {
    /* Parse text colors: text_color_0, text_color_1, etc. */
    int color_index = atoi(key + 11); /* Skip "text_color_" */
    if (color_index >= 0 && color_index < 32) {
      gdk_color_parse(value, &theme->text_colors[color_index]);
    }
  }

  g_free(key);
  g_free(value);
  return 1;
}

void theme_apply(void) {
  g_message("Theme: Applying theme");

  /* Check if GTK is initialized and we have a main window */
  if (!gtk_settings_get_default()) {
    g_message("Theme: GTK not ready, deferring theme application");
    theme_pending_apply = TRUE;
    return;
  }

  if (!current_theme) {
    g_warning("Theme: No current theme to apply");
    return;
  }

  g_message("Theme: Applying theme '%s' with bg_color=#%02x%02x%02x "
            "fg_color=#%02x%02x%02x",
            current_theme->name, current_theme->bg_color.red >> 8,
            current_theme->bg_color.green >> 8,
            current_theme->bg_color.blue >> 8, current_theme->fg_color.red >> 8,
            current_theme->fg_color.green >> 8,
            current_theme->fg_color.blue >> 8);

  /* Keep global palette in sync (mg_update_xtext and styles use colors[]) */
  theme_sync_global_palette(current_theme, parent_window);

  /* Apply theme to main window */
  if (parent_window) {
    gtk_widget_modify_bg(parent_window, GTK_STATE_NORMAL,
                         &current_theme->bg_color);
    g_message("Theme: Applied background color to main window");
  }

  /* Apply theme to all session widgets */
  GSList *list = sess_list;
  while (list) {
    session *sess = (session *)list->data;
    if (sess && sess->gui) {
      session_gui *gui = sess->gui;

      /* Chat area (xtext) */
      if (gui->xtext) {
        /* Update xtext using the standard HexChat function which uses the
           global colors[] array we just synced */
        mg_update_xtext(gui->xtext);

        if (current_theme->font_normal)
          gtk_xtext_set_font(GTK_XTEXT(gui->xtext), current_theme->font_normal);

        /* Create a new style with theme colors for the widget itself */
        GtkStyle *new_style = gtk_style_copy(gtk_widget_get_style(gui->xtext));
        new_style->bg[GTK_STATE_NORMAL] = current_theme->bg_color;
        new_style->fg[GTK_STATE_NORMAL] = current_theme->fg_color;
        new_style->base[GTK_STATE_NORMAL] = current_theme->bg_color;
        new_style->text[GTK_STATE_NORMAL] = current_theme->fg_color;
        gtk_widget_set_style(gui->xtext, new_style);
        g_object_unref(new_style);

        gtk_widget_queue_draw(gui->xtext);
      }

      /* Input box */
      if (gui->input_box) {
        gtk_widget_modify_bg(gui->input_box, GTK_STATE_NORMAL,
                             &current_theme->input_bg);
        gtk_widget_modify_fg(gui->input_box, GTK_STATE_NORMAL,
                             &current_theme->input_fg);
        gtk_widget_modify_base(gui->input_box, GTK_STATE_NORMAL,
                               &current_theme->input_bg);
        gtk_widget_modify_text(gui->input_box, GTK_STATE_NORMAL,
                               &current_theme->input_fg);
        gtk_widget_queue_draw(gui->input_box);
      }

      /* User tree (user list) */
      if (gui->user_tree) {
        gtk_widget_modify_bg(gui->user_tree, GTK_STATE_NORMAL,
                             &current_theme->nicklist_bg);
        gtk_widget_modify_fg(gui->user_tree, GTK_STATE_NORMAL,
                             &current_theme->nicklist_fg);
        gtk_widget_modify_base(gui->user_tree, GTK_STATE_NORMAL,
                               &current_theme->nicklist_bg);
        gtk_widget_modify_text(gui->user_tree, GTK_STATE_NORMAL,
                               &current_theme->nicklist_fg);
        gtk_widget_queue_draw(gui->user_tree);
      }

      /* Topic entry */
      if (gui->topic_entry) {
        gtk_widget_modify_bg(gui->topic_entry, GTK_STATE_NORMAL,
                             &current_theme->input_bg);
        gtk_widget_modify_fg(gui->topic_entry, GTK_STATE_NORMAL,
                             &current_theme->input_fg);
        gtk_widget_modify_base(gui->topic_entry, GTK_STATE_NORMAL,
                               &current_theme->input_bg);
        gtk_widget_modify_text(gui->topic_entry, GTK_STATE_NORMAL,
                               &current_theme->input_fg);
        gtk_widget_queue_draw(gui->topic_entry);
      }

      /* Main table (background) */
      if (gui->main_table) {
        gtk_widget_modify_bg(gui->main_table, GTK_STATE_NORMAL,
                             &current_theme->bg_color);
        gtk_widget_queue_draw(gui->main_table);
      }

      /* User box */
      if (gui->user_box) {
        gtk_widget_modify_bg(gui->user_box, GTK_STATE_NORMAL,
                             &current_theme->nicklist_bg);
        gtk_widget_queue_draw(gui->user_box);
      }

      /* Notebook (tabs) */
      if (gui->note_book) {
        gtk_widget_modify_bg(gui->note_book, GTK_STATE_NORMAL,
                             &current_theme->bg_color);
        gtk_widget_modify_fg(gui->note_book, GTK_STATE_NORMAL,
                             &current_theme->fg_color);
        gtk_widget_queue_draw(gui->note_book);
      }

      /* Topic bar */
      if (gui->topic_bar) {
        gtk_widget_modify_bg(gui->topic_bar, GTK_STATE_NORMAL,
                             &current_theme->input_bg);
        gtk_widget_queue_draw(gui->topic_bar);
      }

      /* Progress bar */
      if (gui->bar) {
        gtk_widget_modify_bg(gui->bar, GTK_STATE_NORMAL,
                             &current_theme->bg_color);
        gtk_widget_modify_fg(gui->bar, GTK_STATE_NORMAL,
                             &current_theme->fg_color);
        gtk_widget_queue_draw(gui->bar);
      }

      /* Nick box */
      if (gui->nick_box) {
        gtk_widget_modify_bg(gui->nick_box, GTK_STATE_NORMAL,
                             &current_theme->nicklist_bg);
        gtk_widget_queue_draw(gui->nick_box);
      }
    }
    list = list->next;
  }

  g_message("Theme: Applied theme to all sessions");

  /* Force a redraw of all widgets */
  if (parent_window) {
    gtk_widget_queue_draw(parent_window);
    g_message("Theme: Queued redraw of main window");
  }

  /* Force immediate processing of all pending updates */
  gdk_window_process_all_updates();
  g_message("Theme: Processed all pending updates");

  g_message("Theme: Theme applied successfully");
}

void theme_apply_pending(void) {
  if (theme_pending_apply) {
    theme_pending_apply = FALSE;
    theme_apply();
  }
}

GSList *theme_get_available_themes(void) {
  GSList *themes = NULL;
  GDir *dir;
  const char *filename;

  dir = g_dir_open(theme_config_dir, 0, NULL);
  if (!dir) {
    return NULL;
  }

  while ((filename = g_dir_read_name(dir))) {
    if (g_str_has_suffix(filename, ".theme")) {
      themes = g_slist_prepend(themes, g_strdup(filename));
    }
  }

  g_dir_close(dir);
  return themes;
}