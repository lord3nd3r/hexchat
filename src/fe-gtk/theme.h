#ifndef HEXCHAT_THEME_H
#define HEXCHAT_THEME_H

#include <gtk/gtk.h>

typedef struct {
    char *name;
    char *author;
    char *description;
    char *version;

    /* Color scheme */
    GdkColor bg_color;           /* Main background */
    GdkColor fg_color;           /* Main foreground/text */
    GdkColor selected_bg;        /* Selected item background */
    GdkColor selected_fg;        /* Selected item foreground */
    GdkColor input_bg;           /* Input box background */
    GdkColor input_fg;           /* Input box foreground */
    GdkColor nicklist_bg;        /* User list background */
    GdkColor nicklist_fg;        /* User list foreground */
    GdkColor chanlist_bg;        /* Channel list background */
    GdkColor chanlist_fg;        /* Channel list foreground */
    GdkColor tree_bg;            /* Tree background */
    GdkColor tree_fg;            /* Tree foreground */
    GdkColor away_user;          /* Away user color */
    GdkColor separator;          /* Separator color */

    /* Text colors for different message types */
    GdkColor text_colors[32];    /* Array for text colors 0-31 */

    /* Fonts */
    char *font_normal;
    char *font_bold;
    char *font_italic;

    /* Additional styling */
    int border_width;
    int padding;
} HexChatTheme;

extern HexChatTheme *current_theme;

/* Theme management functions */
void theme_init(void);
void theme_cleanup(void);
int theme_load_from_file(const char *filename);
int theme_save_to_file(const char *filename);
void theme_apply(void);
void theme_apply_pending(void);
void theme_create_default_dark(void);
GSList *theme_get_available_themes(void);
char *theme_get_config_dir(void);

#endif /* HEXCHAT_THEME_H */