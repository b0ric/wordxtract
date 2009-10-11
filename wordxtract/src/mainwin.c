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

#ifdef WIN32
#include <windows.h>
#endif
#include <gtk/gtk.h>
#include <gdk/gdkkeysyms.h>
#include <sys/stat.h>
#include <stdlib.h>
#include <libgen.h>
#include <string.h>
#include <ctype.h>
#include "main.h"
#include "word.h"
#include "mainwin.h"
#include "dictwin.h"
#include "confwin.h"
#include "dict.h"
#include "plaintext.h"
#include "srt.h"

#define SEARCH_FORWARD 0
#define SEARCH_BACKWARD 1

enum {WORD_ITEM = 0, N_COLUMNS};

GtkWidget *main_window;

static GtkWidget *sentences_text;
static GtkWidget *find_btn;
static GtkWidget *prev_btn;
static GtkWidget *next_btn;
static GtkWidget *clear_btn;
static GtkWidget *statusbar;
/*list view variables*/
static GtkWidget *word_list;
static GtkListStore *store_model;
/*widgets which should be disabled/enabled*/
static GtkWidget *save_item;
static GtkToolItem *save;
static GtkWidget *find_label;
static GtkWidget *find_entry;
/*vars to control current word highlighting (green)*/
static int current_highlighted_end;
static int current_highlighted_start;
/*statusbar context id*/
static guint id, find_id;

static void set_status_msg();
static gint main_window_close(GtkWidget *, GdkEvent *, gpointer);
static void main_window_destroy(GtkWidget *, gpointer);
static void open_item_click(GtkWidget *, gpointer);
static void save_words_item_click(GtkWidget *, gpointer);
static void save_text_item_click(GtkWidget *, gpointer);
static void quit_item_click(GtkWidget *, gpointer);
static void dict_item_click(GtkWidget *, gpointer);
static void pref_item_click(GtkWidget *, gpointer);
static void website_click(GtkAboutDialog *, const gchar *, gpointer);
static void about_item_click(GtkWidget *, gpointer);
static gboolean popup_by_click(GtkWidget *, GdkEventButton *, gpointer);
static gboolean popup_by_keybd(GtkWidget *, gpointer);
static void word_list_popup(GtkWidget *, GdkEventButton *);
static void show_sent_item_click(GtkWidget *, gpointer);
static void add_to_dict_item_click(GtkWidget *, gpointer);
static void fill_list(char **);
static void change_search_navigation_state(gboolean);
static gboolean find_entry_key_press(GtkWidget *, GdkEventKey *, gpointer);
static void on_find_text_change(GtkWidget *, gpointer);
static void find_prev_btn_click(GtkWidget *, gpointer);
static void find_next_btn_click(GtkWidget *, gpointer);
static void clear_btn_click(GtkWidget *, gpointer);
static void search_sentences(const gchar *);
static void set_find_text(gchar *);
static void clear_search_tags();
static gboolean gtk_text_iter_search_word(const GtkTextIter *, const gchar *,
										  GtkTextSearchFlags, GtkTextIter *,
										  GtkTextIter *, const GtkTextIter *, int);
static void find_other(const gchar *, const int);

void create_main_window()
{
 GtkAccelGroup *accel_group = gtk_accel_group_new();

 main_window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
 gtk_widget_set_size_request(main_window, 640, 480);
 gtk_window_set_title(GTK_WINDOW(main_window), PROGNAME);
 gtk_window_set_position(GTK_WINDOW(main_window), GTK_WIN_POS_CENTER);

#ifndef WIN32
 char *iconfile = malloc(strlen(WORDXTRACT_DATA_DIR) + strlen("/pixmaps/wordxtract2.png")+1);
 strcpy(iconfile, WORDXTRACT_DATA_DIR);
 strcat(iconfile, "/pixmaps/wordxtract2.png");

 GError *error = NULL;
 GdkPixbuf *icon = gdk_pixbuf_new_from_file(iconfile, &error);
 if(!icon) {
	fprintf(stderr, "%s\n", error->message);
	g_error_free(error);
 }
 gtk_window_set_icon(GTK_WINDOW(main_window), icon);
#endif

 g_signal_connect(G_OBJECT(main_window), "delete_event", G_CALLBACK(main_window_close), (gpointer) main_window);
 g_signal_connect(G_OBJECT(main_window), "destroy", G_CALLBACK(main_window_destroy), NULL);

 GtkWidget *vbox = gtk_vbox_new(FALSE, 2);
 gtk_widget_show(vbox);
 gtk_container_add(GTK_CONTAINER(main_window), vbox);

 GtkWidget *menu_bar = gtk_menu_bar_new();
 GtkWidget *sep_item = gtk_separator_menu_item_new();
 GtkWidget *file_menu = gtk_menu_new();
 GtkWidget *file_item = gtk_menu_item_new_with_mnemonic(_("_File"));
 GtkWidget *open_item = gtk_image_menu_item_new_from_stock(GTK_STOCK_OPEN, NULL);
 gtk_widget_set_tooltip_text(GTK_WIDGET(open_item), _("Open file to analyse"));
 g_signal_connect(G_OBJECT(open_item), "activate", G_CALLBACK(open_item_click), NULL);
 save_item = gtk_image_menu_item_new_from_stock(GTK_STOCK_SAVE_AS, NULL);
 gtk_widget_set_sensitive(GTK_WIDGET(save_item), FALSE);
 gtk_widget_add_accelerator(open_item, "activate", accel_group, GDK_O, 
							GDK_CONTROL_MASK, GTK_ACCEL_VISIBLE);
 GtkWidget *save_menu = gtk_menu_new();
 GtkWidget *save_words_img = gtk_image_new_from_stock(GTK_STOCK_SAVE_AS, GTK_ICON_SIZE_MENU);
 GtkWidget *save_words_item = gtk_image_menu_item_new_with_label(_("Words"));
 gtk_image_menu_item_set_image(GTK_IMAGE_MENU_ITEM(save_words_item), save_words_img);
 gtk_widget_set_tooltip_text(GTK_WIDGET(save_words_item), _("Save list of words to file"));
 g_signal_connect(G_OBJECT(save_words_item), "activate", G_CALLBACK(save_words_item_click), NULL);
 gtk_widget_add_accelerator(save_words_item, "activate", accel_group, GDK_S, 
							GDK_CONTROL_MASK, GTK_ACCEL_VISIBLE);
 GtkWidget *save_text_img = gtk_image_new_from_stock(GTK_STOCK_SAVE_AS, GTK_ICON_SIZE_MENU);
 GtkWidget *save_text_item = gtk_image_menu_item_new_with_label(_("Text"));
 gtk_image_menu_item_set_image(GTK_IMAGE_MENU_ITEM(save_text_item), save_text_img);
 gtk_widget_set_tooltip_text(GTK_WIDGET(save_text_item), _("Save text from which words were picked up"));
 g_signal_connect(G_OBJECT(save_text_item), "activate", G_CALLBACK(save_text_item_click), NULL);
 gtk_menu_item_set_submenu(GTK_MENU_ITEM(save_item), save_menu);
 gtk_menu_shell_append(GTK_MENU_SHELL(save_menu), save_words_item);
 gtk_menu_shell_append(GTK_MENU_SHELL(save_menu), save_text_item);
 GtkWidget *quit_item = gtk_image_menu_item_new_from_stock(GTK_STOCK_QUIT, NULL);
 gtk_widget_set_tooltip_text(GTK_WIDGET(quit_item), _("Close the program"));
 g_signal_connect(G_OBJECT(quit_item), "activate", G_CALLBACK(quit_item_click), NULL);
 gtk_widget_add_accelerator(quit_item, "activate", accel_group, GDK_Q, 
							GDK_CONTROL_MASK, GTK_ACCEL_VISIBLE);
 gtk_menu_item_set_submenu(GTK_MENU_ITEM(file_item), file_menu);
 gtk_menu_shell_append(GTK_MENU_SHELL(file_menu), open_item);
 gtk_menu_shell_append(GTK_MENU_SHELL(file_menu), save_item);
 gtk_menu_shell_append(GTK_MENU_SHELL(file_menu), sep_item);
 gtk_menu_shell_append(GTK_MENU_SHELL(file_menu), quit_item);
 gtk_menu_shell_append(GTK_MENU_SHELL(menu_bar), file_item);
 gtk_widget_show_all(file_item);

 GtkWidget *edit_menu = gtk_menu_new();
 GtkWidget *edit_item = gtk_menu_item_new_with_mnemonic(_("_Edit"));
 GtkWidget *dict_item = gtk_image_menu_item_new_with_label(_("Dictionary"));
 gtk_widget_set_tooltip_text(GTK_WIDGET(dict_item), _("View and edit dictionary"));
 gtk_widget_add_accelerator(dict_item, "activate", accel_group, GDK_D, 
							GDK_CONTROL_MASK, GTK_ACCEL_VISIBLE);
 GtkWidget *edit_img = gtk_image_new_from_stock(GTK_STOCK_EDIT, GTK_ICON_SIZE_MENU);
 gtk_image_menu_item_set_image(GTK_IMAGE_MENU_ITEM(dict_item), edit_img);
 g_signal_connect(G_OBJECT(dict_item), "activate", G_CALLBACK(dict_item_click), NULL);
 GtkWidget *pref_item = gtk_image_menu_item_new_from_stock(GTK_STOCK_PREFERENCES, NULL);
 gtk_widget_set_tooltip_text(GTK_WIDGET(pref_item), _("Change application settings"));
 g_signal_connect(G_OBJECT(pref_item), "activate", G_CALLBACK(pref_item_click), NULL);
 gtk_menu_item_set_submenu(GTK_MENU_ITEM(edit_item), edit_menu);
 gtk_menu_shell_append(GTK_MENU_SHELL(edit_menu), dict_item);
 gtk_menu_shell_append(GTK_MENU_SHELL(edit_menu), pref_item);
 gtk_menu_shell_append(GTK_MENU_SHELL(menu_bar), edit_item);
 gtk_widget_show_all(edit_item);

 GtkWidget *help_menu = gtk_menu_new();
 GtkWidget *help_item = gtk_menu_item_new_with_mnemonic(_("_Help"));
 GtkWidget *about_item = gtk_image_menu_item_new_from_stock(GTK_STOCK_ABOUT, NULL);
 gtk_widget_set_tooltip_text(GTK_WIDGET(about_item), _("About the program"));
 g_signal_connect(G_OBJECT(about_item), "activate", G_CALLBACK(about_item_click), NULL);
 gtk_menu_item_set_submenu(GTK_MENU_ITEM(help_item), help_menu);
 gtk_menu_shell_append(GTK_MENU_SHELL(help_menu), about_item);
 gtk_menu_shell_append(GTK_MENU_SHELL(menu_bar), help_item);
 gtk_widget_show_all(help_item);

 gtk_box_pack_start(GTK_BOX(vbox), menu_bar, FALSE, FALSE, 2);
 gtk_widget_show(menu_bar);

 GtkWidget *toolbar = gtk_toolbar_new();
 gtk_toolbar_set_style(GTK_TOOLBAR(toolbar), GTK_TOOLBAR_ICONS);
 GtkToolItem *open = gtk_tool_button_new_from_stock(GTK_STOCK_OPEN);
 gtk_widget_set_tooltip_text(GTK_WIDGET(open), _("Open file to analyse"));
 g_signal_connect(G_OBJECT(open), "clicked", G_CALLBACK(open_item_click), NULL);
 gtk_toolbar_insert(GTK_TOOLBAR(toolbar), open, -1);
 save = gtk_tool_button_new_from_stock(GTK_STOCK_SAVE);
 gtk_widget_set_has_tooltip(GTK_WIDGET(save), FALSE);
 gtk_widget_set_tooltip_text(GTK_WIDGET(save), _("Save list of words to file"));
 gtk_widget_set_sensitive(GTK_WIDGET(save), FALSE);
 g_signal_connect(G_OBJECT(save), "clicked", G_CALLBACK(save_words_item_click), NULL);
 gtk_toolbar_insert(GTK_TOOLBAR(toolbar), save, -1);
 GtkToolItem *tool_sep1 = gtk_separator_tool_item_new();
 gtk_toolbar_insert(GTK_TOOLBAR(toolbar), tool_sep1, -1); 
 GtkToolItem *dict = gtk_tool_button_new_from_stock(GTK_STOCK_EDIT);
 gtk_widget_set_tooltip_text(GTK_WIDGET(dict), _("View and edit dictionary"));
 g_signal_connect(G_OBJECT(dict), "clicked", G_CALLBACK(dict_item_click), NULL);
 gtk_toolbar_insert(GTK_TOOLBAR(toolbar), dict, -1);
 GtkToolItem *tool_sep2 = gtk_separator_tool_item_new();
 gtk_toolbar_insert(GTK_TOOLBAR(toolbar), tool_sep2, -1);  
 GtkToolItem *quit =  gtk_tool_button_new_from_stock(GTK_STOCK_QUIT);
 gtk_widget_set_tooltip_text(GTK_WIDGET(quit), _("Close the program"));
 g_signal_connect(G_OBJECT(quit), "clicked", G_CALLBACK(quit_item_click), NULL);
 gtk_toolbar_insert(GTK_TOOLBAR(toolbar), quit, -1);
 gtk_box_pack_start(GTK_BOX(vbox), toolbar, FALSE, FALSE, 2); 
 gtk_widget_show_all(toolbar);

 GtkWidget *hpaned = gtk_hpaned_new();
 gtk_box_pack_start(GTK_BOX(vbox), hpaned, TRUE, TRUE, 0);
 gtk_paned_set_position(GTK_PANED(hpaned), 200);
 gtk_widget_show(hpaned);

 GtkWidget *word_sc_win = gtk_scrolled_window_new(NULL, NULL);
 gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW(word_sc_win), GTK_POLICY_AUTOMATIC, GTK_POLICY_AUTOMATIC);
 gtk_paned_add1(GTK_PANED(hpaned), word_sc_win);
 gtk_scrolled_window_set_shadow_type(GTK_SCROLLED_WINDOW(word_sc_win), GTK_SHADOW_IN);
 gtk_widget_show(word_sc_win);

 word_list = gtk_tree_view_new();
 gtk_tree_selection_set_mode(gtk_tree_view_get_selection(GTK_TREE_VIEW(word_list)), GTK_SELECTION_SINGLE);
 gtk_tree_view_set_headers_visible(GTK_TREE_VIEW(word_list), FALSE);
 gtk_tree_view_set_rules_hint(GTK_TREE_VIEW(word_list), TRUE);
 g_signal_connect(G_OBJECT(word_list), "button-press-event", G_CALLBACK(popup_by_click), NULL);
 g_signal_connect(G_OBJECT(word_list), "popup-menu", G_CALLBACK(popup_by_keybd), NULL);
 store_model = gtk_list_store_new(N_COLUMNS, G_TYPE_STRING);
 gtk_tree_view_set_model(GTK_TREE_VIEW(word_list), GTK_TREE_MODEL(store_model));
 g_object_unref(store_model);
 GtkCellRenderer *renderer = gtk_cell_renderer_text_new();
 g_object_set(renderer, "editable", TRUE, NULL);
 GtkTreeViewColumn *column = gtk_tree_view_column_new_with_attributes("List Items", renderer, "text", WORD_ITEM, NULL);
 gtk_tree_view_append_column(GTK_TREE_VIEW(word_list), column);
 gtk_container_add(GTK_CONTAINER(word_sc_win), word_list);
 gtk_widget_show(word_list);

 GtkWidget *text_vbox = gtk_vbox_new(FALSE, 0);
 gtk_paned_add2(GTK_PANED(hpaned), text_vbox);
 gtk_container_set_border_width(GTK_CONTAINER(text_vbox), 2);
 gtk_widget_show(text_vbox);

 GtkWidget *sentences_sc_win = gtk_scrolled_window_new(NULL, NULL);
 gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW(sentences_sc_win), GTK_POLICY_AUTOMATIC, GTK_POLICY_AUTOMATIC);
 gtk_box_pack_start(GTK_BOX(text_vbox), sentences_sc_win, TRUE, TRUE, 0);
 gtk_scrolled_window_set_shadow_type(GTK_SCROLLED_WINDOW(sentences_sc_win), GTK_SHADOW_IN);
 gtk_widget_show(sentences_sc_win);

 sentences_text = gtk_text_view_new();
 GtkTextBuffer *buffer = gtk_text_view_get_buffer(GTK_TEXT_VIEW(sentences_text));
 gtk_text_view_set_editable(GTK_TEXT_VIEW(sentences_text), FALSE);
 gtk_text_buffer_create_tag(buffer, "yellow-bg", "background", "yellow", NULL);
 gtk_text_buffer_create_tag(buffer, "green-bg", "background", "green", NULL);
 gtk_container_add(GTK_CONTAINER(sentences_sc_win), sentences_text);
 gtk_widget_show(sentences_text);

 GtkWidget *text_hbox = gtk_hbox_new(FALSE, 0);
 gtk_box_pack_end(GTK_BOX(text_vbox), text_hbox, FALSE, FALSE, 0);
 gtk_widget_show(text_hbox);

 find_label = gtk_label_new(_("Find: "));
 gtk_box_pack_start(GTK_BOX(text_hbox), find_label, FALSE, FALSE, 0);
 gtk_widget_set_sensitive(GTK_WIDGET(find_label), FALSE);
 gtk_widget_show(find_label);

 find_entry = gtk_entry_new_with_max_length(WORDLENGTH);
 gtk_box_pack_start(GTK_BOX(text_hbox), find_entry, FALSE, FALSE, 0);
 gtk_widget_set_sensitive(GTK_WIDGET(find_entry), FALSE);
 g_signal_connect(G_OBJECT(find_entry), "activate", G_CALLBACK(on_find_text_change), NULL);
 g_signal_connect(G_OBJECT(find_entry), "key-press-event", G_CALLBACK(find_entry_key_press), NULL);
 gtk_widget_show(find_entry);

 find_btn = gtk_button_new();
 GtkWidget *image = gtk_image_new_from_stock(GTK_STOCK_FIND, GTK_ICON_SIZE_BUTTON);
 gtk_container_add(GTK_CONTAINER(find_btn), image);
 gtk_widget_show(image);
 gtk_box_pack_start(GTK_BOX(text_hbox), find_btn, FALSE, FALSE, 0);
 gtk_widget_set_tooltip_text(GTK_WIDGET(find_btn), _("Find word"));
 gtk_widget_set_sensitive(GTK_WIDGET(find_btn), FALSE);
 g_signal_connect(G_OBJECT(find_btn), "clicked", G_CALLBACK(on_find_text_change), NULL);
 gtk_widget_show(find_btn);

 prev_btn = gtk_button_new();
 image = gtk_image_new_from_stock(GTK_STOCK_GO_BACK, GTK_ICON_SIZE_BUTTON);
 gtk_container_add(GTK_CONTAINER(prev_btn), image);
 gtk_widget_show(image);
 gtk_box_pack_start(GTK_BOX(text_hbox), prev_btn, FALSE, FALSE, 0);
 gtk_widget_set_tooltip_text(GTK_WIDGET(prev_btn), _("Previous word"));
 gtk_widget_set_sensitive(GTK_WIDGET(prev_btn), FALSE);
 g_signal_connect(G_OBJECT(prev_btn), "clicked", G_CALLBACK(find_prev_btn_click), (gpointer) find_entry);
 gtk_widget_show(prev_btn);

 next_btn = gtk_button_new();
 image = gtk_image_new_from_stock(GTK_STOCK_GO_FORWARD, GTK_ICON_SIZE_BUTTON);
 gtk_container_add(GTK_CONTAINER(next_btn), image);
 gtk_widget_show(image);
 gtk_box_pack_start(GTK_BOX(text_hbox), next_btn, FALSE, FALSE, 0);
 gtk_widget_set_tooltip_text(GTK_WIDGET(next_btn), _("Next word"));
 gtk_widget_set_sensitive(GTK_WIDGET(next_btn), FALSE);
 g_signal_connect(G_OBJECT(next_btn), "clicked", G_CALLBACK(find_next_btn_click), (gpointer) find_entry);
 gtk_widget_show(next_btn);

 clear_btn = gtk_button_new();
 image = gtk_image_new_from_stock(GTK_STOCK_CLEAR, GTK_ICON_SIZE_BUTTON);
 gtk_container_add(GTK_CONTAINER(clear_btn), image);
 gtk_widget_show(image);
 gtk_box_pack_start(GTK_BOX(text_hbox), clear_btn, FALSE, FALSE, 0);
 gtk_widget_set_tooltip_text(GTK_WIDGET(clear_btn), _("Cleanup highlighting"));
 gtk_widget_set_sensitive(GTK_WIDGET(clear_btn), FALSE);
 g_signal_connect(G_OBJECT(clear_btn), "clicked", G_CALLBACK(clear_btn_click), (gpointer) find_entry);
 gtk_widget_show(clear_btn);

 statusbar = gtk_statusbar_new();
 gtk_box_pack_start(GTK_BOX(vbox), statusbar, FALSE, FALSE, 0);
 id = gtk_statusbar_get_context_id(GTK_STATUSBAR(statusbar), "main_context");
 find_id = gtk_statusbar_get_context_id(GTK_STATUSBAR(statusbar), "find_context");
 gtk_statusbar_push(GTK_STATUSBAR(statusbar), id, _("Open file to pick up words from it..."));
 gtk_widget_show(statusbar);

 gtk_window_add_accel_group(GTK_WINDOW(main_window), accel_group);

 GtkTextIter iter;
 gchar welcome_msg[] = {N_("\n\t\t\tWelcome to WordXtract\n\n  \tThis is a program which helps to pick up list of\n  "
						"unique words from any plain text or subtitle file.\n  "
						"Currently parser supports only English language\n  "
						"but it allows to adjust number of settings which\n  "
						"may be suitable for other languages. Some words can\n  "
						"be excluded according to list of words specified in\n  "
						"your dictionary. Also it allows you to save processed\n  "
						"text and list of words.\n\n  \tYou may find it more effective to work with translators\n  "
						"like StarDict which allows to translate words \"on the fly\"\n  "
						"by binding \"hot key\" for floating window.\n\n  Enjoy!\n")};
 gtk_text_buffer_get_end_iter(buffer, &iter);
 gtk_text_buffer_insert(buffer, &iter, _(welcome_msg), -1);
}

static void set_status_msg()
{
 gtk_statusbar_pop(GTK_STATUSBAR(statusbar), find_id);
}

static gint main_window_close(GtkWidget *widget, GdkEvent *event, gpointer main_window)
{
 gboolean response = FALSE;
 if (exit_query) {
	GtkWidget *dialog = gtk_message_dialog_new(main_window, GTK_DIALOG_DESTROY_WITH_PARENT,
								 GTK_MESSAGE_QUESTION, GTK_BUTTONS_YES_NO, CONFIRM_QUIT_MSG); 
	gtk_window_set_title(GTK_WINDOW(dialog), CONFIRM_MSG_CAPTION);
	if (gtk_dialog_run(GTK_DIALOG(dialog)) == GTK_RESPONSE_NO)
		response = TRUE;
	gtk_widget_destroy(dialog);
 }
 return response;	
}

static void main_window_destroy(GtkWidget *widget, gpointer data)
{
 gtk_main_quit();
}

static void open_item_click(GtkWidget *widget, gpointer data)
{
 extern void file_error(char *, GtkWidget *, GtkMessageType, char *, char *);

 char *filename;
 FILE *ffile;

/*
	WinAPI's dialogs are familiar to windows users but they aren't compatible with
	gtk_main() loop, so it should be opened into another thread - too much 
	complexity for such simple application:)
*/
/*
 OPENFILENAME ofn;
 char filename[MAX_PATH] = "";
 
 memset(&ofn, 0, sizeof(ofn));
 ofn.lStructSize = sizeof(ofn);
 ofn.hwndOwner = FindWindow(NULL, PROGNAME);
 // Windows' dialog extensions' string
 ofn.lpstrFilter = _("SubRip Files (*.srt)\0*.srt\0Text Files (*.txt)\0*.txt\0");
 ofn.lpstrTitle = "";
 ofn.lpstrFile = filename;
 ofn.nMaxFile = MAX_PATH;
 ofn.Flags = OFN_EXPLORER | OFN_FILEMUSTEXIST | OFN_HIDEREADONLY;
 ofn.lpstrDefExt = "srt";
 if (GetOpenFileName(&ofn)) {
	if (!(ffile = fopen(filename, "r"))) {
		file_error(filename, main_window, GTK_MESSAGE_ERROR, ERR_MSG_CAPTION, ERR_READING_FILE);
		return ;
	}
*/

#ifdef WIN32
 GtkFileFilter *srt_filter = gtk_file_filter_new();
 gtk_file_filter_add_pattern(srt_filter, "*.srt");
 gtk_file_filter_set_name(srt_filter, _("SubRip subtitles (*.srt)"));
 GtkFileFilter *text_filter = gtk_file_filter_new();
 gtk_file_filter_add_pattern(text_filter, "*.txt");
 gtk_file_filter_set_name(text_filter, _("Text files (*.txt)"));
#else
 GtkFileFilter *srt_filter = gtk_file_filter_new();
 gtk_file_filter_add_mime_type(srt_filter, "application/x-subrip");
 gtk_file_filter_set_name(srt_filter, _("SubRip subtitles (*.srt)"));
 GtkFileFilter *text_filter = gtk_file_filter_new();
 gtk_file_filter_add_mime_type(text_filter, "text/plain");
 gtk_file_filter_set_name(text_filter, _("Plain text"));
#endif
 GtkWidget *filedialog = gtk_file_chooser_dialog_new(_("Open File for analysing"), 
							GTK_WINDOW(main_window), GTK_FILE_CHOOSER_ACTION_OPEN,
							GTK_STOCK_CANCEL, GTK_RESPONSE_CANCEL, GTK_STOCK_OPEN,
							GTK_RESPONSE_ACCEPT, NULL);
 gtk_file_chooser_add_filter(GTK_FILE_CHOOSER(filedialog), text_filter);
 gtk_file_chooser_add_filter(GTK_FILE_CHOOSER(filedialog), srt_filter);
 gtk_file_chooser_set_filter(GTK_FILE_CHOOSER(filedialog), srt_filter);

 if (gtk_dialog_run(GTK_DIALOG(filedialog)) == GTK_RESPONSE_ACCEPT) {
	filename = gtk_file_chooser_get_filename(GTK_FILE_CHOOSER(filedialog));
#ifdef WIN32
	gchar *lfilename = g_locale_from_utf8(filename, -1, NULL, NULL, NULL);
	if (!(ffile = fopen(lfilename, "r"))) {
		gtk_widget_destroy(filedialog);
		file_error(lfilename, main_window, GTK_MESSAGE_ERROR, ERR_MSG_CAPTION, ERR_READING_FILE);
		return ;
	}
#else
	if (!(ffile = fopen(filename, "r"))) {
		gtk_widget_destroy(filedialog);
		file_error(filename, main_window, GTK_MESSAGE_ERROR, ERR_MSG_CAPTION, ERR_READING_FILE);
		return ;
	}
#endif
	gtk_list_store_clear(store_model);
	if (words != NULL) {
		free(user_words.by_az);
		free_words(words);
		words = NULL;
	} else {
		gtk_widget_set_sensitive(GTK_WIDGET(find_label), TRUE);
		gtk_widget_set_sensitive(GTK_WIDGET(find_entry), TRUE);
		gtk_widget_set_sensitive(GTK_WIDGET(find_btn), TRUE);
		gtk_widget_set_sensitive(GTK_WIDGET(save_item), TRUE);
		gtk_widget_set_sensitive(GTK_WIDGET(save), TRUE);
	}
	const gchar *filter_name;
	GtkFileFilter *selected_filter = gtk_file_filter_new();
	selected_filter = gtk_file_chooser_get_filter(GTK_FILE_CHOOSER(filedialog));
	filter_name = gtk_file_filter_get_name(GTK_FILE_FILTER(selected_filter));
	clear_sentences();
#ifdef WIN32
	if (!strcmp(filter_name, _("Text files (*.txt)"))) {
		if (process_plain_text(ffile))
			file_error("", main_window, GTK_MESSAGE_WARNING, WARN_MSG_CAPTION, WARN_NON_UTF8_CHARS);
	} else if (!strcmp(filter_name, _("SubRip subtitles (*.srt)"))) {
		if (process_srt(ffile))
			file_error("", main_window, GTK_MESSAGE_WARNING, WARN_MSG_CAPTION, WARN_NON_UTF8_CHARS);
	}
#else
	if (!strcmp(filter_name, _("Plain text"))) {
		if (process_plain_text(ffile))
			file_error("", main_window, GTK_MESSAGE_WARNING, WARN_MSG_CAPTION, WARN_NON_UTF8_CHARS);
	} else if (!strcmp(filter_name, _("SubRip subtitles (*.srt)"))) {
		if (process_srt(ffile))
			file_error("", main_window, GTK_MESSAGE_WARNING, WARN_MSG_CAPTION, WARN_NON_UTF8_CHARS);
	}
#endif
	user_words = get_sorted(words);
	fill_list(user_words.by_az);
	fclose(ffile);
	gchar *file = malloc(strlen(filename)+1);
	file = basename(filename);
	gchar *status_msg = malloc(strlen(file) + strlen(_("%d words were picked up from '%s'"))+1);
	sprintf(status_msg, _("%d words were picked up from '%s'"), user_words.count, file);
	gtk_statusbar_pop(GTK_STATUSBAR(statusbar), id);
	gtk_statusbar_push(GTK_STATUSBAR(statusbar), id, status_msg);
	g_free(filename);
 }
 gtk_widget_destroy(filedialog);
}

static void save_words_item_click(GtkWidget *widget, gpointer data)
{
 extern void file_error(char *, GtkWidget *, GtkMessageType, char *, char *);

 struct stat st;
 FILE *savefile;
 gchar *filename;
 gchar *errstr;

 GtkWidget *filedialog = gtk_file_chooser_dialog_new(_("Save list of words as..."),
									GTK_WINDOW(main_window), GTK_FILE_CHOOSER_ACTION_SAVE,
									GTK_STOCK_CANCEL, GTK_RESPONSE_CANCEL, GTK_STOCK_SAVE, 
									GTK_RESPONSE_ACCEPT, NULL);
 if (gtk_dialog_run(GTK_DIALOG(filedialog)) == GTK_RESPONSE_ACCEPT) {
	filename = gtk_file_chooser_get_filename(GTK_FILE_CHOOSER(filedialog));

#ifdef WIN32
	/* converting filename to windows' locale codeset */
	gchar *lfilename = g_locale_from_utf8(filename, -1, NULL, NULL, NULL);
	/* to add or not to add ".txt" or ".srt" extension to the filename? */
#else
	/* on linux we just refer to the orginal string */
	gchar *lfilename = filename;
#endif

	gint resp = GTK_RESPONSE_YES;
	/* whether file already exists or not? */
	if (!stat(lfilename, &st)) {
		char *errstr = malloc(strlen(filename) + strlen(WARN_ALREADY_EXISTS)+1);
		strcpy(errstr, filename);
		strcat(errstr, WARN_ALREADY_EXISTS);
		GtkWidget *msg = gtk_message_dialog_new(GTK_WINDOW(main_window), 
								GTK_DIALOG_DESTROY_WITH_PARENT,	GTK_MESSAGE_WARNING,
								GTK_BUTTONS_YES_NO, errstr);
		gtk_window_set_title(GTK_WINDOW(msg), WARN_MSG_CAPTION);
		resp = gtk_dialog_run(GTK_DIALOG(msg));
		gtk_widget_destroy(msg);
		free(errstr);
	}
	if (resp == GTK_RESPONSE_YES) {
		if (!(savefile = fopen(lfilename, "w"))) {
			gtk_widget_destroy(filedialog);
#ifdef WIN32
			g_free(lfilename);
#endif
			file_error(filename, main_window, GTK_MESSAGE_ERROR, ERR_MSG_CAPTION, ERR_SAVING_WORDS);
			return ;
		}
		save_words(savefile, user_words, save_user_words);
		fclose(savefile);
	}
#ifdef WIN32
			g_free(lfilename);
#endif
 }
 gtk_widget_destroy(filedialog);
}

static void save_text_item_click(GtkWidget *widget, gpointer data)
{
 extern void file_error(char *, GtkWidget *, GtkMessageType, char *, char *);
 FILE *savefile;
 gchar *filename;
 gchar *errstr;
 gchar *text;

 GtkWidget *filedialog = gtk_file_chooser_dialog_new(_("Save text as..."),
									GTK_WINDOW(main_window), GTK_FILE_CHOOSER_ACTION_SAVE,
									GTK_STOCK_CANCEL, GTK_RESPONSE_CANCEL, GTK_STOCK_SAVE, 
									GTK_RESPONSE_ACCEPT, NULL);
 if (gtk_dialog_run(GTK_DIALOG(filedialog)) == GTK_RESPONSE_ACCEPT) {
	filename = gtk_file_chooser_get_filename(GTK_FILE_CHOOSER(filedialog));
#ifdef WIN32
	gchar *lfilename = g_locale_from_utf8(filename, -1, NULL, NULL, NULL);
	if (!(savefile = fopen(lfilename, "w"))) {
		gtk_widget_destroy(filedialog);
		file_error(filename, main_window, GTK_MESSAGE_ERROR, ERR_MSG_CAPTION, ERR_SAVING_WORDS);
		return ;
	}
	g_free(lfilename);
#else
	if (!(savefile = fopen(filename, "w"))) {
		gtk_widget_destroy(filedialog);
		file_error(filename, main_window, GTK_MESSAGE_ERROR, ERR_MSG_CAPTION, ERR_SAVING_TEXT);
		return ;
	}
#endif
	GtkTextIter start, end;
	GtkTextBuffer *buffer = gtk_text_view_get_buffer(GTK_TEXT_VIEW(sentences_text));
	gtk_text_buffer_get_start_iter(buffer, &start);
	gtk_text_buffer_get_end_iter(buffer, &end);
	text = gtk_text_buffer_get_text(buffer, &start, &end, FALSE);
	fprintf(savefile, text);
	fclose(savefile);
 }
 gtk_widget_destroy(filedialog);
}

static void quit_item_click(GtkWidget *widget, gpointer data)
{
 gboolean response = FALSE;
 if (exit_query) {
	GtkWidget *dialog = gtk_message_dialog_new(GTK_WINDOW(main_window), GTK_DIALOG_DESTROY_WITH_PARENT,
						 GTK_MESSAGE_QUESTION, GTK_BUTTONS_YES_NO, CONFIRM_QUIT_MSG); 
	gtk_window_set_title(GTK_WINDOW(dialog), CONFIRM_MSG_CAPTION);
	if (gtk_dialog_run(GTK_DIALOG(dialog)) == GTK_RESPONSE_NO)
		response = TRUE;
	gtk_widget_destroy(dialog);
 } 
 if (!response)
	gtk_main_quit();
}

static void dict_item_click(GtkWidget *widget, gpointer data)
{
 create_dict_win();
}

static void pref_item_click(GtkWidget *widget, gpointer data)
{
 create_conf_win();
}

static void website_click(GtkAboutDialog *about, const gchar *link_, gpointer data)
{
 GError* error = NULL;
 gchar *argv[3];
 argv[0] = "xdg-open";
 argv[1] = (gchar*)link_;
 argv[2] = NULL;

 g_spawn_async(NULL, argv, NULL, G_SPAWN_SEARCH_PATH, NULL, NULL, NULL, &error);
 if (error != NULL) {
	fprintf(stderr, _("Failed to open URI: %s\n"), error->message);
	g_error_free(error);
 }
}

static void about_item_click(GtkWidget *widget, gpointer data)
{
 struct stat st;
 const gchar url[] = {"http://code.google.com/p/wordxtract/"};
 const gchar comment[] = {N_("WordXtract is a text and subtitle parser\n which shows list of unique words\n from the file.")}; 
 const gchar copyright[] = {"Copyright \xc2\xa9 2009 by Borisov Alexandr"};
 const gchar gplv3[] = {N_("WordXtract is free software: you can\n"
						"redistribute it and/or modify it under the\n"
						"terms of the GNU General Public License as\n"
						"published by the Free Software Foundation,\n"
						"either version 3 of the License, or (at your\n"
						"option) any later version.\n\n"
						"WordXtract is distributed in the hope that\n"
						"it will be useful, but WITHOUT ANY WARRANTY;\n"
						"without even the implied warranty of\n"
						"MERCHANTABILITY or FITNESS FOR A PARTICULAR\n"
						"PURPOSE. See the GNU General Public License\n"
						"for more details.\n\n"
						"You should have received a copy of the GNU\n"
						"General Public License along with WordXtract.\n"
						"If not, see <http://www.gnu.org/licenses/>.")};
 const gchar *authors[] = {"Borisov Alexandr <b0ric.alex@gmail.com>", NULL};
 const gchar *artists[] = {"Petruhin Kirill <19non91@gmail.com>", NULL};
 const gchar translators[] = {N_("ru: Borisov Alexandr <b0ric.alex@gmail.com>")};

 char *logofile = malloc(strlen(WORDXTRACT_DATA_DIR) + strlen("/pixmaps/wordxtract1.png")+1);
 strcpy(logofile, WORDXTRACT_DATA_DIR);
 strcat(logofile, "/pixmaps/wordxtract1.png");

 GError *error = NULL;
 GdkPixbuf *logo = gdk_pixbuf_new_from_file(logofile, &error);
 if (!logo) {
	fprintf(stderr, "%s\n", error->message);
	g_error_free(error);
 }
 gtk_about_dialog_set_url_hook(website_click, NULL, NULL);
 gtk_show_about_dialog(GTK_WINDOW(main_window), "title", "About",
												"program-name", PROGNAME,
			      								"version", VERSION,
												"website", url,
												"website-label", "WordXtract website",
											    "comments", _(comment),
			  								    "copyright", copyright,
												"license", _(gplv3),
												"wrap-license", TRUE,
			      								"authors", authors,
											    "artists", artists,
												"translator-credits", _(translators),
			      								"logo", logo, NULL);
 free(logofile);
}

static gboolean popup_by_click(GtkWidget *word_list, GdkEventButton *event, gpointer data)
{
 GtkTreePath *path;

 if (event->type == GDK_BUTTON_PRESS && event->button == 3) {
	GtkTreeSelection *selection = gtk_tree_view_get_selection(GTK_TREE_VIEW(word_list));
	if (gtk_tree_selection_get_selected(GTK_TREE_SELECTION(selection), NULL, NULL)) {
		if (gtk_tree_view_get_path_at_pos(GTK_TREE_VIEW(word_list), (gint) event->x, 
										  (gint) event->y, &path, NULL, NULL, NULL)) {
			gtk_tree_selection_unselect_all(selection);
			gtk_tree_selection_select_path(selection, path);
			word_list_popup(GTK_WIDGET(word_list), event);
			return TRUE;
		}
	}
 }
return FALSE;
}

static gboolean popup_by_keybd(GtkWidget *word_list, gpointer data)
{
 word_list_popup(word_list, NULL);
 return TRUE;
}

static void word_list_popup(GtkWidget *widget, GdkEventButton *event)
{
 GtkWidget *popup_menu = gtk_menu_new();
 GtkWidget *show_sent_item = gtk_image_menu_item_new_with_label(_("Search word"));
 gtk_menu_shell_append(GTK_MENU_SHELL(popup_menu), show_sent_item);
 gtk_widget_show(show_sent_item);
 g_signal_connect(G_OBJECT(show_sent_item), "activate", G_CALLBACK(show_sent_item_click), NULL);
 gtk_widget_set_tooltip_text(GTK_WIDGET(show_sent_item), _("Show sentenses word appears in"));
 GtkWidget *find_image = gtk_image_new_from_stock("gtk-find", GTK_ICON_SIZE_MENU);
 gtk_widget_show(find_image);
 gtk_image_menu_item_set_image(GTK_IMAGE_MENU_ITEM(show_sent_item), find_image);
 GtkWidget *add_to_dict_item = gtk_image_menu_item_new_with_label(_("Add to the dictionary"));
 gtk_widget_show(add_to_dict_item);
 gtk_menu_shell_append(GTK_MENU_SHELL(popup_menu), add_to_dict_item);
 g_signal_connect(add_to_dict_item, "activate", G_CALLBACK(add_to_dict_item_click), NULL);
 gtk_widget_set_tooltip_text(GTK_WIDGET(add_to_dict_item), _("Add word to the dictionary"));
 GtkWidget *add_image = gtk_image_new_from_stock("gtk-add", GTK_ICON_SIZE_MENU);
 gtk_widget_show(add_image);
 gtk_image_menu_item_set_image(GTK_IMAGE_MENU_ITEM(add_to_dict_item), add_image);
 gtk_widget_show(popup_menu);
 gtk_menu_popup(GTK_MENU(popup_menu), NULL, NULL, NULL, NULL,
				(event != NULL) ? event->button : 0, gdk_event_get_time((GdkEvent*)event));
}

static void show_sent_item_click(GtkWidget *widget, gpointer data)
{
 GtkTreeModel *model;
 GtkTreeIter iter;
 gchar *word;

 GtkTreeSelection *selection = gtk_tree_view_get_selection(GTK_TREE_VIEW(word_list));
 if (gtk_tree_selection_get_selected(selection, &model, &iter)) {
	gtk_tree_model_get(model, &iter, 0, &word, -1);
	set_find_text(word);
	change_search_navigation_state(TRUE);
	search_sentences(word);
	g_free(word);
 }
}

static void add_to_dict_item_click(GtkWidget *widget, gpointer data)
{
 GtkTreeModel *model;
 GtkTreeIter iter;
 gchar *word;

 GtkTreeSelection *selection = gtk_tree_view_get_selection(GTK_TREE_VIEW(word_list));
 if (gtk_tree_selection_get_selected(selection, &model, &iter)) {
	gtk_tree_model_get(model, &iter, 0, &word, -1);
	dict = add_word(dict, word);
	g_free(word);
 }
}

static void fill_list(char **word_list)
{
 GtkTreeIter iter;
 int cnt, i;

 cnt = get_words_count(words);
 for (i = 0; i < cnt; i++) {
	gtk_list_store_append(store_model, &iter);
	gtk_list_store_set(store_model, &iter, WORD_ITEM, *word_list, -1);
	word_list++;
 }
}

static void change_search_navigation_state(gboolean state)
{
 gtk_widget_set_sensitive(GTK_WIDGET(prev_btn), state);
 gtk_widget_set_sensitive(GTK_WIDGET(next_btn), state);
 gtk_widget_set_sensitive(GTK_WIDGET(clear_btn), state);
}

void add_sentence(char *sentence)
{
 GtkTextIter iter;

 GtkTextBuffer *buffer = gtk_text_view_get_buffer(GTK_TEXT_VIEW(sentences_text));
 gtk_text_buffer_get_end_iter(buffer, &iter);
 gtk_text_buffer_insert(buffer, &iter, (gchar*) sentence, -1);
}

static gboolean find_entry_key_press(GtkWidget *entry, GdkEventKey *event, gpointer data)
{
 if (event->type == GDK_KEY_PRESS)
	if ((event->length)||(event->keyval == GDK_BackSpace)||(event->keyval == GDK_Delete))
		set_status_msg();
 return FALSE;
}

static void on_find_text_change(GtkWidget *widget, gpointer data)
{
 const gchar *find_text;

 find_text = gtk_entry_get_text(GTK_ENTRY(find_entry));
 change_search_navigation_state(strcmp(find_text, ""));
 if (strcmp(find_text, "")) 
	search_sentences(find_text);
}

static void find_next_btn_click(GtkWidget *widget, gpointer entry)
{
 const gchar *find_text = NULL;

 find_text = gtk_entry_get_text(GTK_ENTRY(entry));
 find_other(find_text, SEARCH_FORWARD);
}

static void find_prev_btn_click(GtkWidget *widget, gpointer entry)
{
 const gchar *find_text = NULL;

 find_text = gtk_entry_get_text(GTK_ENTRY(entry));
 find_other(find_text, SEARCH_BACKWARD);
}

static void clear_btn_click(GtkWidget *widget, gpointer data)
{
 clear_search_tags();
 gtk_entry_set_text(GTK_ENTRY(find_entry), "");
 set_status_msg();
 change_search_navigation_state(FALSE);
}

void clear_sentences()
{
 GtkTextBuffer *buffer;
 gchar text[] = "";

 buffer = gtk_text_view_get_buffer(GTK_TEXT_VIEW(sentences_text));
 gtk_text_buffer_set_text(buffer, text, -1);
}

static void search_sentences(const gchar *word)
{
 GtkTextIter find_start, find_end;
 GtkTextIter match_start, match_end;
 int offset, cnt = 0;

 GtkTextBuffer *buffer = gtk_text_view_get_buffer(GTK_TEXT_VIEW(sentences_text));
 gtk_text_buffer_get_start_iter(buffer, &find_start);
 gtk_text_buffer_get_end_iter(buffer, &find_end);
 gtk_text_buffer_remove_tag_by_name(buffer, "green-bg", &find_start, &find_end); 
 gtk_text_buffer_remove_tag_by_name(buffer, "yellow-bg", &find_start, &find_end);
 while (gtk_text_iter_search_word(&find_start, word, GTK_TEXT_SEARCH_TEXT_ONLY| 
									 GTK_TEXT_SEARCH_VISIBLE_ONLY, &match_start,
									 &match_end, NULL, SEARCH_FORWARD)) {
	gtk_text_buffer_apply_tag_by_name(buffer, "yellow-bg", &match_start, &match_end);
	offset = gtk_text_iter_get_offset(&match_end);
 	gtk_text_buffer_get_iter_at_offset(buffer, &find_start, offset);
	cnt++;
 }

 if (!cnt) {
	char status_msg[50] = {0};
	sprintf(status_msg, _("Word '%s' was not found"), word);
	gtk_statusbar_pop(GTK_STATUSBAR(statusbar), find_id);
	gtk_statusbar_push(GTK_STATUSBAR(statusbar), find_id, (gchar *) status_msg);
 }

 gtk_text_buffer_get_start_iter(buffer, &find_start);
 gtk_text_buffer_get_end_iter(buffer, &find_end);
 if (gtk_text_iter_search_word(&find_start, word, GTK_TEXT_SEARCH_TEXT_ONLY| 
								 GTK_TEXT_SEARCH_VISIBLE_ONLY, &match_start,
								 &match_end, NULL, SEARCH_FORWARD)) {
	gtk_text_buffer_apply_tag_by_name(buffer, "green-bg", &match_start, &match_end);
	current_highlighted_end = gtk_text_iter_get_offset(&match_end);
	current_highlighted_start = gtk_text_iter_get_offset(&match_start);
	gtk_text_view_scroll_to_iter(GTK_TEXT_VIEW(sentences_text), &match_start, 0.0, TRUE, 0.5, 0.5);	
 }
}

static void set_find_text(gchar *text)
{
 gtk_entry_set_text(GTK_ENTRY(find_entry), text);
}

static void clear_search_tags()
{
 GtkTextIter start, end;

 GtkTextBuffer *buffer = gtk_text_view_get_buffer(GTK_TEXT_VIEW(sentences_text));
 gtk_text_buffer_get_start_iter(buffer, &start);
 gtk_text_buffer_get_end_iter(buffer, &end);
 gtk_text_buffer_remove_tag_by_name(buffer, "yellow-bg", &start, &end);
 gtk_text_buffer_remove_tag_by_name(buffer, "green-bg", &start, &end);
}

/* We won't need this abomination if GTK developers implement 
 * SEARCH_CASE_INSENSITIVE flag in GtkTextSearchFlags struct
 * which is in their TODO list
 */
static gboolean gtk_text_iter_search_word(const GtkTextIter *iter, const gchar *str,
                                   GtkTextSearchFlags flags, GtkTextIter *match_start,
                                   GtkTextIter *match_end, const GtkTextIter *limit,
								   const int direction)
{
 GtkTextIter match_start_wcap, match_end_wcap;
 gboolean result = FALSE;
 int offset, offset_wcap;
 gchar *str_wcap;

 str_wcap = malloc((strlen(str)+1)*sizeof(gchar));
 strcpy(str_wcap, str);
 /* We've lowered only first letter, so there are only two cases*/
 *str_wcap = toupper(*str_wcap);
 if (direction == SEARCH_FORWARD) {
	if (gtk_text_iter_forward_search(iter, str, flags, match_start, match_end, limit))
		offset = gtk_text_iter_get_offset(match_start);
	else
		offset = -1;
	if (gtk_text_iter_forward_search(iter, str_wcap, flags, &match_start_wcap, &match_end_wcap, limit))
		offset_wcap = gtk_text_iter_get_offset(&match_start_wcap);
	else
		offset_wcap = -1;
	if (((offset_wcap < offset)||(offset == -1))&&(offset_wcap != -1)) {
		*match_start = match_start_wcap;
		*match_end = match_end_wcap;
	}
 } else if (direction == SEARCH_BACKWARD) {
	if (gtk_text_iter_backward_search(iter, str, flags, match_start, match_end, limit))
		offset = gtk_text_iter_get_offset(match_start);
	else
		offset = -1;
	if (gtk_text_iter_backward_search(iter, str_wcap, flags, &match_start_wcap, &match_end_wcap, limit))
		offset_wcap = gtk_text_iter_get_offset(&match_start_wcap);
	else
		offset_wcap = -1;
	if (offset < offset_wcap) {
		*match_start = match_start_wcap;
		*match_end = match_end_wcap;
	}
 }
 free(str_wcap);
 if ((offset_wcap != -1)||(offset != -1))
	result = TRUE;
 return result;
}

static void find_other(const gchar *word, const int direction)
{
 GtkTextIter find_start, find_end;
 GtkTextIter match_start, match_end;

 GtkTextBuffer *buffer = gtk_text_view_get_buffer(GTK_TEXT_VIEW(sentences_text));
 gtk_text_buffer_get_start_iter(buffer, &find_start);
 gtk_text_buffer_get_end_iter(buffer, &find_end);
 gtk_text_buffer_remove_tag_by_name(buffer, "green-bg", &find_start, &find_end);
 if (direction == SEARCH_FORWARD) {
	gtk_text_buffer_get_iter_at_offset(buffer, &find_start, current_highlighted_end);
 } else if (direction == SEARCH_BACKWARD) {
 	gtk_text_buffer_get_iter_at_offset(buffer, &find_start, current_highlighted_start);
 }
 if (gtk_text_iter_search_word(&find_start, word, GTK_TEXT_SEARCH_TEXT_ONLY| 
								 GTK_TEXT_SEARCH_VISIBLE_ONLY, &match_start,
								 &match_end, NULL, direction)) {
	gtk_text_buffer_apply_tag_by_name(buffer, "green-bg", &match_start, &match_end);
	current_highlighted_start = gtk_text_iter_get_offset(&match_start);
	current_highlighted_end = gtk_text_iter_get_offset(&match_end);
	gtk_text_view_scroll_to_iter(GTK_TEXT_VIEW(sentences_text), &match_start, 0.0, TRUE, 0.5, 0.5);
 } else {
	if (direction == SEARCH_FORWARD) {
		gtk_text_buffer_get_end_iter(buffer, &find_end);
		current_highlighted_start = gtk_text_iter_get_offset(&find_end);
		current_highlighted_end = gtk_text_iter_get_offset(&find_end);
	} else if (direction == SEARCH_BACKWARD) {
		gtk_text_buffer_get_start_iter(buffer, &find_start);
		current_highlighted_start = gtk_text_iter_get_offset(&find_start);
		current_highlighted_end = gtk_text_iter_get_offset(&find_start);	
	}
 }
}

