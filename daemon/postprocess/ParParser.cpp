/*
 *  This file is part of nzbget
 *
 *  Copyright (C) 2007-2015 Andrey Prygunkov <hugbug@users.sourceforge.net>
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
 *
 * $Revision$
 * $Date$
 *
 */


#include "nzbget.h"
#include "FileSystem.h"
#include "ParParser.h"

bool ParParser::FindMainPars(const char* path, ParFileList* fileList)
{
	if (fileList)
	{
		fileList->clear();
	}

	DirBrowser dir(path);
	while (const char* filename = dir.Next())
	{
		int baseLen = 0;
		if (ParseParFilename(filename, &baseLen, nullptr))
		{
			if (!fileList)
			{
				return true;
			}

			// check if the base file already added to list
			bool exists = false;
			for (CString& filename2 : *fileList)
			{
				exists = SameParCollection(filename, filename2);
				if (exists)
				{
					break;
				}
			}
			if (!exists)
			{
				fileList->emplace_back(filename);
			}
		}
	}
	return fileList && !fileList->empty();
}

bool ParParser::SameParCollection(const char* filename1, const char* filename2)
{
	int baseLen1 = 0, baseLen2 = 0;
	return ParseParFilename(filename1, &baseLen1, nullptr) &&
		ParseParFilename(filename2, &baseLen2, nullptr) &&
		baseLen1 == baseLen2 &&
		!strncasecmp(filename1, filename2, baseLen1);
}

bool ParParser::ParseParFilename(const char* parFilename, int* baseNameLen, int* blocks)
{
	BString<1024> filename = parFilename;
	for (char* p = filename; *p; p++) *p = tolower(*p); // convert string to lowercase

	int len = strlen(filename);
	if (len < 6)
	{
		return false;
	}

	// find last occurence of ".par2" and trim filename after it
	char* end = filename;
	while (char* p = strstr(end, ".par2")) end = p + 5;
	*end = '\0';

	len = strlen(filename);
	if (len < 6)
	{
		return false;
	}

	if (strcasecmp(filename + len - 5, ".par2"))
	{
		return false;
	}
	*(filename + len - 5) = '\0';

	int blockcnt = 0;
	char* p = strrchr(filename, '.');
	if (p && !strncasecmp(p, ".vol", 4))
	{
		char* b = strchr(p, '+');
		if (!b)
		{
			b = strchr(p, '-');
		}
		if (b)
		{
			blockcnt = atoi(b+1);
			*p = '\0';
		}
	}

	if (baseNameLen)
	{
		*baseNameLen = strlen(filename);
	}
	if (blocks)
	{
		*blocks = blockcnt;
	}

	return true;
}
