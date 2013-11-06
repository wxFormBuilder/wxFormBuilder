/**********************************************************************
 * Premake - cb.c
 * The Code::Blocks target
 *
 * Copyright (c) 2002-2006 Jason Perkins and the Premake project
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
#include "cb.h"

static int writeWorkspace();

int cb_generate()
{
	int i;

	puts("Generating Code::Blocks workspace and project files:");

	if (!writeWorkspace())
		return 0;

	for (i = 0; i < prj_get_numpackages(); ++i)
	{
		prj_select_package(i);
		printf("...%s\n", prj_get_pkgname());

		if (prj_is_lang("c++") || prj_is_lang("c"))
		{
			if (!cb_cpp())
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


/************************************************************************
 * List callback: scans the list of links for a package. If a link is
 * found to a sibling package, prints a dependency string for the
 * workspace file.
 ***********************************************************************/
static const char* printProjectDependencies(const char* name)
{
	int i;
	for (i = 0; i < prj_get_numpackages(); ++i)
	{
		if (matches(prj_get_pkgname_for(i), name))
		{
			const char* filename = prj_get_pkgfilename_for(i, "cbp");
			if (startsWith(filename, "./")) filename += 2;
			io_print("\t\t\t<Depends filename=\"%s\" />\n", filename);
		}
	}

	return NULL;
}


static int writeWorkspace()
{
	int i;

	if (!io_openfile(path_join(prj_get_path(), prj_get_name(), "workspace")))
		return 0;

	io_print("<?xml version=\"1.0\" encoding=\"UTF-8\" standalone=\"yes\" ?>\n");
	io_print("<CodeBlocks_workspace_file>\n");
	io_print("\t<Workspace title=\"%s\">\n", prj_get_name());

	for (i = 0; i < prj_get_numpackages(); ++i)
	{
		const char* filename;

		prj_select_package(i);

		filename = prj_get_pkgfilename("cbp");
		if (startsWith(filename, "./")) filename += 2;
		io_print("\t\t<Project filename=\"%s\"", filename);
		if (i == 0) io_print(" active=\"1\"");
		io_print(">\n");

		/* Write project dependencies */
		prj_select_config(0);
		print_list(prj_get_links(), "", "", "", printProjectDependencies);

		io_print("\t\t</Project>\n");
	}

	io_print("\t</Workspace>\n");
	io_print("</CodeBlocks_workspace_file>\n");
	io_closefile();
	return 1;
}
