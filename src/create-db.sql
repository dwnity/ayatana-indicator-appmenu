create table usage (application text, entry text, timestamp datetime);
create index if not exists application_index on usage (application);
