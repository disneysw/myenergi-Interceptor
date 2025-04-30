#ifndef _FILE_H
#define _FILE_H

#include "include.h"
#include "FS.h"

String readFile(fs::FS &fs, const char *path);
void writeFile(fs::FS &fs, const char *path, const char *message);
bool InitFilesystem();

#endif //_FILE_H