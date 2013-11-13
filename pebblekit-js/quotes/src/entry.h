#pragma once

typedef void (*EntryCallback)(char *name);

void entry_init(char *name);

void entry_deinit(void);

void entry_get_name(char *name, EntryCallback cb);
