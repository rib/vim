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
//    cogl_clear (&gui.bgcolor, COGL_BUFFER_BIT_COLOR);
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
	renderer = cogl_pango_font_map_get_renderer (gui.font_map);
	
	cogl_set_source_color (&gui.fgcolor);
	pango_renderer_draw_glyphs (renderer,
				    item->analysis.font,
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

/*
 * Use the blank mouse pointer or not.
 *
 * hide: TRUE = use blank ptr, FALSE = use parent ptr
 */
    void
gui_mch_mousehide(int hide)
{
    fprintf (stderr, "gui_mch_mousehide FIXME\n");
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
    /*
     * Before redrawing, make sure w_topline is correct, and w_leftcol
     * if lines don't wrap, and w_skipcol if lines wrap.
     */
    update_topline();
    validate_cursor();

#if 0
    if (VIsual_active)
	update_curbuf(INVERTED);/* update inverted part */
    else if (must_redraw)
	update_screen(0);
    else if (redraw_cmdline || clear_cmdline)
	showmode();
#endif

#if 0
    if (VIsual_active)
	update_curbuf(INVERTED);/* update inverted part */
    else
#endif
	update_screen(CLEAR);

    /* FIXME: it seems like it would be nicer if vim had some kind
     * of mch_queue_maketitle() mechansim instead. A default
     * implementation could just set need_maketitle = TRUE, and
     * the clutter backend could queue and idle handler to update
     * the title. */
    if (need_maketitle)
	maketitle();

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

    static int
key_event_to_string (ClutterKeyEvent *key_event,
		     char_u *string)
{
    int	len;
    guint32 uc;

    uc = key_event->unicode_value;
    if (uc != 0)
    {
	/* Check for CTRL-foo */
	if ((key_event->modifier_state & CLUTTER_CONTROL_MASK)
	    && uc >= 0x20 && uc < 0x80)
	{
	    /* These mappings look arbitrary at the first glance, but in fact
	     * resemble quite exactly the behaviour of the GTK+ 1.2 GUI on my
	     * machine.  The only difference is BS vs. DEL for CTRL-8 (makes
	     * more sense and is consistent with usual terminal behaviour). */
	    if (uc >= '@')
		string[0] = uc & 0x1F;
	    else if (uc == '2')
		string[0] = NUL;
	    else if (uc >= '3' && uc <= '7')
		string[0] = uc ^ 0x28;
	    else if (uc == '8')
		string[0] = BS;
	    else if (uc == '?')
		string[0] = DEL;
	    else
		string[0] = uc;
	    len = 1;
	}
	else
	{
	    /* Translate a normal key to UTF-8.  This doesn't work for dead
	     * keys of course, you _have_ to use an input method for that. */
	    len = utf_char2bytes((int)uc, string);
	}
    }
    else
    {
	/* Translate keys which are represented by ASCII control codes in Vim.
	 * There are only a few of those; most control keys are translated to
	 * special terminal-like control sequences. */
	len = 1;
	switch (key_event->keyval)
	{
	    case CLUTTER_KEY_Tab: case CLUTTER_KEY_KP_Tab:
	    case CLUTTER_KEY_ISO_Left_Tab:
		string[0] = TAB;
		break;
	    case CLUTTER_KEY_Linefeed:
		string[0] = NL;
		break;
	    case CLUTTER_KEY_Return: case CLUTTER_KEY_ISO_Enter:
	    case CLUTTER_KEY_3270_Enter:
		string[0] = CAR;
		break;
	    case CLUTTER_KEY_Escape:
		string[0] = ESC;
		break;
	    default:
		len = 0;
		break;
	}
    }
    string[len] = NUL;

    return len;
}

    static int
modifiers_clutter2vim (ClutterModifierType modifier_state)
{
    int modifiers = 0;

    if (modifier_state & CLUTTER_SHIFT_MASK)
	modifiers |= MOD_MASK_SHIFT;
    if (modifier_state & CLUTTER_CONTROL_MASK)
	modifiers |= MOD_MASK_CTRL;
    if (modifier_state & CLUTTER_MOD1_MASK)
	modifiers |= MOD_MASK_ALT;
    if (modifier_state & CLUTTER_MOD4_MASK)
	modifiers |= MOD_MASK_META;

    return modifiers;
}

/*
 * Keycodes recognized by vim.
 * NOTE: when changing this, the table in gui_x11.c probably needs the same
 * change!
 */
static struct special_key
{
    guint key_sym;
    char_u code0;
    char_u code1;
}
const special_keys[] =
{
    {CLUTTER_KEY_Up,		'k', 'u'},
    {CLUTTER_KEY_Down,		'k', 'd'},
    {CLUTTER_KEY_Left,		'k', 'l'},
    {CLUTTER_KEY_Right,		'k', 'r'},
    {CLUTTER_KEY_F1,		'k', '1'},
    {CLUTTER_KEY_F2,		'k', '2'},
    {CLUTTER_KEY_F3,		'k', '3'},
    {CLUTTER_KEY_F4,		'k', '4'},
    {CLUTTER_KEY_F5,		'k', '5'},
    {CLUTTER_KEY_F6,		'k', '6'},
    {CLUTTER_KEY_F7,		'k', '7'},
    {CLUTTER_KEY_F8,		'k', '8'},
    {CLUTTER_KEY_F9,		'k', '9'},
    {CLUTTER_KEY_F10,		'k', ';'},
    {CLUTTER_KEY_F11,		'F', '1'},
    {CLUTTER_KEY_F12,		'F', '2'},
    {CLUTTER_KEY_F13,		'F', '3'},
    {CLUTTER_KEY_F14,		'F', '4'},
    {CLUTTER_KEY_F15,		'F', '5'},
    {CLUTTER_KEY_F16,		'F', '6'},
    {CLUTTER_KEY_F17,		'F', '7'},
    {CLUTTER_KEY_F18,		'F', '8'},
    {CLUTTER_KEY_F19,		'F', '9'},
    {CLUTTER_KEY_F20,		'F', 'A'},
    {CLUTTER_KEY_F21,		'F', 'B'},
    {CLUTTER_KEY_Pause,		'F', 'B'}, /* Pause == F21 according to netbeans.txt */
    {CLUTTER_KEY_F22,		'F', 'C'},
    {CLUTTER_KEY_F23,		'F', 'D'},
    {CLUTTER_KEY_F24,		'F', 'E'},
    {CLUTTER_KEY_F25,		'F', 'F'},
    {CLUTTER_KEY_F26,		'F', 'G'},
    {CLUTTER_KEY_F27,		'F', 'H'},
    {CLUTTER_KEY_F28,		'F', 'I'},
    {CLUTTER_KEY_F29,		'F', 'J'},
    {CLUTTER_KEY_F30,		'F', 'K'},
    {CLUTTER_KEY_F31,		'F', 'L'},
    {CLUTTER_KEY_F32,		'F', 'M'},
    {CLUTTER_KEY_F33,		'F', 'N'},
    {CLUTTER_KEY_F34,		'F', 'O'},
    {CLUTTER_KEY_F35,		'F', 'P'},
#ifdef SunXK_F36
    {SunXK_F36,		'F', 'Q'},
    {SunXK_F37,		'F', 'R'},
#endif
    {CLUTTER_KEY_Help,		'%', '1'},
    {CLUTTER_KEY_Undo,		'&', '8'},
    {CLUTTER_KEY_BackSpace,	'k', 'b'},
    {CLUTTER_KEY_Insert,	'k', 'I'},
    {CLUTTER_KEY_Delete,	'k', 'D'},
    {CLUTTER_KEY_3270_BackTab,	'k', 'B'},
    {CLUTTER_KEY_Clear,		'k', 'C'},
    {CLUTTER_KEY_Home,		'k', 'h'},
    {CLUTTER_KEY_End,		'@', '7'},
    {CLUTTER_KEY_Prior,		'k', 'P'},
    {CLUTTER_KEY_Next,		'k', 'N'},
    {CLUTTER_KEY_Print,		'%', '9'},
    /* Keypad keys: */
    {CLUTTER_KEY_KP_Left,	'k', 'l'},
    {CLUTTER_KEY_KP_Right,	'k', 'r'},
    {CLUTTER_KEY_KP_Up,		'k', 'u'},
    {CLUTTER_KEY_KP_Down,	'k', 'd'},
    {CLUTTER_KEY_KP_Insert,	KS_EXTRA, (char_u)KE_KINS},
    {CLUTTER_KEY_KP_Delete,	KS_EXTRA, (char_u)KE_KDEL},
    {CLUTTER_KEY_KP_Home,	'K', '1'},
    {CLUTTER_KEY_KP_End,	'K', '4'},
    {CLUTTER_KEY_KP_Prior,	'K', '3'},  /* page up */
    {CLUTTER_KEY_KP_Next,	'K', '5'},  /* page down */

    {CLUTTER_KEY_KP_Add,	'K', '6'},
    {CLUTTER_KEY_KP_Subtract,	'K', '7'},
    {CLUTTER_KEY_KP_Divide,	'K', '8'},
    {CLUTTER_KEY_KP_Multiply,	'K', '9'},
    {CLUTTER_KEY_KP_Enter,	'K', 'A'},
    {CLUTTER_KEY_KP_Decimal,	'K', 'B'},

    {CLUTTER_KEY_KP_0,		'K', 'C'},
    {CLUTTER_KEY_KP_1,		'K', 'D'},
    {CLUTTER_KEY_KP_2,		'K', 'E'},
    {CLUTTER_KEY_KP_3,		'K', 'F'},
    {CLUTTER_KEY_KP_4,		'K', 'G'},
    {CLUTTER_KEY_KP_5,		'K', 'H'},
    {CLUTTER_KEY_KP_6,		'K', 'I'},
    {CLUTTER_KEY_KP_7,		'K', 'J'},
    {CLUTTER_KEY_KP_8,		'K', 'K'},
    {CLUTTER_KEY_KP_9,		'K', 'L'},

    /* End of list marker: */
    {0, 0, 0}
};

    static gboolean
handle_input_idle_cb (void *user_data)
{
    unsigned int *id = user_data;

    /* XXX: this is a bit hacky! */
    static oparg_T oa; /* operator arguments */
    static gboolean oa_initialized = FALSE;

    if (!oa_initialized)
	clear_oparg (&oa);

    normal_cmd (&oa, TRUE);

    *id = 0;
    return FALSE;
}

    static void
queue_input_handling (void)
{
    static unsigned int id = 0;
    if (id == 0)
	g_idle_add (handle_input_idle_cb, &id);
}

    static gboolean
key_press_event_cb (ClutterActor *stage,
		    ClutterKeyEvent *key_event,
		    void *data)
{
    /* The largest string we may get is, up to 6 bytes + NUL + CSI
     * escapes + safety measure. */
    char_u string[32], string2[32];
    unsigned int key_sym = key_event->keyval;
    unsigned long state = key_event->modifier_state;
    int len;
    int i;
    char_u *s, *d;

    clutter_actor_queue_redraw (clutter_stage_get_default ());

    len = key_event_to_string(key_event, string2);

    /* Careful: convert_input() doesn't handle the NUL character.
     * No need to convert pure ASCII anyway, thus the len > 1 check. */
    if (len > 1 && input_conv.vc_type != CONV_NONE)
	len = convert_input(string2, len, sizeof(string2));

    s = string2;

    d = string;
    for (i = 0; i < len; ++i)
    {
	*d++ = s[i];
	if (d[-1] == CSI && d + 2 < string + sizeof(string))
	{
	    /* Turn CSI into K_CSI. */
	    *d++ = KS_EXTRA;
	    *d++ = (int)KE_CSI;
	}
    }
    len = d - string;

    /* Shift-Tab results in Left_Tab, but we want <S-Tab> */
    if (key_sym == CLUTTER_KEY_ISO_Left_Tab)
    {
	key_sym = CLUTTER_KEY_Tab;
	state |= CLUTTER_SHIFT_MASK;
    }

    /* Check for Alt/Meta key (Mod1Mask), but not for a BS, DEL or character
     * that already has the 8th bit set.
     * Don't do this for <S-M-Tab>, that should become K_S_TAB with ALT.
     * Don't do this for double-byte encodings, it turns the char into a lead
     * byte. */
    if (len == 1
	    && (state & CLUTTER_MOD1_MASK)
	    && !(key_sym == CLUTTER_KEY_BackSpace ||
		 key_sym == CLUTTER_KEY_Delete)
	    && (string[0] & 0x80) == 0
	    && !(key_sym == CLUTTER_KEY_Tab && (state & CLUTTER_SHIFT_MASK))
#ifdef FEAT_MBYTE
	    && !enc_dbcs
#endif
	    )
    {
	string[0] |= 0x80;
	state &= ~CLUTTER_MOD1_MASK; /* don't use it again */

	/* FIXME: 'tempted to simply assume FEAT_MBYTE + enc_utf8 */
#ifdef FEAT_MBYTE
	if (enc_utf8) /* convert to utf-8 */
	{
	    string[1] = string[0] & 0xbf;
	    string[0] = ((unsigned)string[0] >> 6) + 0xc0;
	    if (string[1] == CSI)
	    {
		string[2] = KS_EXTRA;
		string[3] = (int)KE_CSI;
		len = 4;
	    }
	    else
		len = 2;
	}
#endif
    }

    /* Check for special keys.	Also do this when len == 1 (key has an ASCII
     * value) to detect backspace, delete and keypad keys. */
    if (len == 0 || len == 1)
    {
	for (i = 0; special_keys[i].key_sym != 0; i++)
	{
	    if (special_keys[i].key_sym == key_sym)
	    {
		string[0] = CSI;
		string[1] = special_keys[i].code0;
		string[2] = special_keys[i].code1;
		len = -3;
		break;
	    }
	}
    }

    if (len == 0)   /* Unrecognized key */
	return TRUE;

    /* Special keys (and a few others) may have modifiers. Also when using a
     * double-byte encoding (can't set the 8th bit). */
    if (len == -3 || key_sym == CLUTTER_KEY_space || key_sym == CLUTTER_KEY_Tab
	    || key_sym == CLUTTER_KEY_Return || key_sym == CLUTTER_KEY_Linefeed
	    || key_sym == CLUTTER_KEY_Escape || key_sym == CLUTTER_KEY_KP_Tab
	    || key_sym == CLUTTER_KEY_ISO_Enter || key_sym == CLUTTER_KEY_3270_Enter
#ifdef FEAT_MBYTE
	    || (enc_dbcs && len == 1 && (state & CLUTTER_MOD1_MASK))
#endif
	    )
    {
	int modifiers = modifiers_clutter2vim(state);
	int key;

	/*
	 * For some keys a shift modifier is translated into another key
	 * code.
	 */
	if (len == -3)
	    key = TO_SPECIAL(string[1], string[2]);
	else
	    key = string[0];

	key = simplify_key(key, &modifiers);
	if (key == CSI)
	    key = K_CSI;
	if (IS_SPECIAL(key))
	{
	    string[0] = CSI;
	    string[1] = K_SECOND(key);
	    string[2] = K_THIRD(key);
	    len = 3;
	}
	else
	{
	    string[0] = key;
	    len = 1;
	}

	if (modifiers != 0)
	{
	    string2[0] = CSI;
	    string2[1] = KS_MODIFIER;
	    string2[2] = modifiers;
	    add_to_input_buf(string2, 3);
	}
    }

    if (len == 1 && ((string[0] == Ctrl_C && ctrl_c_interrupts)
		   || (string[0] == intr_char && intr_char != Ctrl_C)))
    {
	trash_input_buf();
	got_int = TRUE;
    }

    add_to_input_buf(string, len);

    /* blank out the pointer if necessary */
    if (p_mh)
	gui_mch_mousehide(TRUE);

    /* The clutter mainloop can either be run in main_loop() or in
     * gui_mch_wait_for_chars. In the latter case clutter_main_level
     * will be > 1 and we don't need to queue input handling since we
     * actually are already doing input handling and we will
     * immediately act on the input when the mainloop quits.
     */
    /* FIXME: we need to come up with a new way to handle
     * gui_mch_wait_for_chars because nested mainloops wont be
     * acceptable when we try and make a clutter-vim actor.
     */
    if (clutter_main_level () > 1)
	clutter_main_quit ();
    else
	queue_input_handling ();

    return TRUE;
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
    g_signal_connect_after (stage, "key-press-event",
			    G_CALLBACK (key_press_event_cb), NULL);

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
#if 0
    /* this is what gui_gtk_x11.c does but it looks like gtk2 ignores
     * this anyway. */
    gui.current_font = font;
    fprintf (stderr, "gui_mch_set_font FIXME\n");
#endif
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

#if defined(FEAT_TITLE) || defined(PROTO)
    void
gui_mch_settitle (char_u *title, char_u *icon UNUSED)
{
    if (title != NULL && output_conv.vc_type != CONV_NONE)
	title = string_convert(&output_conv, title, NULL);

    clutter_stage_set_title (CLUTTER_STAGE (gui.stage), (const char *)title);

    if (output_conv.vc_type != CONV_NONE)
	vim_free(title);
}
#endif /* FEAT_TITLE */

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

    static gint
input_timer_cb(gpointer data)
{
    int *timed_out = (int *) data;

    /* Just inform the caller about the occurrence of it */
    *timed_out = TRUE;

    clutter_main_quit();

    return FALSE;		/* don't happen again */
}

/*
 * GUI input routine called by gui_wait_for_chars().  Waits for a character
 * from the keyboard.
 *  wtime == -1     Wait forever.
 *  wtime == 0	    This should never happen.
 *  wtime > 0	    Wait wtime milliseconds for a character.
 * Returns OK if a character was found to be available within the given time,
 * or FAIL otherwise.
 */
    int
gui_mch_wait_for_chars (long wtime)
{
    int focus;
    guint timer;
    static int timed_out;

    fprintf (stderr, "gui_mch_wait_for_chars\n");

    timed_out = FALSE;

    /* this timeout makes sure that we will return if no characters arrived in
     * time */

    if (wtime > 0)
	timer = g_timeout_add((guint32)wtime, input_timer_cb, &timed_out);
    else
	timer = 0;

    focus = gui.in_focus;

    do
    {
	/* Stop or start blinking when focus changes */
	if (gui.in_focus != focus)
	{
	    if (gui.in_focus)
		gui_mch_start_blink();
	    else
		gui_mch_stop_blink();
	    focus = gui.in_focus;
	}

#if defined(FEAT_NETBEANS_INTG)
	/* Process the queued netbeans messages. */
	netbeans_parse_messages();
#endif

	/*
	 * Loop in GTK+ processing  until a timeout or input occurs.
	 * Skip this if input is available anyway (can happen in rare
	 * situations, sort of race condition).
	 */
	if (!input_available())
	    clutter_main();

	/* Got char, return immediately */
	if (input_available())
	{
	    if (timer != 0 && !timed_out)
		g_source_remove (timer);
	    return OK;
	}
    } while (wtime < 0 || !timed_out);

#if 0
    /*
     * Flush all eventually pending (drawing) events.
     */
    gui_mch_update();
#endif

    return FAIL;
}

