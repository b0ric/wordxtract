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
#include <gdk/gdkkeysyms.h>
#include <stdlib.h>
#include <string.h>
#include "main.h"
#include "word.h"
#include "mainwin.h"
#include "dictwin.h"
#include "dict.h"

enum {ICON_COL = 0, ICON_STR_COL, WORD_COL, N_COLUMNS};
enum {WORD_ITEM = 0};

static GtkWidget *dict_words_list;
static GtkWidget *user_words_list;
static GtkWidget *words_cnt_label;

static void set_words_count();
static void gtk_renderer_set_stock_icon(GtkTreeViewColumn *, GtkCellRenderer *, GtkTreeModel *, GtkTreeIter *, gpointer);
static gboolean dict_words_popup_by_click(GtkWidget *, GdkEventButton *, gpointer);
static gboolean dict_words_popup_by_keybd(GtkWidget *, gpointer);
static void dict_words_popup(GtkWidget *widget, GdkEventButton *);
static void remove_word_item_click(GtkWidget *, gpointer);
static gboolean dict_words_list_key_press(GtkWidget *, GdkEventKey *, gpointer);
static gboolean user_words_list_key_press(GtkWidget *, GdkEventKey *, gpointer);
static void fill_dict_words(GtkListStore *, char **);
static void add_btn_click(GtkWidget *, gpointer);
static gboolean add_entry_key_press(GtkWidget *, GdkEventKey *, gpointer);
static void cancel_btn_click(GtkWidget *, gpointer);
static void ok_btn_click(GtkWidget *, gpointer);

void create_dict_win()
{
 /*at first we refresh sorted list of words*/
 free(dict_words.by_az);
 dict_words = get_sorted(dict);

 GtkWidget *dict_win = gtk_window_new(GTK_WINDOW_TOPLEVEL);
 gtk_widget_set_size_request(dict_win, 550, 410);
 gtk_window_set_title(GTK_WINDOW(dict_win), _("WordXtract Dictionary"));
 gtk_window_set_transient_for(GTK_WINDOW(dict_win), GTK_WINDOW(main_window));
 gtk_window_set_position(GTK_WINDOW(dict_win), GTK_WIN_POS_CENTER_ON_PARENT);
 gtk_window_set_modal(GTK_WINDOW(dict_win), TRUE);
 gtk_window_set_skip_taskbar_hint(GTK_WINDOW(dict_win), TRUE);

 GtkWidget *vbox = gtk_vbox_new(FALSE, 2);
 gtk_container_add(GTK_CONTAINER(dict_win), vbox);
 gtk_widget_show(vbox);

 GtkWidget *dict_hbox = gtk_hbox_new(FALSE, 2);
 gtk_box_pack_start(GTK_BOX(vbox), dict_hbox, TRUE, TRUE, 2);
 gtk_widget_show(dict_hbox);

 /*dictionary panel*/
 GtkWidget *dict_vbox = gtk_vbox_new(FALSE, 2);
 gtk_box_pack_start(GTK_BOX(dict_hbox), dict_vbox, TRUE, TRUE, 0);
 gtk_widget_show(dict_vbox);

 GtkWidget *dict_label = gtk_label_new(_("Words in the dictionary: "));
 gtk_box_pack_start(GTK_BOX(dict_vbox), dict_label, FALSE, FALSE, 0);
 gtk_widget_show(dict_label);

 GtkWidget *dict_sc = gtk_scrolled_window_new(NULL, NULL);
 gtk_scrolled_window_set_shadow_type(GTK_SCROLLED_WINDOW(dict_sc), GTK_SHADOW_IN);
 gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW(dict_sc), GTK_POLICY_AUTOMATIC, GTK_POLICY_AUTOMATIC);
 gtk_box_pack_start(GTK_BOX(dict_vbox), dict_sc, TRUE, TRUE, 0);
 gtk_widget_show(dict_sc);

 dict_words_list = gtk_tree_view_new();
 gtk_tree_selection_set_mode(gtk_tree_view_get_selection(GTK_TREE_VIEW(dict_words_list)), GTK_SELECTION_SINGLE);
 gtk_tree_view_set_headers_visible(GTK_TREE_VIEW(dict_words_list), FALSE);
 gtk_tree_view_set_rules_hint(GTK_TREE_VIEW(dict_words_list), TRUE);
 gtk_container_add(GTK_CONTAINER(dict_sc), dict_words_list);
 GtkListStore *store_model = gtk_list_store_new(1, G_TYPE_STRING);
 gtk_tree_view_set_model(GTK_TREE_VIEW(dict_words_list), GTK_TREE_MODEL(store_model));
 g_object_unref(store_model);
 GtkCellRenderer *renderer = gtk_cell_renderer_text_new();
 g_object_set(renderer, "editable", TRUE, NULL);
 GtkTreeViewColumn *column = gtk_tree_view_column_new_with_attributes("List Items", renderer, "text", WORD_ITEM, NULL);
 gtk_tree_view_append_column(GTK_TREE_VIEW(dict_words_list), column);
 g_signal_connect(G_OBJECT(dict_words_list), "button-press-event", G_CALLBACK(dict_words_popup_by_click), NULL);
 g_signal_connect(G_OBJECT(dict_words_list), "popup-menu", G_CALLBACK(dict_words_popup_by_keybd), NULL);
 fill_dict_words(store_model, dict_words.by_az);

 g_signal_connect(G_OBJECT(dict_words_list), "key-press-event", G_CALLBACK(dict_words_list_key_press), NULL);
 gtk_widget_show(dict_words_list);

 /*user words panel*/
 GtkWidget *user_vbox = gtk_vbox_new(FALSE, 2);
 gtk_box_pack_start(GTK_BOX(dict_hbox), user_vbox, FALSE, FALSE, 0);
 gtk_widget_show(user_vbox);

 GtkWidget *user_label = gtk_label_new(_("Changes to the dictionary: "));
 gtk_box_pack_start(GTK_BOX(user_vbox), user_label, FALSE, FALSE, 0);
 gtk_widget_show(user_label);

 GtkWidget *user_sc = gtk_scrolled_window_new(NULL, NULL);
 gtk_scrolled_window_set_shadow_type(GTK_SCROLLED_WINDOW(user_sc), GTK_SHADOW_IN);
 gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW(user_sc), GTK_POLICY_AUTOMATIC, GTK_POLICY_AUTOMATIC);
 gtk_box_pack_start(GTK_BOX(user_vbox), user_sc, TRUE, TRUE, 0); 
 gtk_widget_show(user_sc);

 user_words_list = gtk_tree_view_new();
 gtk_tree_selection_set_mode(gtk_tree_view_get_selection(GTK_TREE_VIEW(user_words_list)), GTK_SELECTION_SINGLE);
 gtk_tree_view_set_headers_visible(GTK_TREE_VIEW(user_words_list), FALSE);
 gtk_tree_view_set_rules_hint(GTK_TREE_VIEW(user_words_list), TRUE);
 GtkListStore *user_store_model = gtk_list_store_new(N_COLUMNS, GDK_TYPE_PIXBUF, G_TYPE_STRING, G_TYPE_STRING); //??
 gtk_tree_view_set_model(GTK_TREE_VIEW(user_words_list), GTK_TREE_MODEL(user_store_model));
 g_object_unref(user_store_model); 

 GtkTreeViewColumn *user_column = gtk_tree_view_column_new();
 GtkCellRenderer *icon_renderer = gtk_cell_renderer_pixbuf_new();
 gtk_tree_view_column_pack_start(user_column, icon_renderer, TRUE);
 gtk_tree_view_column_set_attributes(user_column, icon_renderer, "pixbuf", ICON_COL, NULL);
 gtk_tree_view_column_set_cell_data_func(user_column, icon_renderer, gtk_renderer_set_stock_icon, NULL, NULL);
 gtk_tree_view_append_column(GTK_TREE_VIEW(user_words_list), user_column);
 /*second data fild is for icon stock id. so it doesn't need any column*/
 user_column = gtk_tree_view_column_new();
 renderer = gtk_cell_renderer_text_new();
 g_object_set(renderer, "editable", TRUE, NULL);
 gtk_tree_view_column_pack_start(user_column, renderer, TRUE);
 gtk_tree_view_column_set_attributes(user_column, renderer, "text", WORD_COL, NULL);
 gtk_tree_view_append_column(GTK_TREE_VIEW(user_words_list), user_column);
 gtk_container_add(GTK_CONTAINER(user_sc), user_words_list);
 g_signal_connect(G_OBJECT(user_words_list), "key-press-event", G_CALLBACK(user_words_list_key_press), NULL);
 gtk_widget_show(user_words_list);

 GtkWidget *entry_hbox = gtk_hbox_new(FALSE, 2);
 gtk_box_pack_start(GTK_BOX(user_vbox), entry_hbox, FALSE, FALSE, 0);
 gtk_widget_show(entry_hbox);

 GtkWidget *add_label = gtk_label_new(_("Add word: "));
 gtk_box_pack_start(GTK_BOX(entry_hbox), add_label, FALSE, FALSE, 0);
 gtk_widget_show(add_label);

 GtkWidget *add_entry = gtk_entry_new_with_max_length(WORDLENGTH);
 gtk_box_pack_start(GTK_BOX(entry_hbox), add_entry, FALSE, FALSE, 0);
 g_signal_connect(G_OBJECT(add_entry), "activate", G_CALLBACK(add_btn_click), NULL);
 g_signal_connect(G_OBJECT(add_entry), "key-press-event", G_CALLBACK(add_entry_key_press), (gpointer) user_words_list);
 gtk_widget_show(add_entry);

 GtkWidget *add_btn = gtk_button_new();
 GtkWidget *image = gtk_image_new_from_stock(GTK_STOCK_ADD, GTK_ICON_SIZE_BUTTON);
 gtk_container_add(GTK_CONTAINER(add_btn), image);
 gtk_widget_show(image);
 gtk_box_pack_start(GTK_BOX(entry_hbox), add_btn, FALSE, FALSE, 0);
 gtk_widget_set_tooltip_text(GTK_WIDGET(add_btn), _("Add word to the dictionary"));
 g_signal_connect(G_OBJECT(add_btn), "clicked", G_CALLBACK(add_btn_click), (gpointer) add_entry);
 gtk_widget_show(add_btn);

 GtkWidget *hbox = gtk_hbox_new(FALSE, 2);
 gtk_box_pack_start(GTK_BOX(vbox), hbox, FALSE, FALSE, 2);
 gtk_widget_show(hbox);

 words_cnt_label = gtk_label_new("");
 set_words_count();
 gtk_box_pack_start(GTK_BOX(hbox), words_cnt_label, FALSE, FALSE, 0);
 gtk_widget_show(words_cnt_label);

 GtkWidget *btn_hbox = gtk_hbox_new(TRUE, 2);
 gtk_box_pack_end(GTK_BOX(hbox), btn_hbox, FALSE, FALSE, 0);
 gtk_widget_show(btn_hbox);

 GtkWidget *cancel_btn = gtk_button_new_from_stock(GTK_STOCK_CANCEL);
 gtk_box_pack_end(GTK_BOX(btn_hbox), cancel_btn, TRUE, TRUE, 0);
 gtk_widget_set_tooltip_text(GTK_WIDGET(cancel_btn), _("Discard changes and close the window"));
 g_signal_connect(G_OBJECT(cancel_btn), "clicked", G_CALLBACK(cancel_btn_click), (gpointer) dict_win);
 gtk_widget_show(cancel_btn);

 GtkWidget *ok_btn = gtk_button_new_from_stock(GTK_STOCK_OK);
 gtk_box_pack_end(GTK_BOX(btn_hbox), ok_btn, TRUE, TRUE, 0);
 gtk_widget_set_tooltip_text(GTK_WIDGET(ok_btn), _("Apply changes and close the window"));
 g_signal_connect(G_OBJECT(ok_btn), "clicked", G_CALLBACK(ok_btn_click), (gpointer) dict_win);
 gtk_widget_show(ok_btn);

 gtk_widget_show(dict_win);
}

static void fill_dict_words(GtkListStore *store, char **word_list)
{
 GtkTreeIter iter;
 int i;

 for (i = 0; i < dict_words.count; i++) {
	gtk_list_store_append(store, &iter);
	gtk_list_store_set(store, &iter, WORD_ITEM, *word_list, -1);
	word_list++;
 }
}

static void set_words_count()
{
 char words_cnt_str[100] = {0};
 sprintf(words_cnt_str, _("<b> Words in the dictionary: %d</b>"), dict_words.count);
 gtk_label_set_text(GTK_LABEL(words_cnt_label), words_cnt_str);
 gtk_label_set_use_markup(GTK_LABEL(words_cnt_label), TRUE);
}

static void gtk_renderer_set_stock_icon(GtkTreeViewColumn *column, GtkCellRenderer *renderer,
							GtkTreeModel *model, GtkTreeIter *iter, gpointer data)
{
 gchar *stock_id;

 gtk_tree_model_get(GTK_TREE_MODEL(model), iter, ICON_STR_COL, &stock_id, -1);
 g_object_set(renderer, "stock-id", stock_id, NULL);
}

static gboolean dict_words_popup_by_click(GtkWidget *word_list, GdkEventButton *event, gpointer data)
{
 GtkTreePath *path;

 if (event->type == GDK_BUTTON_PRESS && event->button == 3) {
	GtkTreeSelection *selection = gtk_tree_view_get_selection(GTK_TREE_VIEW(word_list));
	if (gtk_tree_selection_get_selected(GTK_TREE_SELECTION(selection), NULL, NULL)) {
		if (gtk_tree_view_get_path_at_pos(GTK_TREE_VIEW(word_list), (gint) event->x, 
										  (gint) event->y, &path, NULL, NULL, NULL)) {
			gtk_tree_selection_unselect_all(selection);
			gtk_tree_selection_select_path(selection, path);
			dict_words_popup(GTK_WIDGET(word_list), event);
			return TRUE;
		}
	}
 }
 return FALSE;
}

static gboolean dict_words_popup_by_keybd(GtkWidget *word_list, gpointer data)
{
 dict_words_popup(word_list, NULL);
 return TRUE;
}

static void dict_words_popup(GtkWidget *widget, GdkEventButton *event)
{
 GtkWidget *popup_menu = gtk_menu_new();
 GtkWidget *remove_word_item = gtk_image_menu_item_new_from_stock(GTK_STOCK_REMOVE, NULL);
 gtk_widget_set_tooltip_text(GTK_WIDGET(remove_word_item), _("Remove this word from the dictionary"));
 g_signal_connect(G_OBJECT(remove_word_item), "activate", G_CALLBACK(remove_word_item_click), NULL);
 gtk_menu_shell_append(GTK_MENU_SHELL(popup_menu), remove_word_item);
 gtk_widget_show(remove_word_item);
 gtk_widget_show(popup_menu);
 gtk_menu_popup(GTK_MENU(popup_menu), NULL, NULL, NULL, NULL,
				(event != NULL) ? event->button : 0, gdk_event_get_time((GdkEvent*)event));
}

static void remove_word_item_click(GtkWidget *widget, gpointer data)
{
 GtkTreeModel *model;
 GtkListStore *user_store_model;
 GtkTreeIter iter;
 gboolean valid = FALSE, remove = TRUE;
 gchar *word;

 GtkTreeSelection *selection = gtk_tree_view_get_selection(GTK_TREE_VIEW(dict_words_list));
 if (gtk_tree_selection_get_selected(selection, &model, &iter)) {
	gtk_tree_model_get(model, &iter, WORD_ITEM, &word, -1);
	user_store_model = GTK_LIST_STORE(gtk_tree_view_get_model(GTK_TREE_VIEW(user_words_list)));
	valid = gtk_tree_model_get_iter_first(GTK_TREE_MODEL(user_store_model), &iter);
	gchar *cur_word;
	while (valid) {
		gtk_tree_model_get(GTK_TREE_MODEL(user_store_model), &iter, WORD_COL, &cur_word, -1);
		if (!strcmp(word, cur_word)) {
			remove = FALSE;
			g_free(cur_word);
			break;
		}
		valid = gtk_tree_model_iter_next(GTK_TREE_MODEL(user_store_model), &iter);
	}
	if (remove) {
		gtk_list_store_append(user_store_model, &iter);
		gtk_list_store_set(user_store_model, &iter, ICON_STR_COL, "gtk-remove", WORD_COL, word, -1);
	}
 }
}

static gboolean dict_words_list_key_press(GtkWidget *widget, GdkEventKey *event, gpointer data)
{
 if ((event->type == GDK_KEY_PRESS)&&(event->keyval == GDK_Delete))
	remove_word_item_click(widget, data);
 return FALSE;
}

static gboolean user_words_list_key_press(GtkWidget *widget, GdkEventKey *event, gpointer data)
{
 GtkListStore *user_store_model;
 GtkTreeIter iter;

 if ((event->type == GDK_KEY_PRESS)&&(event->keyval == GDK_Delete)) {
	GtkTreeSelection *selection = gtk_tree_view_get_selection(GTK_TREE_VIEW(user_words_list));
	if (gtk_tree_selection_get_selected(selection, NULL, &iter)) {
		user_store_model = GTK_LIST_STORE(gtk_tree_view_get_model(GTK_TREE_VIEW(user_words_list)));
		gtk_list_store_remove(GTK_LIST_STORE(user_store_model), &iter);
	}
 }
 return FALSE;
}

static void add_btn_click(GtkWidget *widget, gpointer add_entry)
{
 GtkListStore *user_store_model;
 GtkTreeIter iter;
 gpointer entry;
 gboolean valid, add = FALSE;
 gchar *word;

 if (add_entry)
	entry = add_entry;
 else
	entry = widget;
 word = (gchar *) gtk_entry_get_text(GTK_ENTRY(entry));
 add = strcmp(word, "")&&!is_in_dict(word, dict);
 user_store_model = GTK_LIST_STORE(gtk_tree_view_get_model(GTK_TREE_VIEW(user_words_list)));

 if (add) {
	/*check whether word is already in 'words to add' list*/
	valid = gtk_tree_model_get_iter_first(GTK_TREE_MODEL(user_store_model), &iter);
	gchar *cur_word;
	while (valid) {
		gtk_tree_model_get(GTK_TREE_MODEL(user_store_model), &iter, WORD_COL, &cur_word, -1);
		if (!strcmp(word, cur_word)) {
			add = FALSE;
			g_free(cur_word);
			break;
		}
		valid = gtk_tree_model_iter_next(GTK_TREE_MODEL(user_store_model), &iter);
	}
 }
 if (add) {
		gtk_list_store_append(user_store_model, &iter);
		gtk_list_store_set(user_store_model, &iter, ICON_STR_COL, "gtk-add", WORD_COL, word, -1);
		gtk_entry_set_text(GTK_ENTRY(entry), "");
 } else {
	if (strcmp(word, "")) {
		gtk_label_set_text(GTK_LABEL(words_cnt_label), _("<b>This word is in the dictionary!</b>"));
		gtk_label_set_use_markup(GTK_LABEL(words_cnt_label), TRUE);
	}
 }
}

static gboolean add_entry_key_press(GtkWidget *widget, GdkEventKey *event, gpointer data)
{
 if (event->type == GDK_KEY_PRESS)
	if ((event->length)||(event->keyval == GDK_BackSpace)||(event->keyval == GDK_Delete))
		set_words_count();
 return FALSE;
}

static void cancel_btn_click(GtkWidget *widget, gpointer dict_win)
{
 gtk_widget_destroy(GTK_WIDGET(dict_win));
}

static void ok_btn_click(GtkWidget *widget, gpointer dict_win)
{
 GtkTreeIter iter;
 gboolean valid;
 gchar *stock_id;
 gchar *word;
 FILE *fdict;

 GtkListStore *user_store_model = GTK_LIST_STORE(gtk_tree_view_get_model(GTK_TREE_VIEW(user_words_list)));
 valid = gtk_tree_model_get_iter_first(GTK_TREE_MODEL(user_store_model), &iter);
 while (valid) {
	gtk_tree_model_get(GTK_TREE_MODEL(user_store_model), &iter, ICON_STR_COL, &stock_id, -1);
	if (!strcmp(stock_id, "gtk-add")) {
		gtk_tree_model_get(GTK_TREE_MODEL(user_store_model), &iter, WORD_COL, &word, -1);
		add_word(dict, word);
		g_free(stock_id);
		g_free(word);
	}
	if (!strcmp(stock_id, "gtk-remove")) {
		gtk_tree_model_get(GTK_TREE_MODEL(user_store_model), &iter, WORD_COL, &word, -1);
		del_word(&dict, word);
		g_free(stock_id);
		g_free(word);
	}
 	valid = gtk_tree_model_iter_next(GTK_TREE_MODEL(user_store_model), &iter);
 }

 if (!(fdict = fopen(dictfile, "w")))
	perror(dictfile);
 else {
	save_dict(fdict, dict);
	fclose(fdict);
 }
 free(dict_words.by_az);
 dict_words = get_sorted(dict);
 gtk_widget_destroy(GTK_WIDGET(dict_win));
}


