#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "window-menu.h"

#define WINDOW_MENU_GET_PRIVATE(o) \
(G_TYPE_INSTANCE_GET_PRIVATE ((o), WINDOW_MENU_TYPE, WindowMenuPrivate))

static void window_menu_class_init (WindowMenuClass *klass);
static void window_menu_init       (WindowMenu *self);
static void window_menu_dispose    (GObject *object);
static void window_menu_finalize   (GObject *object);

G_DEFINE_TYPE (WindowMenu, window_menu, G_TYPE_OBJECT);

static void
window_menu_class_init (WindowMenuClass *klass)
{
	GObjectClass *object_class = G_OBJECT_CLASS (klass);

	object_class->dispose = window_menu_dispose;
	object_class->finalize = window_menu_finalize;

	return;
}

static void
window_menu_init (WindowMenu *self)
{

	return;
}

static void
window_menu_dispose (GObject *object)
{

	G_OBJECT_CLASS (window_menu_parent_class)->dispose (object);
	return;
}

static void
window_menu_finalize (GObject *object)
{

	G_OBJECT_CLASS (window_menu_parent_class)->finalize (object);
	return;
}


/**************************
  API
 **************************/
GList *
window_menu_get_entries (WindowMenu * wm)
{
	WindowMenuClass * class = WINDOW_MENU_GET_CLASS(wm);

	if (class->get_entries != NULL) {
		return class->get_entries(wm);
	} else {
		return NULL;
	}
}

guint
window_menu_get_location (WindowMenu * wm, IndicatorObjectEntry * entry)
{
	WindowMenuClass * class = WINDOW_MENU_GET_CLASS(wm);

	if (class->get_location != NULL) {
		return class->get_location(wm, entry);
	} else {
		return 0;
	}
}

guint
window_menu_get_xid (WindowMenu * wm)
{
	WindowMenuClass * class = WINDOW_MENU_GET_CLASS(wm);

	if (class->get_xid != NULL) {
		return class->get_xid(wm);
	} else {
		return 0;
	}
}

gchar *
window_menu_get_path (WindowMenu * wm)
{
	WindowMenuClass * class = WINDOW_MENU_GET_CLASS(wm);

	if (class->get_path != NULL) {
		return class->get_path(wm);
	} else {
		return NULL;
	}
}

gchar *
window_menu_get_address (WindowMenu * wm)
{
	WindowMenuClass * class = WINDOW_MENU_GET_CLASS(wm);

	if (class->get_address != NULL) {
		return class->get_address(wm);
	} else {
		return NULL;
	}
}

gboolean
window_menu_get_error_state (WindowMenu * wm)
{
	WindowMenuClass * class = WINDOW_MENU_GET_CLASS(wm);

	if (class->get_error_state != NULL) {
		return class->get_error_state(wm);
	} else {
		return TRUE;
	}
}

WindowMenuStatus
window_menu_get_status (WindowMenu * wm)
{
	WindowMenuClass * class = WINDOW_MENU_GET_CLASS(wm);

	if (class->get_status != NULL) {
		return class->get_status(wm);
	} else {
		return WINDOW_MENU_STATUS_NORMAL;
	}
}

void
window_menu_entry_restore (WindowMenu * wm, IndicatorObjectEntry * entry)
{
	WindowMenuClass * class = WINDOW_MENU_GET_CLASS(wm);

	if (class->entry_restore != NULL) {
		return class->entry_restore(wm, entry);
	} else {
		return;
	}
}

void
window_menu_entry_activate (WindowMenu * wm, IndicatorObjectEntry * entry, guint timestamp)
{
	WindowMenuClass * class = WINDOW_MENU_GET_CLASS(wm);

	if (class->entry_activate != NULL) {
		return class->entry_activate(wm, entry, timestamp);
	} else {
		return;
	}
}
