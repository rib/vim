/* vi:set ts=8 sts=4 sw=4:
 *
 * VIM - Vi IMproved	by Bram Moolenaar
 *
 * Do ":help uganda"  in Vim to read copying and usage conditions.
 * Do ":help credits" in Vim to see a list of people who contributed.
 */

#ifndef GUI_CLUTTER__H
# define GUI_CLUTTER__H

#include <ucontext.h>

extern ucontext_t main_uctx, vim_uctx, gui_uctx;

void gui_clutter_start_mainloop(void);

#endif
