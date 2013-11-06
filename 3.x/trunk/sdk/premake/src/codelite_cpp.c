/**********************************************************************
 * Premake - codelite_cpp.c
 * The Code::Blocks C/C++ target
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
#include "os.h"

static const char* filterLinks(const char* name);
static const char* filterLinksForPaths(const char* name);
static void list_files(const char* path, int stage);
static void print_opt(const char* opt);
static const char* printProjectDependencies(const char* name);

int codelite_cpp()
{
    const char* kind;
	int i;

	/* Write the file */
	if (!io_openfile(path_join(prj_get_pkgpath(), prj_get_pkgname(), "project")))
		return 0;

	io_print("<?xml version=\"1.0\" encoding=\"utf-8\"?>\n");
	io_print("<CodeLite_Project Name=\"%s\">\n", prj_get_pkgname());

    prj_select_config(0);
	print_source_tree("", list_files);

    if (prj_is_kind("winexe") || prj_is_kind("exe"))
        kind = "Executable";
    else if (prj_is_kind("lib"))
        kind = "Static Library";
    else if (prj_is_kind("dll"))
        kind = "Dynamic Library";
    else
    {
        printf("** Unsupported project kind %s\n", prj_get_kind());
        return 0;
    }

    io_print("  <Settings Type=\"%s\">\n", kind);

	for (i = 0; i < prj_get_numconfigs(); ++i)
	{
		const char* compiler;
		const char* outfile;

		prj_select_config(i);
		
		/* Write the general setup */
		compiler = prj_is_lang("c") ? "gcc" : "g++";
        io_print("    <Configuration Name=\"%s\" CompilerType=\"gnu %s\" DebuggerType=\"GNU gdb debugger\" Type=\"%s\">\n", prj_get_cfgname(), compiler, kind );
		outfile = prj_get_target();
		io_print("      <General OutputFile=\"%s\" IntermediateDirectory=\"%s\" ", outfile, prj_get_objdir());

		if (!os_is("windows"))
			io_print("Command=\"./%s\" CommandArguments=\"\" ", path_getname(outfile));
		else
			io_print("Command=\"%s\" CommandArguments=\"\" ", path_getname(outfile));
			
		io_print("WorkingDirectory=\"%s\" ", prj_get_outdir());
		if (prj_is_kind("winexe"))
			io_print("PauseExecWhenProcTerminates=\"no\"/>\n");
		else
			io_print("PauseExecWhenProcTerminates=\"yes\"/>\n");
		
		/* Write compiler flags */
		io_print("      <Compiler Required=\"yes\" Options=\"");
		if (prj_has_flag("extra-warnings"))
			print_opt("-Wall");
		if (prj_has_flag("fatal-warnings"))
			print_opt("-Werror");
		if (prj_has_flag("no-exceptions"))
			print_opt("--no-exceptions");
		if (prj_has_flag("no-frame-pointer"))
			print_opt("-fomit-frame-pointer");
		if (prj_has_flag("no-rtti"))
			print_opt("--no-rtti");
		if (!prj_has_flag("no-symbols"))
			print_opt("-g");
		if (prj_has_flag("optimize-size"))
			print_opt("-Os");
		if (prj_has_flag("optimize-speed"))
			print_opt("-O3");
		if (prj_has_flag("optimize") && !prj_has_flag("optimize-size") && !prj_has_flag("optimize-speed"))
			print_opt("-O");
		
		/* This is debateable, but it will not hurt on any platform */
		if (prj_is_kind("dll") && !os_is("windows"))
			print_opt("-fPIC");
		
		/* Write pch support. Presently not supported, but is planned. */
		//if (prj_has_pch())
		//{
			/* Warns you if your pch file is out of date */
			//print_opt("-Winvalid-pch");
			/* Force include the pch header so the user doesn't need to */
			//io_print("-include &quot;%s&quot;;", prj_get_pch_header());
		//}
		
		print_list(prj_get_buildoptions(), "", ";", "", NULL);
		
		/* End the compiler open element */
		io_print("\">\n");

		print_list(prj_get_incpaths(), "        <IncludePath Value=\"", "\"/>\n", "", NULL);
		print_list(prj_get_defines(), "        <Preprocessor Value=\"", "\"/>\n", "", NULL);
		io_print("      </Compiler>\n");

		/* Write linker flags */
		io_print("      <Linker Required=\"yes\" Options=\"");
		
		if (prj_is_kind("dll"))
		{		
			/* Create import libraries for DLLs if we're using Cygwin or MinGW.
			 * This seems fragile to me, but I'm going to let it sit and see
			 * if anyone complains about it. */
			if (!prj_has_flag("no-import-lib") && os_is("windows"))
			{
				const char* str;

				io_print("-Wl,--out-implib=&quot;%s", prj_get_bindir());
				if (prj_get_prefix() == NULL)
					str = "lib";
				else
					str = prj_get_prefix();

				if (prj_has_importlibname())
					io_print("/%s%s.a&quot;;", str, prj_get_importlibname());
				else
					io_print("/%s%s.a&quot;;", str, path_getbasename(prj_get_target()));
			}
		}
		
		if (prj_is_kind("winexe"))
			print_opt("-mwindows");

		/* OS X has a bug, see http://lists.apple.com/archives/Darwin-dev/2006/Sep/msg00084.html */
		if (prj_has_flag("no-symbols"))
		{
			if (os_is("macosx"))
				print_opt("-Wl,-x");
			else
				print_opt("-s");
		}

		if (os_is("macosx") && prj_has_flag("dylib"))
		{
			print_opt("-dynamiclib");
			print_opt("-flat_namespace");
		}
		print_list(prj_get_linkoptions(), "", ";", "", NULL);
		io_print("\">\n");
		
		// Make a copy into a different buffer,
		// so that the same buffer is not used for the two function calls.
		strcpy(g_buffer, prj_get_bindir());
		io_print("        <LibraryPath Value=\"%s\"/>\n", g_buffer);
		if (!matches(g_buffer, prj_get_libdir()))
			io_print("        <LibraryPath Value=\"%s\"/>\n", prj_get_libdir());
		print_list(prj_get_libpaths(), "        <LibraryPath Value=\"", "\"/>\n", "", NULL);
		print_list(prj_get_links(), "        <LibraryPath Value=\"", "\"/>\n", "", filterLinksForPaths);
		print_list(prj_get_links(), "        <Library Value=\"", "\"/>\n", "", filterLinks);
		
		/* Close the linker element */
		io_print("      </Linker>\n");
		
        if (prj_find_filetype(".rc"))
        {
			io_print("      <ResourceCompiler Required=\"yes\" Options=\"");
			print_list(prj_get_resdefines(), "-D", ";", "", NULL);
			print_list(prj_get_resoptions(), "", ";", "", NULL);
			io_print("\">\n");
			print_list(prj_get_respaths(), "        <IncludePath Value=\"", "\"/>\n", "", NULL);
			io_print("      </ResourceCompiler>\n");
        }
		else
			io_print("      <ResourceCompiler Required=\"no\" Options=\"\"/>\n");
		
		/* Pre/Post build setings */
		if (prj_get_numprebuildcommands() > 0 || prj_get_numpostbuildcommands() > 0)
		{
			if (prj_get_numprebuildcommands() > 0)
			{
				io_print("      <PreBuild>\n");
				print_list(prj_get_prebuildcommands(), "        <Command Enabled=\"yes\">", "</Command>\n", "", NULL);
				io_print("      </PreBuild>\n");
			}
			else
				io_print("      <PreBuild/>\n");
			
			if (prj_get_numpostbuildcommands() > 0)
			{
				io_print("      <PostBuild>\n");
				print_list(prj_get_postbuildcommands(), "        <Command Enabled=\"yes\">", "</Command>\n", "", NULL);
				io_print("      </PostBuild>\n");
			}
			else
				io_print("      <PostBuild/>\n");
		}
		
		/* Configuration stuff that isn't supported */
		io_print("      <CustomBuild Enabled=\"no\">\n");
		io_print("        <CleanCommand></CleanCommand>\n");
		io_print("        <BuildCommand></BuildCommand>\n");
		io_print("        <SingleFileCommand></SingleFileCommand>\n");
		io_print("        <MakefileGenerationCommand></MakefileGenerationCommand>\n");
		io_print("        <ThirdPartyToolName>None</ThirdPartyToolName>\n");
		io_print("        <WorkingDirectory></WorkingDirectory>\n");
		io_print("      </CustomBuild>\n");
		io_print("      <AdditionalRules>\n");
		io_print("        <CustomPostBuild></CustomPostBuild>\n");
		io_print("        <CustomPreBuild></CustomPreBuild>\n");
		io_print("      </AdditionalRules>\n");
		io_print("    </Configuration>\n");	
	}
	/* End of the project settings */
	io_print("  </Settings>\n");
	
	/* Write project dependencies */
	for (i = 0; i < prj_get_numconfigs(); ++i)
	{
		prj_select_config(i);
		io_print("  <Dependencies name=\"%s\">\n", prj_get_cfgname());
		print_list(prj_get_links(), "    <Project Name=\"", "\"/>\n", "", printProjectDependencies);
		io_print("  </Dependencies>\n");
	}
	
	/* End of the project file */
	io_print("</CodeLite_Project>\n");

	io_closefile();
	return 1;
}

static void print_opt(const char* opt)
{
	io_print("%s;", opt);
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
			io_print("    <Project Name=\"%s\"/>\n", name);
		}
	}

	return NULL;
}

/************************************************************************
 * Callback for print_source_tree()
 ***********************************************************************/

static void list_files(const char* path, int stage)
{
	static int indent = 1;
	//const char* pchSource = prj_has_pch() ? prj_get_pch_source() : NULL;

	const char* ptr = path;
	while (strncmp(ptr, "../", 3) == 0)
		ptr += 3;

	ptr = strchr(ptr, '/');
	while (ptr != NULL)
		ptr = strchr(ptr + 1, '/');

	ptr = strrchr(path, '/');
	ptr = (ptr == NULL) ? (char*)path : ptr + 1;

	switch (stage)
	{
	case WST_OPENGROUP:
		if (strlen(path) > 0 && !matches(ptr, ".."))
		{
			io_print("%*s<VirtualDirectory Name=\"%s\">\n", indent * 2, " ", ptr);
			indent++;
		}
		break;

	case WST_CLOSEGROUP:
		if (strlen(path) > 0 && !matches(ptr, ".."))
		{
			indent--;
			io_print("%*s</VirtualDirectory>\n", indent * 2, " " );
		}
		break;

	case WST_SOURCEFILE:
		io_print("%*s<File Name=\"%s\"/>\n",indent * 2, " ", path);
		break;
	}
}

/************************************************************************
 * Checks each entry in the list of package links. If the entry refers
 * to a sibling package, returns the path to that package's output
 ***********************************************************************/

static const char* filterLinks(const char* name)
{
	int i = prj_find_package(name);
	if (i >= 0)
	{
		const char* lang = prj_get_language_for(i);
		if (matches(lang, "c++") || matches(lang, "c"))
		{
			const char* target = prj_get_targetname_for(i);
			return path_getname(target);
		}
		else
		{
			return NULL;
		}
	}
	else
	{
		return name;
	}
}

static const char* filterLinksForPaths(const char* name)
{
	int i = prj_find_package(name);
	if (i >= 0)
	{
		const char* lang = prj_get_language_for(i);
		if (matches(lang, "c++") || matches(lang, "c"))
		{
			const char* target = prj_get_target_for(i);
			const char* dir = path_getdir(target);
			if (!prj_has_libpath(dir))
			{
				return dir;
			}
		}
	}
	return NULL;
}
