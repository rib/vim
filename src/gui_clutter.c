/* vi:set ts=8 sts=4 sw=4:
 *
 * VIM - Vi IMproved		by Bram Moolenaar
 *
 * Do ":help uganda"  in Vim to read copying and usage conditions.
 * Do ":help credits" in Vim to see a list of people who contributed.
 * See README.txt for an overview of the Vim source code.
 */

/*
 * Copyright © 2010 Intel Coorporation.
 *
 * Authors:
 *  Robert Bragg <robert@sixbynine.org>
 */

#include <clutter/clutter.h>
#include <clutter/x11/clutter-x11.h>
#include <glib.h>

#include "vim.h"

#define DEFAULT_FONT	"Monospace 10"

    void
clip_mch_lose_selection (VimClipboard *cbd)
{

}


    int
clip_mch_own_selection (VimClipboard *cbd)
{

    return OK;
}

    void
clip_mch_request_selection (VimClipboard *cbd)
{
    fprintf (stderr, "clip_mch_request_selection  FIXME\n");
}

    void
clip_mch_set_selection (VimClipboard *cbd)
{
    fprintf (stderr, "clip_mch_set_selection  FIXME\n");
}

    void
gui_mch_add_menu (vimmenu_T *menu, int idx)
{
    fprintf (stderr, "gui_mch_add_menu  FIXME\n");
}

    void
gui_mch_add_menu_item (vimmenu_T *menu, int idx)
{
    fprintf (stderr, "gui_mch_add_menu_item  FIXME\n");
}

    int
gui_mch_adjust_charheight (void)
{
    PangoFontMetrics *metrics;
    int	ascent;
    int descent;

    metrics = pango_context_get_metrics(gui.text_context, gui.norm_font,
				pango_context_get_language(gui.text_context));
    ascent  = pango_font_metrics_get_ascent(metrics);
    descent = pango_font_metrics_get_descent(metrics);

    pango_font_metrics_unref(metrics);

    gui.char_height = (ascent + descent + PANGO_SCALE - 1) / PANGO_SCALE
								+ p_linespace;
    gui.char_ascent = PANGO_PIXELS(ascent + p_linespace * PANGO_SCALE / 2);

    /* A not-positive value of char_height may crash Vim.  Only happens
     * if 'linespace' is negative (which does make sense sometimes). */
    gui.char_ascent = MAX(gui.char_ascent, 0);
    gui.char_height = MAX(gui.char_height, gui.char_ascent + 1);

    return OK;
}

    void
gui_mch_beep (void)
{
    fprintf (stderr, "gui_mch_beep  FIXME\n");
}

    void
gui_mch_clear_all (void)
{
    cogl_clear (&gui.bgcolor, COGL_BUFFER_BIT_COLOR);
}

    void
gui_mch_clear_block (int row1, int col1, int row2, int col2)
{
    fprintf (stderr, "gui_mch_clear_block  FIXME\n");
}

    void
gui_mch_create_scrollbar (scrollbar_T *sb, int orient)
{
    fprintf (stderr, "gui_mch_create_scrollbar  FIXME\n");
}

    void
gui_mch_delete_lines (int row, int num_lines)
{
    fprintf (stderr, "gui_mch_delete_lines  FIXME\n");
}

    void
gui_mch_destroy_menu (vimmenu_T *menu)
{
    fprintf (stderr, "gui_mch_destroy_menu  FIXME\n");
}

    void
gui_mch_destroy_scrollbar (scrollbar_T *sb)
{
    fprintf (stderr, "gui_mch_destroy_scrollbar FIXME\n");
}

    void
gui_mch_draw_hollow_cursor (guicolor_T color)
{
    fprintf (stderr, "gui_mch_draw_hollow_cursor FIXME\n");
}

    void
gui_mch_draw_menubar (void)
{
    fprintf (stderr, "gui_mch_draw_menubar FIXME\n");
}

    void
gui_mch_draw_part_cursor (int w, int h, guicolor_T color)
{
    fprintf (stderr, "gui_mch_draw_part_cursor FIXME\n");
}

    int
gui_clutter_draw_string (int row, int col, char_u *s, int len, int flags)
{
    char_u *conv_buf = NULL; /* result of UTF-8 conversion */
    int	convlen;
    int column_offset = 0;  /* column offset in cells */
    PangoGlyphString *glyphs;
    PangoAttrList *attr_list;
    GList *item_list;
    GList *l;

    char *debug = g_strndup (s, len);
    fprintf (stderr, "gui_clutter_draw_string %s\n", debug);

    if (output_conv.vc_type != CONV_NONE)
    {
	/*
	 * Convert characters from 'encoding' to 'termencoding', which is set
	 * to UTF-8 by gui_mch_init().	did_set_string_option() in option.c
	 * prohibits changing this to something else than UTF-8 if the GUI is
	 * in use.
	 */
	convlen = len;
	conv_buf = string_convert(&output_conv, s, &convlen);
	g_return_val_if_fail(conv_buf != NULL, len);

	s = conv_buf;
	len = convlen;
    }

    /* Safety check: pango crashes when invoked with invalid utf-8
     * characters. */
    if (!utf_valid_string(s, s + len))
    {
	vim_free(conv_buf);
	return len;
    }

    glyphs = pango_glyph_string_new();

    attr_list = pango_attr_list_new();

#define INSERT_PANGO_ATTR(Attribute, AttrList, Start, End)  \
    G_STMT_START{					    \
	PangoAttribute *tmp_attr_;			    \
	tmp_attr_ = (Attribute);			    \
	tmp_attr_->start_index = (Start);		    \
	tmp_attr_->end_index = (End);			    \
	pango_attr_list_insert((AttrList), tmp_attr_);	    \
    }G_STMT_END

    if ((flags & DRAW_BOLD))
	INSERT_PANGO_ATTR(pango_attr_weight_new(PANGO_WEIGHT_BOLD),
			  attr_list, 0, len);
    if (flags & DRAW_ITALIC)
	INSERT_PANGO_ATTR(pango_attr_style_new(PANGO_STYLE_ITALIC),
			  attr_list, 0, len);

#undef INSERT_PANGO_ATTR

    /*
     * Break the text into segments with consistent directional level
     * and shaping engine.	Pure Latin text needs only a single segment,
     * so there's no need to worry about the loop's efficiency.  Better
     * try to optimize elsewhere, e.g. reducing exposes and stuff :)
     */
    item_list = pango_itemize(gui.text_context,
			      (const char *)s, 0, len, attr_list, NULL);

    for (l = item_list; l; l = l->next)
    {
	PangoItem *item = l->data;
	PangoRenderer *renderer;
	int x, y;
	CoglColor color;

	pango_shape((const char *)s + item->offset, item->length,
		    &item->analysis, glyphs);

	x = TEXT_X(col) * PANGO_SCALE;
	y = TEXT_Y(row) * PANGO_SCALE;
	//x = TEXT_X(col);
	//y = TEXT_Y(row);
	renderer = cogl_pango_font_map_get_renderer (gui.font_map);
	
	cogl_color_init_from_4f (&color, 1, 0, 0, 1);
	cogl_pango_renderer_draw_glyphs (renderer,
					 item->analysis.font,
					 &gui.fgcolor,
					 glyphs,
					 x, y);

	pango_item_free(item);

	column_offset += glyphs->num_glyphs;
    }

    pango_attr_list_unref(attr_list);

    vim_free(conv_buf);

    return column_offset;
}

    void
gui_mch_enable_menu (int showit)
{
    fprintf (stderr, "gui_mch_enable_menu FIXME\n");
}

    void
gui_mch_enable_scrollbar (scrollbar_T *sb, int flag)
{
    fprintf (stderr, "gui_mch_enable_scrollbar FIXME\n");
}

    void
gui_mch_exit (int rc)
{
    fprintf (stderr, "gui_mch_exit FIXME\n");
}

    void
gui_mch_flash (int msec)
{
    fprintf (stderr, "gui_mch_flash FIXME\n");
}

    void
gui_mch_flush (void)
{
    fprintf (stderr, "gui_mch_flush FIXME\n");
}

    void
gui_mch_free_font (GuiFont font)
{
    if (font != NOFONT)
	pango_font_description_free(font);
}

    void
gui_mch_free_fontset (GuiFontset fontset)
{
    fprintf (stderr, "gui_mch_free_fontset FIXME\n");
}

    guicolor_T
gui_mch_get_color (char_u *name)
{
    PangoColor pango_color;
    long color;

    if (!gui.in_use)		/* can't do this when GUI not running */
	return INVALCOLOR;

    if (pango_color_parse (&pango_color, name))
    {
	color = (pango_color.red >> 8) | \
		((pango_color.green >> 8) << 8) | \
		((pango_color.blue >> 8) << 16);
    }
    else
	return INVALCOLOR;
}

    Display *
gui_mch_get_display (void)
{
    return clutter_x11_get_default_display ();
}

    GuiFontset
gui_mch_get_fontset (char_u *name, int report_error, int fixed_width)
{
    fprintf (stderr, "gui_mch_get_fontset FIXME\n");
}

    GuiFont
gui_mch_get_font (char_u *name, int report_error)
{
    PangoFontDescription    *font;

    fprintf (stderr, "\n\nXXXXX gui_mch_get_font\n\n");

    /* can't do this when GUI is not running */
    if (!gui.in_use || name == NULL)
	return NULL;

    if (output_conv.vc_type != CONV_NONE)
    {
	char_u *buf;

	buf = string_convert(&output_conv, name, NULL);
	if (buf != NULL)
	{
	    font = pango_font_description_from_string((const char *)buf);
	    vim_free(buf);
	}
	else
	    font = NULL;
    }
    else
	font = pango_font_description_from_string((const char *)name);

    if (font != NULL)
    {
	PangoFont *real_font;

	/* pango_context_load_font() bails out if no font size is set */
	if (pango_font_description_get_size(font) <= 0)
	    pango_font_description_set_size(font, 10 * PANGO_SCALE);

	real_font = pango_context_load_font(gui.text_context, font);

	if (real_font == NULL)
	{
	    pango_font_description_free(font);
	    font = NULL;
	}
	else
	    g_object_unref(real_font);
    }

    if (font == NULL)
    {
	if (report_error)
	    EMSG2(_((char *)e_font), name);
	return NULL;
    }

    /*
     * The fixed-width check has been disabled for GTK+ 2.  Rationale:
     *
     *	 - The check tends to report false positives, particularly
     *	   in non-Latin locales or with old X fonts.
     *	 - Thanks to our fixed-width hack in gui_gtk2_draw_string(),
     *	   GTK+ 2 Vim is actually capable of displaying variable width
     *	   fonts.  Those will just be spaced out like in AA xterm.
     *	 - Failing here for the default font causes GUI startup to fail
     *	   even with wiped out configuration files.
     *	 - The font dialog displays all fonts unfiltered, and it's rather
     *	   annoying if 95% of the listed fonts produce an error message.
     */
# if 0
    {
	/* Check that this is a mono-spaced font.  Naturally, this is a bit
	 * hackish -- fixed-width isn't really suitable for i18n text :/ */
	PangoLayout	*layout;
	unsigned int	i;
	int		last_width   = -1;
	const char	test_chars[] = { 'W', 'i', ',', 'x' }; /* arbitrary */

	layout = pango_layout_new(gui.text_context);
	pango_layout_set_font_description(layout, font);

	for (i = 0; i < G_N_ELEMENTS(test_chars); ++i)
	{
	    int width;

	    pango_layout_set_text(layout, &test_chars[i], 1);
	    pango_layout_get_size(layout, &width, NULL);

	    if (last_width >= 0 && width != last_width)
	    {
		pango_font_description_free(font);
		font = NULL;
		break;
	    }

	    last_width = width;
	}

	g_object_unref(layout);
    }

    if (font == NULL && report_error)
	EMSG2(_(e_fontwidth), name);
# endif

    return font;

}

    char_u *
gui_mch_get_fontname (GuiFont font, char_u *name)
{
    fprintf (stderr, "gui_mch_get_fontname FIXME\n");
}

    void
gui_mch_getmouse (int *x, int *y)
{
    fprintf (stderr, "gui_mch_getmouse FIXME\n");
}

    long_u
gui_mch_get_rgb (guicolor_T pixel)
{
    fprintf (stderr, "gui_mch_get_rgb FIXME\n");
}

    void
gui_mch_get_screen_dimensions (int *screen_w, int *screen_h)
{
    fprintf (stderr, "gui_mch_get_screen_dimensions FIXME\n");
}

    int
gui_mch_get_winpos (int *x, int *y)
{
    fprintf (stderr, "gui_mch_get_winpos FIXME\n");
    return OK;
}

    int
gui_mch_haskey (char_u *name)
{
    fprintf (stderr, "gui_mch_haskey FIXME\n");
    return OK;
}

    void
gui_mch_iconify (void)
{
    fprintf (stderr, "gui_mch_iconify FIXME\n");
}

    static void
paint_cb (ClutterActor *actor,
	  void *user_data)
{
    update_screen (0);

    /* We simply want to issue a redraw of everything, and gui_redraw
     * handles clamping of the width and height arguments. */
    gui_redraw (0, 0, G_MAXINT, G_MAXINT);
}

    static void
resize_notify_cb (GObject *stage,
		  GParamSpec *pspec,
		  void *user_data)
{
    float width, height;
    clutter_actor_get_size (CLUTTER_ACTOR (stage), &width, &height);
    fprintf (stderr, "resize_notify_cb w=%f, h=%f\n", width, height);
    gui_resize_shell (width, height);
    clutter_actor_queue_redraw (CLUTTER_ACTOR (stage));
}

    int
gui_mch_init (void)
{
    ClutterActor *stage;
    float resolution;

    /* Set the human-readable application name */
    g_set_application_name("Vim");

    /*
     * Force UTF-8 output no matter what the value of 'encoding' is.
     * did_set_string_option() in option.c prohibits changing 'termencoding'
     * to something else than UTF-8 if the GUI is in use.
     */
    set_option_value((char_u *)"termencoding", 0L, (char_u *)"utf-8", 0);

    stage = clutter_stage_new ();
    gui.stage = stage;

    g_signal_connect_after (stage, "paint",
			    G_CALLBACK (paint_cb), NULL);
    g_signal_connect (stage, "notify::width",
		      G_CALLBACK (resize_notify_cb), NULL);
    g_signal_connect (stage, "notify::height",
		      G_CALLBACK (resize_notify_cb), NULL);

    clutter_stage_set_user_resizable (CLUTTER_STAGE (stage), TRUE);
    /* 
     * Create the PangoContext used for drawing all text.
     */

    gui.font_map = COGL_PANGO_FONT_MAP (cogl_pango_font_map_new());

    /*  Note: in Clutter 1.4 this will be deprecated and the
     *  clutter_settings API should be used instead. */
    resolution = clutter_backend_get_resolution (clutter_get_default_backend ());

    cogl_pango_font_map_set_resolution (gui.font_map, resolution);
    cogl_pango_font_map_set_use_mipmapping (gui.font_map, TRUE);

    gui.text_context = cogl_pango_font_map_create_context (gui.font_map);

    return OK;
}

    int
gui_mch_init_check (void)
{
    if (setlocale (LC_ALL, "") == NULL)
	g_warning ("Locale not supported by C library.\n"
		   "Using the fallback 'C' locale.");

    if (clutter_init (NULL, NULL) != CLUTTER_INIT_SUCCESS)
    {
	gui.dying = TRUE;
	EMSG(_((char *)e_clutter_init));
	return FAIL;
    }

    return OK;
}

/*
 * Check if a given font is a CJK font. This is done in a very crude manner. It
 * just see if U+04E00 for zh and ja and U+AC00 for ko are covered in a given
 * font. Consequently, this function cannot  be used as a general purpose check
 * for CJK-ness for which fontconfig APIs should be used.  This is only used by
 * gui_mch_init_font() to deal with 'CJK fixed width fonts'.
 */
    static int
is_cjk_font(PangoFontDescription *font_desc)
{
    static const char * const cjk_langs[] =
	{"zh_CN", "zh_TW", "zh_HK", "ja", "ko"};

    PangoFont	*font;
    unsigned	i;
    int		is_cjk = FALSE;

    font = pango_context_load_font(gui.text_context, font_desc);

    if (font == NULL)
	return FALSE;

    for (i = 0; !is_cjk && i < G_N_ELEMENTS(cjk_langs); ++i)
    {
	PangoCoverage	*coverage;
	gunichar	uc;

	coverage = pango_font_get_coverage(
		font, pango_language_from_string(cjk_langs[i]));

	if (coverage != NULL)
	{
	    uc = (cjk_langs[i][0] == 'k') ? 0xAC00 : 0x4E00;
	    is_cjk = (pango_coverage_get(coverage, uc) == PANGO_COVERAGE_EXACT);
	    pango_coverage_unref(coverage);
	}
    }

    g_object_unref(font);

    return is_cjk;
}

static PangoEngineShape *default_shape_engine = NULL;

/*
 * Create a map from ASCII characters in the range [32,126] to glyphs
 * of the current font.  This is used by gui_clutter_draw_string() to skip
 * the itemize and shaping process for the most common case.
 */
    static void
ascii_glyph_table_init(void)
{
    char_u	    ascii_chars[128];
    PangoAttrList   *attr_list;
    GList	    *item_list;
    int		    i;

    if (gui.ascii_glyphs != NULL)
	pango_glyph_string_free(gui.ascii_glyphs);
    if (gui.ascii_font != NULL)
	g_object_unref(gui.ascii_font);

    gui.ascii_glyphs = NULL;
    gui.ascii_font   = NULL;

    /* For safety, fill in question marks for the control characters. */
    for (i = 0; i < 32; ++i)
	ascii_chars[i] = '?';
    for (; i < 127; ++i)
	ascii_chars[i] = i;
    ascii_chars[i] = '?';

    attr_list = pango_attr_list_new();
    item_list = pango_itemize(gui.text_context, (const char *)ascii_chars,
			      0, sizeof(ascii_chars), attr_list, NULL);

    if (item_list != NULL && item_list->next == NULL) /* play safe */
    {
	PangoItem   *item;
	int	    width;

	item  = (PangoItem *)item_list->data;
	width = gui.char_width * PANGO_SCALE;

	/* Remember the shape engine used for ASCII. */
	default_shape_engine = item->analysis.shape_engine;

	gui.ascii_font = item->analysis.font;
	g_object_ref(gui.ascii_font);

	gui.ascii_glyphs = pango_glyph_string_new();

	pango_shape((const char *)ascii_chars, sizeof(ascii_chars),
		    &item->analysis, gui.ascii_glyphs);

	g_return_if_fail(gui.ascii_glyphs->num_glyphs == sizeof(ascii_chars));

	for (i = 0; i < gui.ascii_glyphs->num_glyphs; ++i)
	{
	    PangoGlyphGeometry *geom;

	    geom = &gui.ascii_glyphs->glyphs[i].geometry;
	    geom->x_offset += MAX(0, width - geom->width) / 2;
	    geom->width = width;
	}
    }

    g_list_foreach(item_list, (GFunc)&pango_item_free, NULL);
    g_list_free(item_list);
    pango_attr_list_unref(attr_list);
}

    int
gui_mch_init_font (char_u *font_name, int fontset)
{
    PangoFontDescription    *font_desc;
    PangoLayout		    *layout;
    int			    width;

    /* If font_name is NULL, this means to use the default, which should
     * be present on all proper Pango/fontconfig installations. */
    if (font_name == NULL)
	font_name = (char_u *)DEFAULT_FONT;

    font_desc = gui_mch_get_font(font_name, FALSE);

    if (font_desc == NULL)
	return FAIL;

    gui_mch_free_font(gui.norm_font);
    gui.norm_font = font_desc;

    pango_context_set_font_description(gui.text_context, font_desc);

    layout = pango_layout_new(gui.text_context);
    pango_layout_set_text(layout, "MW", 2);
    pango_layout_get_size(layout, &width, NULL);
    /*
     * Set char_width to half the width obtained from pango_layout_get_size()
     * for CJK fixed_width/bi-width fonts.  An unpatched version of Xft leads
     * Pango to use the same width for both non-CJK characters (e.g. Latin
     * letters and numbers) and CJK characters.  This results in 's p a c e d
     * o u t' rendering when a CJK 'fixed width' font is used. To work around
     * that, divide the width returned by Pango by 2 if cjk_width is equal to
     * width for CJK fonts.
     *
     * For related bugs, see:
     * http://bugzilla.gnome.org/show_bug.cgi?id=106618
     * http://bugzilla.gnome.org/show_bug.cgi?id=106624
     *
     * With this, for all four of the following cases, Vim works fine:
     *	   guifont=CJK_fixed_width_font
     *	   guifont=Non_CJK_fixed_font
     *	   guifont=Non_CJK_fixed_font,CJK_Fixed_font
     *	   guifont=Non_CJK_fixed_font guifontwide=CJK_fixed_font
     */
    if (is_cjk_font(gui.norm_font))
    {
	int cjk_width;

	/* Measure the text extent of U+4E00 and U+4E8C */
	pango_layout_set_text(layout, "\344\270\200\344\272\214", -1);
	pango_layout_get_size(layout, &cjk_width, NULL);

	if (width == cjk_width)  /* Xft not patched */
	    width /= 2;
    }
    g_object_unref(layout);

    gui.char_width = (width / 2 + PANGO_SCALE - 1) / PANGO_SCALE;

    /* A zero width may cause a crash.	Happens for semi-invalid fontsets. */
    if (gui.char_width <= 0)
	gui.char_width = 8;

    gui_mch_adjust_charheight();

    /* Set the fontname, which will be used for information purposes */
    hl_set_font_name(font_name);

    ascii_glyph_table_init();

    /* Avoid unnecessary overhead if 'guifontwide' is equal to 'guifont'. */
    if (gui.wide_font != NULL
	&& pango_font_description_equal(gui.norm_font, gui.wide_font))
    {
	pango_font_description_free(gui.wide_font);
	gui.wide_font = NULL;
    }

    /* Preserve the logical dimensions of the screen. */
    /* update_window_manager_hints(0, 0); */

    return OK;
}

    void
gui_mch_insert_lines (int row, int num_lines)
{
    fprintf (stderr, "gui_mch_insert_lines FIXME\n");
}

    void
gui_mch_invert_rectangle (int r, int c, int nr, int nc)
{
    fprintf (stderr, "gui_mch_invert_rectangle FIXME\n");
}

    void
gui_mch_menu_grey (vimmenu_T *menu, int grey)
{
    fprintf (stderr, "gui_mch_menu_grey FIXME\n");
}

    void
gui_mch_menu_hidden (vimmenu_T *menu, int hidden)
{
    fprintf (stderr, "gui_mch_menu_hidden FIXME\n");
}

    void
gui_mch_new_colors (void)
{
    fprintf (stderr, "gui_mch_new_colors FIXME\n");
}

    int
gui_mch_open (void)
{
    clutter_actor_show (gui.stage);
    return OK;
}

    void
gui_mch_prepare (int *argc, char **argv)
{
    fprintf (stderr, "gui_mch_prepare FIXME\n");
}

    static void
init_cogl_color_from_guicolor_T (CoglColor *color, guicolor_T guicolor)
{
    guint8 r = guicolor & 0xff;
    guint8 g = (guicolor & 0xff00) >> 8;
    guint8 b = (guicolor & 0xff0000) >> 16;

    cogl_color_set_from_4ub (color, r, g, b, 0xff);
}

    void
gui_mch_set_bg_color (guicolor_T color)
{
    init_cogl_color_from_guicolor_T (&gui.bgcolor, color);
}

    void
gui_mch_set_blinking (long waittime, long on, long off)
{
    fprintf (stderr, "gui_mch_set_blinking FIXME\n");
}

    void
gui_mch_set_fg_color (guicolor_T color)
{
    init_cogl_color_from_guicolor_T (&gui.fgcolor, color);
}

    void
gui_mch_set_font (GuiFont font)
{
    fprintf (stderr, "gui_mch_set_font FIXME\n");
}

    void
gui_mch_set_fontset (GuiFontset fontset)
{
    fprintf (stderr, "gui_mch_set_fontset FIXME\n");
}

    void
gui_mch_set_foreground (void)
{
    fprintf (stderr, "gui_mch_set_foreground FIXME\n");
}

    void
gui_mch_setmouse (int x, int y)
{
    fprintf (stderr, "gui_mch_setmouse FIXME\n");
}

    void
gui_mch_set_scrollbar_pos (scrollbar_T *sb, int x, int y, int w, int h)
{
    fprintf (stderr, "gui_mch_set_scrollbar_pos FIXME\n");
}

    void
gui_mch_set_scrollbar_thumb (scrollbar_T *sb, long val, long size, long max)
{
    fprintf (stderr, "gui_mch_set_scrollbar_thumb FIXME\n");
}

    void
gui_mch_set_shellsize (int width, int height, int min_width, int min_height, int base_width, int base_height, int direction)
{
    fprintf (stderr, "gui_mch_set_shellsize FIXME\n");
}

    void
gui_mch_set_sp_color (guicolor_T color)
{
    fprintf (stderr, "gui_mch_set_sp_color FIXME\n");
}

    void
gui_mch_set_text_area_pos (int x, int y, int w, int h)
{
    fprintf (stderr, "gui_mch_set_text_area_pos FIXME\n");
}

    void
gui_mch_set_winpos (int x, int y)
{
    fprintf (stderr, "gui_mch_set_winpos FIXME\n");
}

    void
gui_mch_show_popupmenu (vimmenu_T *menu)
{
    fprintf (stderr, "gui_mch_show_popupmenu FIXME\n");
}

    void
gui_mch_start_blink (void)
{
    fprintf (stderr, "gui_mch_start_blink FIXME\n");
}

    void
gui_mch_stop_blink (void)
{
    fprintf (stderr, "gui_mch_stop_blink FIXME\n");
}

    void
gui_mch_toggle_tearoffs (int enable)
{
    fprintf (stderr, "gui_mch_toggle_tearoffs FIXME\n");
}

    void
gui_mch_update_tabline (void)
{
    fprintf (stderr, "gui_mch_update_tabline FIXME\n");
}

    void
gui_mch_update (void)
{
    fprintf (stderr, "gui_mch_update FIXME\n");
}

    int
gui_mch_wait_for_chars (long wtime)
{
    fprintf (stderr, "gui_mch_wait_for_chars FIXME\n");
    return OK;
}

