/**********************************************************************
 * Premake - codelite.c
 * The Code::Blocks target
 *
 * Copyright (c) 2002-2008 Jason Perkins and the Premake project
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License in the file LICENSE.txt for details.
 **********************************************************************/

#include <stdio.h>
#include <string.h>
#include "premake.h"
#include "codelite.h"

static int writeWorkspace();

int codelite_generate()
{
	int i;

	puts("Generating CodeLite workspace and project files:");

	if (!writeWorkspace())
		return 0;

	for (i = 0; i < prj_get_numpackages(); ++i)
	{
		prj_select_package(i);
		printf("...%s\n", prj_get_pkgname());

		if (prj_is_lang("c++") || prj_is_lang("c"))
		{
			if (!codelite_cpp())
				return 0;
		}
		else
		{
			printf("** Error: unsupported language '%s'\n", prj_get_language());
			return 0;
		}
	}

	return 1;
}

static int writeWorkspace()
{
	int i, j;

	if (!io_openfile(path_join(prj_get_path(), prj_get_name(), "workspace")))
		return 0;

	io_print("<?xml version=\"1.0\" encoding=\"utf-8\"?>\n");
	io_print("<CodeLite_Workspace Name=\"%s\" Database=\"./%s.tags\">\n", prj_get_name(), prj_get_name());

	for (i = 0; i < prj_get_numpackages(); ++i)
	{
		const char* filename;

		prj_select_package(i);

		filename = prj_get_pkgfilename("project");
		if (startsWith(filename, "./")) filename += 2;
		io_print("  <Project Name=\"%s\" Path=\"%s\" Active=\"%s\" />\n", prj_get_pkgname(), filename, (0 == i) ? "Yes" : "No" );
	}

	io_print("  <BuildMatrix>\n");
	for (i = 0; i < prj_get_numconfigs(); ++i)
	{
		prj_select_config(i);
		io_print("    <WorkspaceConfiguration Name=\"%s\" Selected=\"yes\">\n", prj_get_cfgname());
		for (j = 0; j < prj_get_numpackages(); ++j)
		{
			prj_select_package(j);
			io_print("      <Project Name=\"%s\" ConfigName=\"%s\"/>\n", prj_get_pkgname(), prj_get_cfgname() );
		}
		io_print("    </WorkspaceConfiguration>\n");
	}
	io_print("  </BuildMatrix>\n");
	io_print("</CodeLite_Workspace>\n");
	io_closefile();
	return 1;
}
