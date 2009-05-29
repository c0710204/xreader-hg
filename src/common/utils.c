/*
 * This file is part of xReader.
 *
 * Copyright (C) 2008 hrimfaxi (outmatch@gmail.com)
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
 * or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License
 * for more details.
 *
 * You should have received a copy of the GNU General Public License along
 * with this program; if not, write to the Free Software Foundation, Inc.,
 * 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA
 */

#include <stdio.h>
#include <string.h>
#include <pspkernel.h>
#include <stdarg.h>
#include "config.h"
#include "utils.h"
#include "strsafe.h"
#include "dbg.h"
#include "xrhal.h"
#ifdef DMALLOC
#include "dmalloc.h"
#endif

extern dword utils_dword2string(dword dw, char *dest, dword width)
{
	dest[width] = 0;
	if (dw == 0) {
		dest[--width] = '0';
		return width;
	}
	while (dw > 0 && width > 0) {
		dest[--width] = '0' + (dw % 10);
		dw /= 10;
	}
	return width;
}

extern bool utils_string2dword(const char *src, dword * dw)
{
	*dw = 0;
	while (*src >= '0' && *src <= '9') {
		*dw = *dw * 10 + (*src - '0');
		src++;
	}
	return (*src == 0);
}

extern bool utils_string2double(const char *src, double *db)
{
	*db = 0.0;
	bool doted = false;
	double p = 0.1;

	while ((*src >= '0' && *src <= '9') || *src == '.') {
		if (*src == '.') {
			if (doted)
				return false;
			else
				doted = true;
		} else {
			if (doted) {
				*db = *db + p * (*src - '0');
				p = p / 10;
			} else
				*db = *db * 10 + (*src - '0');
		}
		src++;
	}
	return (*src == 0);
}

extern const char *utils_fileext(const char *filename)
{
	dword len = strlen(filename);
	const char *p = filename + len;

	while (p > filename && *p != '.' && *p != '/')
		p--;
	if (*p == '.')
		return p + 1;
	else
		return NULL;
}

extern bool utils_del_file(const char *file)
{
	int result;
	SceIoStat stat;

	memset(&stat, 0, sizeof(SceIoStat));
	result = xrIoGetstat(file, &stat);
	if (result < 0)
		return false;
	stat.st_attr &= ~0x0F;
	result = xrIoChstat(file, &stat, 3);
	if (result < 0)
		return false;
	result = xrIoRemove(file);
	if (result < 0)
		return false;

	return true;
}

extern dword utils_del_dir(const char *dir)
{
	dword count = 0;
	int dl = xrIoDopen(dir);

	if (dl < 0)
		return count;
	SceIoDirent sid;

	memset(&sid, 0, sizeof(SceIoDirent));
	while (xrIoDread(dl, &sid)) {
		if (sid.d_name[0] == '.')
			continue;
		char compPath[260];

		SPRINTF_S(compPath, "%s/%s", dir, sid.d_name);
		if (FIO_S_ISDIR(sid.d_stat.st_mode)) {
			if (utils_del_dir(compPath)) {
				count++;
			}
			continue;
		}
		if (utils_del_file(compPath)) {
			count++;
		}
		memset(&sid, 0, sizeof(SceIoDirent));
	}
	xrIoDclose(dl);
	xrIoRmdir(dir);

	return count;
}

bool utils_is_file_exists(const char *filename)
{
	if (!filename)
		return false;

	SceUID uid;

	uid = xrIoOpen(filename, PSP_O_RDONLY, 0777);
	if (uid < 0)
		return false;
	xrIoClose(uid);
	return true;
}

void *safe_realloc(void *ptr, size_t size)
{
	void *p = realloc(ptr, size);

	if (size == 0) {
		return NULL;
	}

	if (p == NULL) {
		if (ptr)
			free(ptr);
		ptr = NULL;
	}

	return p;
}

