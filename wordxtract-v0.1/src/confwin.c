/*
 * This file is part of WordXtract.
 *
 * Copyright (C) 2009 Borisov Alexandr
 *
 * WordXtract is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * WordXtract is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with WordXtract. If not, see <http://www.gnu.org/licenses/>.
 */

#include <gtk/gtk.h>
#include "main.h"
#include "engparser.h"
#include "mainwin.h"

static GtkWidget *conf_win;

/*saving controls*/
static GtkWidget *col_counter;
static GtkWidget *field_counter;
/*general parsing controls*/
static GtkWidget *wcap_btn;
static GtkWidget *lower_first_cap;
/*quote controls*/
static GtkWidget *wquot_btn;		/*==excl_with_quoted_btn (here and below*/ 
static GtkWidget *quoted_btn;		/*'excl_' part of vatiable name is omitted)*/
static GtkWidget *st_quote_btn;
static GtkWidget *mid_quote_btn;
static GtkWidget *after_quote_btn;
static GtkWidget *end_quote_btn;
/*hyphen controls*/
static GtkWidget *whyph_btn;
static GtkWidget *hyphened_btn;
static GtkWidget *st_hyphen_btn;
static GtkWidget *mid_hyphen_btn;
static GtkWidget *after_hyphen_btn;
static GtkWidget *end_hyphen_btn;

static void ok_btn_click(GtkWidget *, gpointer);
static void cancel_btn_click(GtkWidget *, gpointer);
static void wquot_btn_click(GtkWidget *, gpointer);
static void mid_quote_btn_click(GtkWidget *, gpointer);
static void after_quote_btn_click(GtkWidget *, gpointer);
static void whyph_btn_click(GtkWidget *, gpointer);
static void mid_hyphen_btn_click(GtkWidget *, gpointer);
static void after_hyphen_btn_click(GtkWidget *, gpointer);

void create_conf_win()
{
 conf_win = gtk_window_new(GTK_WINDOW_TOPLEVEL);
 gtk_widget_set_size_request(conf_win, 455, 350);
 gtk_window_set_title(GTK_WINDOW(conf_win), _("WordXtract Preferences"));
 gtk_window_set_transient_for(GTK_WINDOW(conf_win), GTK_WINDOW(main_window));
 gtk_window_set_position(GTK_WINDOW(conf_win), GTK_WIN_POS_CENTER_ON_PARENT);
 gtk_window_set_modal(GTK_WINDOW(conf_win), TRUE);
 gtk_window_set_skip_taskbar_hint(GTK_WINDOW(conf_win), TRUE);

 GtkWidget *vbox = gtk_vbox_new(FALSE, 2);
 gtk_container_add(GTK_CONTAINER(conf_win), vbox);
 gtk_widget_show(vbox);

 GtkWidget *tabs = gtk_notebook_new();
 gtk_box_pack_start(GTK_BOX(vbox), tabs, TRUE, TRUE, 2);
 /*'General' tab*/
 GtkWidget *gen_label = gtk_label_new(_("General"));
 GtkWidget *gen_frame = gtk_frame_new(NULL);
 gtk_container_set_border_width(GTK_CONTAINER(gen_frame), 5);
 gtk_frame_set_shadow_type(GTK_FRAME(gen_frame), GTK_SHADOW_IN);
 gtk_widget_show(gen_frame);

 GtkWidget *gen_table = gtk_table_new(6, 12, TRUE);
 gtk_container_set_border_width(GTK_CONTAINER(gen_table), 10);
 gtk_container_add(GTK_CONTAINER(gen_frame), gen_table);
 gtk_widget_show(gen_table);

 GtkWidget *save_label = gtk_label_new(_("<b>Words saving options</b>"));
 gtk_label_set_use_markup(GTK_LABEL(save_label), TRUE);
 gtk_table_attach_defaults(GTK_TABLE(gen_table), save_label, 0, 6, 0, 1);
 gtk_misc_set_alignment(GTK_MISC(save_label), 0, 0.5);
 gtk_widget_show(save_label);

 GtkWidget *col_label = gtk_label_new(_("Columns: "));
 gtk_misc_set_alignment(GTK_MISC(col_label), 0, 0.5);
 gtk_table_attach_defaults(GTK_TABLE(gen_table), col_label, 1, 4, 1, 2);
 gtk_widget_show(col_label);

 GtkObject *col_adjust = gtk_adjustment_new(save_user_words.columns, 1, 10, 1, 2, 0);
 col_counter = gtk_spin_button_new(GTK_ADJUSTMENT(col_adjust), 0.5, 0);
 gtk_widget_set_tooltip_text(col_counter, _("Specify how many columns will be in the saved list of the words"));
 gtk_table_attach_defaults(GTK_TABLE(gen_table), col_counter, 5, 7, 1, 2);
 gtk_widget_show(col_counter);

 gboolean field_sens = (save_user_words.columns > 1) ? TRUE : FALSE;
 GtkWidget *field_label = gtk_label_new(_("Field langth: "));
 gtk_misc_set_alignment(GTK_MISC(field_label), 0, 0.5);
 gtk_widget_set_sensitive(field_label, field_sens);
 gtk_table_attach_defaults(GTK_TABLE(gen_table), field_label, 1, 4, 2, 3);
 gtk_widget_show(field_label);

 GtkObject *field_adjust = gtk_adjustment_new(save_user_words.col_width, 10, 100, 1, 10, 0);
 field_counter = gtk_spin_button_new(GTK_ADJUSTMENT(field_adjust), 0.5, 0);
 gtk_widget_set_tooltip_text(field_counter, _("Specify length of one field (in symbols) in the saved list of the words"));
 gtk_widget_set_sensitive(field_counter, field_sens);
 gtk_table_attach_defaults(GTK_TABLE(gen_table), field_counter, 5, 7, 2, 3);
 gtk_widget_show(field_counter);
 gtk_notebook_append_page(GTK_NOTEBOOK(tabs), gen_frame, gen_label);

 /*'Analysing' tab*/
 GtkWidget *analys_label = gtk_label_new(_("Analyser"));
 GtkWidget *analys_frame = gtk_frame_new(NULL);
 gtk_container_set_border_width(GTK_CONTAINER(analys_frame), 5);
 gtk_frame_set_shadow_type(GTK_FRAME(analys_frame), GTK_SHADOW_IN);
 gtk_widget_show(analys_frame);

 GtkWidget *analys_sc_win = gtk_scrolled_window_new(NULL, NULL);
 gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW(analys_sc_win), GTK_POLICY_AUTOMATIC, GTK_POLICY_AUTOMATIC);
 gtk_container_add(GTK_CONTAINER(analys_frame), analys_sc_win);
 gtk_widget_show(analys_sc_win);

 GtkWidget *analys_table = gtk_table_new(17, 12, TRUE);
 gtk_container_set_border_width(GTK_CONTAINER(analys_table), 10);
 gtk_widget_set_size_request(analys_table, 410, 700);
 gtk_scrolled_window_add_with_viewport(GTK_SCROLLED_WINDOW(analys_sc_win), analys_table);
 gtk_widget_show(analys_table);

 GtkWidget *lang_label = gtk_label_new(_("Analyser language: "));
 gtk_table_attach_defaults(GTK_TABLE(analys_table), lang_label, 0, 4, 0, 1);
 gtk_widget_show(lang_label);

 GtkWidget *lang_box = gtk_combo_box_new_text();
 gtk_combo_box_append_text(GTK_COMBO_BOX(lang_box), _("English"));
 gtk_combo_box_set_active(GTK_COMBO_BOX(lang_box), 0);
 gtk_table_attach_defaults(GTK_TABLE(analys_table), lang_box, 5, 9, 0, 1);
 gtk_widget_set_tooltip_text(lang_box, _("Language of the text you want to analyse"));
 gtk_widget_show(lang_box);

 wcap_btn = gtk_check_button_new_with_label(_("Exclude words with cAPiTal letters"));
 gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(wcap_btn), (gboolean) excl_w_capital);
 gtk_table_attach_defaults(GTK_TABLE(analys_table), wcap_btn, 0, 12, 1, 2);
 gtk_widget_show(wcap_btn);

 lower_first_cap = gtk_check_button_new_with_label(_("Lower first capital in each sentence"));
 gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(lower_first_cap), (gboolean) lower_first_capital);
 gtk_table_attach_defaults(GTK_TABLE(analys_table), lower_first_cap, 0, 12, 2, 3);
 gtk_widget_show(lower_first_cap);

 GtkWidget *quote_label = gtk_label_new(_("<b>Quotes analysis</b>"));
 gtk_label_set_use_markup(GTK_LABEL(quote_label), TRUE);
 gtk_misc_set_alignment(GTK_MISC(quote_label), 0, 0.5);
 gtk_table_attach_defaults(GTK_TABLE(analys_table), quote_label, 0, 12, 3, 4);
 gtk_widget_show(quote_label);

 wquot_btn = gtk_check_button_new_with_label(_("Exclude words with quotes"));
 gboolean quotes = quote.excl_symbolled&&quote.excl_w_starting
					&&quote.excl_w_middle&&quote.excl_w_ending;
 gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(wquot_btn), quotes);
 gtk_table_attach_defaults(GTK_TABLE(analys_table), wquot_btn, 1, 12, 4, 5);
 g_signal_connect(G_OBJECT(wquot_btn), "clicked", G_CALLBACK(wquot_btn_click), NULL);
 gtk_widget_show(wquot_btn);

 quoted_btn = gtk_check_button_new_with_label(_("Exclude 'quoted' words"));
 gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(quoted_btn), quote.excl_symbolled);
 gtk_widget_set_sensitive(quoted_btn, !quotes);
 gtk_table_attach_defaults(GTK_TABLE(analys_table), quoted_btn, 2, 12, 5, 6);
 gtk_widget_show(quoted_btn);

 st_quote_btn = gtk_check_button_new_with_label(_("Exclude words with 'quote in the beginning"));
 gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(st_quote_btn), quote.excl_w_starting);
 gtk_widget_set_sensitive(st_quote_btn, !quotes);
 gtk_table_attach_defaults(GTK_TABLE(analys_table), st_quote_btn, 2, 12, 6, 7);
 gtk_widget_show(st_quote_btn);

 mid_quote_btn = gtk_check_button_new_with_label(_("Exclude words with quo'te in the middle"));
 gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(mid_quote_btn), quote.excl_w_middle);
 gtk_widget_set_sensitive(mid_quote_btn, !quotes&&!quote.excl_word_after_symb);
 gtk_table_attach_defaults(GTK_TABLE(analys_table), mid_quote_btn, 2, 12, 7, 8);
 g_signal_connect(G_OBJECT(mid_quote_btn), "clicked", G_CALLBACK(mid_quote_btn_click), NULL);
 gtk_widget_show(mid_quote_btn);

 after_quote_btn = gtk_check_button_new();
 GtkWidget *after_quote_label = gtk_label_new(_("Exclude only part of the word after quo<s>'te</s>"));
 gtk_label_set_use_markup(GTK_LABEL(after_quote_label), TRUE);
 gtk_container_add(GTK_CONTAINER(after_quote_btn), after_quote_label);
 gtk_widget_show(after_quote_label);
 gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(after_quote_btn), quote.excl_word_after_symb);
 gtk_widget_set_sensitive(after_quote_btn, !quotes&&!quote.excl_w_middle);
 gtk_table_attach_defaults(GTK_TABLE(analys_table), after_quote_btn, 2, 12, 8, 9);
 g_signal_connect(G_OBJECT(after_quote_btn), "clicked", G_CALLBACK(after_quote_btn_click), NULL);
 gtk_widget_show(after_quote_btn);

 end_quote_btn = gtk_check_button_new_with_label(_("Exclude words with quote' in the end"));
 gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(end_quote_btn), quote.excl_w_ending);
 gtk_widget_set_sensitive(end_quote_btn, !quotes);
 gtk_table_attach_defaults(GTK_TABLE(analys_table), end_quote_btn, 2, 12, 9, 10);
 gtk_widget_show(end_quote_btn);

 GtkWidget *hyphen_label = gtk_label_new(_("<b>Hyphen analysis</b>"));
 gtk_label_set_use_markup(GTK_LABEL(hyphen_label), TRUE);
 gtk_misc_set_alignment(GTK_MISC(hyphen_label), 0, 0.5);
 gtk_table_attach_defaults(GTK_TABLE(analys_table), hyphen_label, 0, 12, 10, 11);
 gtk_widget_show(hyphen_label);

 whyph_btn = gtk_check_button_new_with_label(_("Exclude words with hyphens"));
 gboolean hyphens = hyphen.excl_symbolled&&hyphen.excl_w_starting
					&&hyphen.excl_w_middle&&hyphen.excl_w_ending;
 gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(whyph_btn), hyphens);
 gtk_table_attach_defaults(GTK_TABLE(analys_table), whyph_btn, 1, 12, 11, 12);
 g_signal_connect(G_OBJECT(whyph_btn), "clicked", G_CALLBACK(whyph_btn_click), NULL);
 gtk_widget_show(whyph_btn);

 hyphened_btn = gtk_check_button_new_with_label(_("Exclude -hyphened- words"));
 gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(hyphened_btn), hyphen.excl_symbolled);
 gtk_widget_set_sensitive(hyphened_btn, !hyphens);
 gtk_table_attach_defaults(GTK_TABLE(analys_table), hyphened_btn, 2, 12, 12, 13);
 gtk_widget_show(hyphened_btn);

 st_hyphen_btn = gtk_check_button_new_with_label(_("Exclude words with -hyphen in the beginning"));
 gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(st_hyphen_btn), hyphen.excl_w_starting);
 gtk_widget_set_sensitive(st_hyphen_btn, !hyphens);
 gtk_table_attach_defaults(GTK_TABLE(analys_table), st_hyphen_btn, 2, 12, 13, 14);
 gtk_widget_show(st_hyphen_btn);

 mid_hyphen_btn = gtk_check_button_new_with_label(_("Exclude words with hy-phen in the middle"));
 gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(mid_hyphen_btn), hyphen.excl_w_middle);
 gtk_widget_set_sensitive(mid_hyphen_btn, !hyphens);
 gtk_table_attach_defaults(GTK_TABLE(analys_table), mid_hyphen_btn, 2, 12, 14, 15); 
 g_signal_connect(G_OBJECT(mid_hyphen_btn), "clicked", G_CALLBACK(mid_hyphen_btn_click), NULL);
 gtk_widget_show(mid_hyphen_btn);

 after_hyphen_btn = gtk_check_button_new();
 GtkWidget *after_hyphen_label = gtk_label_new(_("Exclude only part of the word after hy<s>-phen</s>"));
 gtk_label_set_use_markup(GTK_LABEL(after_hyphen_label), TRUE);
 gtk_container_add(GTK_CONTAINER(after_hyphen_btn), after_hyphen_label);
 gtk_widget_show(after_hyphen_label);
 gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(after_hyphen_btn), hyphen.excl_word_after_symb);
 gtk_widget_set_sensitive(after_hyphen_btn, !hyphens&&!hyphen.excl_w_middle);
 gtk_table_attach_defaults(GTK_TABLE(analys_table), after_hyphen_btn, 2, 12, 15, 16);
 g_signal_connect(G_OBJECT(after_hyphen_btn), "clicked", G_CALLBACK(after_hyphen_btn_click), NULL);
 gtk_widget_show(after_hyphen_btn);

 end_hyphen_btn = gtk_check_button_new_with_label(_("Exclude words with hyphen- in the end"));
 gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(end_hyphen_btn), hyphen.excl_w_ending);
 gtk_widget_set_sensitive(end_hyphen_btn, !hyphens);
 gtk_table_attach_defaults(GTK_TABLE(analys_table), end_hyphen_btn, 2, 12, 16, 17);
 gtk_widget_show(end_hyphen_btn);
 gtk_notebook_append_page(GTK_NOTEBOOK(tabs), analys_frame, analys_label);
 gtk_widget_show(tabs);

 GtkWidget *hbox = gtk_hbox_new(FALSE, 2);
 gtk_box_pack_start(GTK_BOX(vbox), hbox, FALSE, FALSE, 0);
 gtk_widget_show(hbox);

 GtkWidget *btn_hbox = gtk_hbox_new(TRUE, 2);
 gtk_box_pack_end(GTK_BOX(hbox), btn_hbox, FALSE, FALSE, 0);
 gtk_widget_show(btn_hbox);

 GtkWidget *ok_btn = gtk_button_new_from_stock(GTK_STOCK_OK);
 gtk_box_pack_end(GTK_BOX(btn_hbox), ok_btn, TRUE, TRUE, 0);
 g_signal_connect(G_OBJECT(ok_btn), "clicked", G_CALLBACK(ok_btn_click), NULL);
 gtk_widget_show(ok_btn);

 GtkWidget *cancel_btn = gtk_button_new_from_stock(GTK_STOCK_CANCEL);
 gtk_box_pack_end(GTK_BOX(btn_hbox), cancel_btn, TRUE, TRUE, 0);
 g_signal_connect(G_OBJECT(cancel_btn), "clicked", G_CALLBACK(cancel_btn_click), NULL);
 gtk_widget_show(cancel_btn);

 gtk_widget_show(conf_win);
}

static void ok_btn_click(GtkWidget *widget, gpointer data)
{
 save_user_words.columns = (int) gtk_spin_button_get_value_as_int(GTK_SPIN_BUTTON(col_counter));
 save_user_words.col_width = (int) gtk_spin_button_get_value_as_int(GTK_SPIN_BUTTON(field_counter));
 /*lang = */
 excl_w_capital = (int) GTK_TOGGLE_BUTTON(wcap_btn)->active;
 lower_first_capital = (int) GTK_TOGGLE_BUTTON(lower_first_cap)->active;
 if (GTK_TOGGLE_BUTTON(wquot_btn)->active) {
	quote.excl_symbolled = 1;
	quote.excl_w_starting = 1;
	quote.excl_w_middle = 1;
	quote.excl_w_ending = 1;
 } else {
	quote.excl_symbolled = (int) GTK_TOGGLE_BUTTON(quoted_btn)->active;
	quote.excl_w_starting = (int) GTK_TOGGLE_BUTTON(st_quote_btn)->active;
	quote.excl_w_middle = (int) GTK_TOGGLE_BUTTON(mid_quote_btn)->active;
	quote.excl_word_after_symb = (int) GTK_TOGGLE_BUTTON(after_quote_btn)->active;
	quote.excl_w_ending = (int) GTK_TOGGLE_BUTTON(end_quote_btn)->active;
 }
 if (GTK_TOGGLE_BUTTON(whyph_btn)->active) {
	hyphen.excl_symbolled = 1;
	hyphen.excl_w_starting = 1; 
	hyphen.excl_w_middle = 1;
	hyphen.excl_w_ending = 1;
 } else {
	hyphen.excl_symbolled = (int) GTK_TOGGLE_BUTTON(hyphened_btn)->active;
	hyphen.excl_w_starting = (int) GTK_TOGGLE_BUTTON(st_hyphen_btn)->active;
	hyphen.excl_w_middle = (int) GTK_TOGGLE_BUTTON(mid_hyphen_btn)->active;
	hyphen.excl_word_after_symb = (int) GTK_TOGGLE_BUTTON(after_hyphen_btn)->active;
	hyphen.excl_w_ending = (int) GTK_TOGGLE_BUTTON(end_hyphen_btn)->active;
 }
 write_config();
 gtk_widget_destroy(conf_win);
}

static void cancel_btn_click(GtkWidget *widget, gpointer data)
{
 gtk_widget_destroy(conf_win);
}

static void wquot_btn_click(GtkWidget *button, gpointer data)
{
 gboolean state = !gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(button));
 gtk_widget_set_sensitive(quoted_btn, state);
 gtk_widget_set_sensitive(st_quote_btn, state);
 gtk_widget_set_sensitive(mid_quote_btn, state&&!GTK_TOGGLE_BUTTON(after_quote_btn)->active);
 gtk_widget_set_sensitive(after_quote_btn, state&&!GTK_TOGGLE_BUTTON(mid_quote_btn)->active);
 gtk_widget_set_sensitive(end_quote_btn, state);
}

static void mid_quote_btn_click(GtkWidget *button, gpointer data)
{
 gboolean state = !gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(button));
 gtk_widget_set_sensitive(after_quote_btn, state);
}

static void after_quote_btn_click(GtkWidget *button, gpointer data)
{
 gboolean state = !gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(button));
 gtk_widget_set_sensitive(mid_quote_btn, state);
}

static void whyph_btn_click(GtkWidget *button, gpointer data)
{
 gboolean state = !gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(button));
 gtk_widget_set_sensitive(hyphened_btn, state);
 gtk_widget_set_sensitive(st_hyphen_btn, state);
 gtk_widget_set_sensitive(mid_hyphen_btn, state&&!GTK_TOGGLE_BUTTON(after_hyphen_btn)->active);
 gtk_widget_set_sensitive(after_hyphen_btn, state&&!GTK_TOGGLE_BUTTON(mid_hyphen_btn)->active);
 gtk_widget_set_sensitive(end_hyphen_btn, state);
}

static void mid_hyphen_btn_click(GtkWidget *button, gpointer data)
{
 gboolean state = !gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(button));
 gtk_widget_set_sensitive(after_hyphen_btn, state);
}

static void after_hyphen_btn_click(GtkWidget *button, gpointer data)
{
 gboolean state = !gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(button));
 gtk_widget_set_sensitive(mid_hyphen_btn, state);
}







