void clip_mch_lose_selection __ARGS((VimClipboard *cbd));
int clip_mch_own_selection __ARGS((VimClipboard *cbd));
void clip_mch_request_selection __ARGS((VimClipboard *cbd));
void clip_mch_set_selection __ARGS((VimClipboard *cbd));
void gui_mch_add_menu __ARGS((vimmenu_T *menu, int idx));
void gui_mch_add_menu_item __ARGS((vimmenu_T *menu, int idx));
void gui_mch_add_menu_item __ARGS((vimmenu_T *menu, int idx));
int gui_mch_adjust_charheight __ARGS((void));
void gui_mch_beep __ARGS((void));
void gui_mch_clear_all __ARGS((void));
void gui_mch_clear_block __ARGS((int row1, int col1, int row2, int col2));
void gui_mch_create_scrollbar __ARGS((scrollbar_T *sb, int orient));
void gui_mch_delete_lines __ARGS((int row, int num_lines));
void gui_mch_destroy_menu __ARGS((vimmenu_T *menu));
void gui_mch_destroy_scrollbar __ARGS((scrollbar_T *sb));
void gui_mch_draw_hollow_cursor __ARGS((guicolor_T color));
void gui_mch_draw_menubar __ARGS((void));
void gui_mch_draw_part_cursor __ARGS((int w, int h, guicolor_T color));
void gui_mch_draw_string __ARGS((int row, int col, char_u *s, int len, int flags));
void gui_mch_enable_menu __ARGS((int showit));
void gui_mch_enable_scrollbar __ARGS((scrollbar_T *sb, int flag));
void gui_mch_exit __ARGS((int rc));
void gui_mch_flash __ARGS((int msec));
void gui_mch_flush __ARGS((void));
void gui_mch_free_font __ARGS((GuiFont font));
void gui_mch_free_fontset __ARGS((GuiFontset fontset));
guicolor_T gui_mch_get_color __ARGS((char_u *name));
Display *gui_mch_get_display __ARGS((void));
GuiFontset gui_mch_get_fontset __ARGS((char_u *name, int report_error, int fixed_width));
GuiFont gui_mch_get_font __ARGS((char_u *name, int report_error));
char_u *gui_mch_get_fontname __ARGS((GuiFont font, char_u *name));
char_u *gui_mch_get_fontname __ARGS((GuiFont font, char_u *name));
void gui_mch_getmouse __ARGS((int *x, int *y));
long_u gui_mch_get_rgb __ARGS((guicolor_T pixel));
void gui_mch_get_screen_dimensions __ARGS((int *screen_w, int *screen_h));
int gui_mch_get_winpos __ARGS((int *x, int *y));
int gui_mch_haskey __ARGS((char_u *name));
void gui_mch_iconify __ARGS((void));
int gui_mch_init_check __ARGS((void));
int gui_mch_init __ARGS((void));
int gui_mch_init_font __ARGS((char_u *font_name, int fontset));
int gui_mch_init_check __ARGS((void));
int gui_mch_init_font __ARGS((char_u *font_name, int fontset));
void gui_mch_insert_lines __ARGS((int row, int num_lines));
void gui_mch_invert_rectangle __ARGS((int r, int c, int nr, int nc));
void gui_mch_menu_grey __ARGS((vimmenu_T *menu, int grey));
void gui_mch_menu_hidden __ARGS((vimmenu_T *menu, int hidden));
void gui_mch_new_colors __ARGS((void));
int gui_mch_open __ARGS((void));
void gui_mch_prepare __ARGS((int *argc, char **argv));
void gui_mch_set_bg_color __ARGS((guicolor_T color));
void gui_mch_set_blinking __ARGS((long waittime, long on, long off));
void gui_mch_set_fg_color __ARGS((guicolor_T color));
void gui_mch_set_font __ARGS((GuiFont font));
void gui_mch_set_fontset __ARGS((GuiFontset fontset));
void gui_mch_set_foreground __ARGS((void));
void gui_mch_setmouse __ARGS((int x, int y));
void gui_mch_set_scrollbar_pos __ARGS((scrollbar_T *sb, int x, int y, int w, int h));
void gui_mch_set_scrollbar_thumb __ARGS((scrollbar_T *sb, long val, long size, long max));
void gui_mch_set_shellsize __ARGS((int width, int height, int min_width, int min_height, int base_width, int base_height, int direction));
void gui_mch_set_sp_color __ARGS((guicolor_T color));
void gui_mch_set_text_area_pos __ARGS((int x, int y, int w, int h));
void gui_mch_set_winpos __ARGS((int x, int y));
void gui_mch_show_popupmenu __ARGS((vimmenu_T *menu));
void gui_mch_start_blink __ARGS((void));
void gui_mch_stop_blink __ARGS((void));
void gui_mch_toggle_tearoffs __ARGS((int enable));
void gui_mch_update_tabline __ARGS((void));
void gui_mch_update __ARGS((void));
int gui_mch_wait_for_chars __ARGS((long wtime));

