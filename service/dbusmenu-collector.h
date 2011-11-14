#ifndef __DBUSMENU_COLLECTOR_H__
#define __DBUSMENU_COLLECTOR_H__

#include <glib.h>
#include <glib-object.h>

G_BEGIN_DECLS

#define DBUSMENU_COLLECTOR_TYPE            (dbusmenu_collector_get_type ())
#define DBUSMENU_COLLECTOR(obj)            (G_TYPE_CHECK_INSTANCE_CAST ((obj), DBUSMENU_COLLECTOR_TYPE, DbusmenuCollector))
#define DBUSMENU_COLLECTOR_CLASS(klass)    (G_TYPE_CHECK_CLASS_CAST ((klass), DBUSMENU_COLLECTOR_TYPE, DbusmenuCollectorClass))
#define IS_DBUSMENU_COLLECTOR(obj)         (G_TYPE_CHECK_INSTANCE_TYPE ((obj), DBUSMENU_COLLECTOR_TYPE))
#define IS_DBUSMENU_COLLECTOR_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE ((klass), DBUSMENU_COLLECTOR_TYPE))
#define DBUSMENU_COLLECTOR_GET_CLASS(obj)  (G_TYPE_INSTANCE_GET_CLASS ((obj), DBUSMENU_COLLECTOR_TYPE, DbusmenuCollectorClass))

typedef struct _DbusmenuCollector          DbusmenuCollector;
typedef struct _DbusmenuCollectorClass     DbusmenuCollectorClass;
typedef struct _DbusmenuCollectorPrivate   DbusmenuCollectorPrivate;
typedef struct _DbusmenuCollectorFound     DbusmenuCollectorFound;

struct _DbusmenuCollectorClass {
	GObjectClass parent_class;
};

struct _DbusmenuCollector {
	GObject parent;

	DbusmenuCollectorPrivate * priv;
};

GType dbusmenu_collector_get_type (void);
DbusmenuCollector * dbusmenu_collector_new (void);
GList * dbusmenu_collector_search (DbusmenuCollector * collector, const gchar * dbus_addr, const gchar * dbus_path, const gchar * search);

guint dbusmenu_collector_found_get_distance (DbusmenuCollectorFound * found);
const gchar * dbusmenu_collector_found_get_display (DbusmenuCollectorFound * found);
void dbusmenu_collector_found_exec (DbusmenuCollectorFound * found);
void dbusmenu_collector_found_free (DbusmenuCollectorFound * found);
void dbusmenu_collector_found_list_free (GList * found_list);
const gchar *  dbusmenu_collector_found_get_indicator (DbusmenuCollectorFound * found);

G_END_DECLS

#endif
