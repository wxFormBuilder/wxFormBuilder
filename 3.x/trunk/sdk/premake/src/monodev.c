/**********************************************************************
 * Premake - monodev.c
 * The MonoDevelop target
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
#include "monodev.h"

int monodev_target;
int monodev_warncontent;

static int writeCombine();

int monodev_generate()
{
	int i;

	monodev_warncontent = 0;

	puts("Generating MonoDevelop solution and project files:");

	if (!writeCombine())
		return 0;

	for (i = 0; i < prj_get_numpackages(); ++i)
	{
		prj_select_package(i);

		printf("...%s\n", prj_get_pkgname());
		if (prj_is_lang("c#"))
		{
			if (!monodev_cs())
				return 0;
		}
		else if (prj_is_lang("c++") || prj_is_lang("c"))
		{
			printf("** Error: this generator does not support C/C++ development.\n");
			return 0;
		}
		else
		{
			printf("** Error: unrecognized language '%s'\n", prj_get_language());
			return 0;
		}
	}

	if (monodev_warncontent)
	{
		puts("\n** Warning: this project uses the 'Content' build action. This action is not");
		puts("            supported by Monodevelop; some manual configuration may be needed.");
	}

	return 1;
}


static int writeCombine()
{
	const char* path;
	int i, j;

	if (!io_openfile(path_join(prj_get_path(), prj_get_name(), "mds")))
		return 0;

	io_print("<Combine name=\"%s\" fileversion=\"2.0\">\n", prj_get_name());

	/* Write out the entries for each build configuration */
	prj_select_package(0);
	prj_select_config(0);
	io_print("  <Configurations active=\"%s\">\n", xmlEscape(prj_get_cfgname()));
	for (i = 0; i < prj_get_numconfigs(); ++i)
	{
		prj_select_config(i);
		io_print("    <Configuration name=\"%s\" ctype=\"CombineConfiguration\">\n", xmlEscape(prj_get_cfgname()));

		/* List all packages under this configuration */
		prj_select_config(0);
		for(j = 0; j < prj_get_numpackages(); j++)
		{
			prj_select_package(j);
			io_print("      <Entry build=\"True\" name=\"%s\" configuration=\"%s\" />\n", prj_get_pkgname(), xmlEscape(prj_get_cfgname()));
		}

		io_print("    </Configuration>\n");
	}
	io_print("  </Configurations>\n");

	/* TODO: select the first executable project */
	prj_select_package(0);
	io_print("  <StartMode startupentry=\"%s\" single=\"True\">\n", prj_get_pkgname());

	/* Write out the startup entries */
	for (i = 0; i < prj_get_numpackages(); ++i)
	{
		prj_select_package(i);
		io_print("    <Execute type=\"None\" entry=\"%s\" />\n", prj_get_pkgname());
	}

	io_print("  </StartMode>\n");
	io_print("  <Entries>\n");

	/* Write out the project entries */
	for (i = 0; i < prj_get_numpackages(); ++i)
	{
		prj_select_package(i);
		path = prj_get_pkgfilename("mdp");
		io_print("    <Entry filename=\"");
		if (path[0] != '.')
			io_print("./");
		io_print("%s\" />\n", path);
	}

	io_print("  </Entries>\n");

	/* Finish */
	io_print("</Combine>");
	io_closefile();
	return 1;
}
