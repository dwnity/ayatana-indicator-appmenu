#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <glib.h>
#include <glib/gstdio.h>
#include <sqlite3.h>
#include "usage-tracker.h"

struct _UsageTrackerPrivate {
	gchar * cachefile;
	sqlite3 * db;
	guint drop_timer;
};

#define USAGE_TRACKER_GET_PRIVATE(o) \
(G_TYPE_INSTANCE_GET_PRIVATE ((o), USAGE_TRACKER_TYPE, UsageTrackerPrivate))

static void usage_tracker_class_init (UsageTrackerClass *klass);
static void usage_tracker_init       (UsageTracker *self);
static void usage_tracker_dispose    (GObject *object);
static void usage_tracker_finalize   (GObject *object);
static void build_db                 (UsageTracker * self);
static gboolean drop_entries         (gpointer user_data);

G_DEFINE_TYPE (UsageTracker, usage_tracker, G_TYPE_OBJECT);

static void
usage_tracker_class_init (UsageTrackerClass *klass)
{
	GObjectClass *object_class = G_OBJECT_CLASS (klass);

	g_type_class_add_private (klass, sizeof (UsageTrackerPrivate));

	object_class->dispose = usage_tracker_dispose;
	object_class->finalize = usage_tracker_finalize;

	return;
}

static void
usage_tracker_init (UsageTracker *self)
{
	self->priv = USAGE_TRACKER_GET_PRIVATE(self);

	self->priv->cachefile = NULL;
	self->priv->db = NULL;
	self->priv->drop_timer = 0;

	const gchar * basecachedir = g_getenv("HUD_CACHE_DIR");
	if (basecachedir == NULL) {
		basecachedir = g_get_user_cache_dir();
	}

	gchar * cachedir = g_build_filename(basecachedir, "hud", NULL);
	if (!g_file_test(cachedir, G_FILE_TEST_EXISTS | G_FILE_TEST_IS_DIR)) {
		g_mkdir(cachedir, 1 << 6 | 1 << 7 | 1 << 8); // 700
	}
	g_free(cachedir);

	self->priv->cachefile = g_build_filename(basecachedir, "hud", "usage-log.sqlite", NULL);
	gboolean db_exists = g_file_test(self->priv->cachefile, G_FILE_TEST_EXISTS);
	int open_status = sqlite3_open(self->priv->cachefile, &self->priv->db); 

	if (open_status != SQLITE_OK) {
		g_warning("Error building LRU DB");
		sqlite3_close(self->priv->db);
		self->priv->db = NULL;
	}

	if (self->priv->db != NULL && !db_exists) {
		build_db(self);
	}

	drop_entries(self);
	/* Drop entries daily if we run for a really long time */
	self->priv->drop_timer = g_timeout_add_seconds(24 * 60 * 60, drop_entries, self);
	
	return;
}

static void
usage_tracker_dispose (GObject *object)
{
	UsageTracker * self = USAGE_TRACKER(object);

	if (self->priv->db != NULL) {
		sqlite3_close(self->priv->db);
		self->priv->db = NULL;
	}

	if (self->priv->drop_timer != 0) {
		g_source_remove(self->priv->drop_timer);
		self->priv->drop_timer = 0;
	}

	G_OBJECT_CLASS (usage_tracker_parent_class)->dispose (object);
	return;
}

static void
usage_tracker_finalize (GObject *object)
{
	UsageTracker * self = USAGE_TRACKER(object);

	if (self->priv->cachefile != NULL) {
		g_free(self->priv->cachefile);
		self->priv->cachefile = NULL;
	}

	G_OBJECT_CLASS (usage_tracker_parent_class)->finalize (object);
	return;
}

UsageTracker *
usage_tracker_new (void)
{
	return g_object_new(USAGE_TRACKER_TYPE, NULL);
}

static void
build_db (UsageTracker * self)
{
	g_debug("New database, initializing");

	/* Create the table */
	int exec_status = SQLITE_OK;
	gchar * failstring = NULL;
	exec_status = sqlite3_exec(self->priv->db,
	                           "create table usage (application text, entry text, timestamp datetime);",
	                           NULL, NULL, &failstring);
	if (exec_status != SQLITE_OK) {
		g_warning("Unable to create table: %s", failstring);
	}

	/* Import data from the system */

	return;
}

void
usage_tracker_mark_usage (UsageTracker * self, const gchar * application, const gchar * entry)
{
	g_return_if_fail(IS_USAGE_TRACKER(self));

	gchar * statement = g_strdup_printf("insert into usage (application, entry, timestamp) values ('%s', '%s', date('now'));", application, entry);
	g_debug("Executing: %s", statement);

	int exec_status = SQLITE_OK;
	gchar * failstring = NULL;
	exec_status = sqlite3_exec(self->priv->db,
	                           statement,
	                           NULL, NULL, &failstring);
	if (exec_status != SQLITE_OK) {
		g_warning("Unable to insert into table: %s", failstring);
	}

	g_free(statement);
	return;
}

static int
count_cb (void * user_data, int columns, char ** values, char ** names)
{
	g_return_val_if_fail(columns == 1, -1);

	guint * count = (guint *)user_data;

	*count = g_ascii_strtoull(values[0], NULL, 10);

	return SQLITE_OK;
}

guint
usage_tracker_get_usage (UsageTracker * self, const gchar * application, const gchar * entry)
{
	g_return_val_if_fail(IS_USAGE_TRACKER(self), 0);

	// TODO: Check if application has entries, if not, import defaults

	gchar * statement = g_strdup_printf("select count(*) from usage where application = '%s' and entry = '%s' and timestamp > date('now', 'utc', '-30 days');", application, entry); // TODO: Add timestamp
	g_debug("Executing: %s", statement);

	int exec_status = SQLITE_OK;
	gchar * failstring = NULL;
	guint count;
	exec_status = sqlite3_exec(self->priv->db,
	                           statement,
	                           count_cb, &count, &failstring);
	if (exec_status != SQLITE_OK) {
		g_warning("Unable to insert into table: %s", failstring);
	}

	g_free(statement);
	return count;
}

static gboolean
drop_entries (gpointer user_data)
{
	g_return_val_if_fail(IS_USAGE_TRACKER(user_data), FALSE);
	UsageTracker * self = USAGE_TRACKER(user_data);

	const gchar * statement = "delete from usage where timestamp < date('now', 'utc', '-30 days');";
	g_debug("Executing: %s", statement);

	int exec_status = SQLITE_OK;
	gchar * failstring = NULL;
	exec_status = sqlite3_exec(self->priv->db,
	                           statement,
	                           NULL, NULL, &failstring);
	if (exec_status != SQLITE_OK) {
		g_warning("Unable to drop entries from table: %s", failstring);
	}

	return TRUE;
}
