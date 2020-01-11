/**********************************************************************
 * Premake - project.h
 * An interface around the project data.
 *
 * Copyright (c) 2002-2005 Jason Perkins and the Premake project
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

typedef struct tagOption
{
	const char* flag;
	const char* desc;
} Option;


typedef struct tagPrjConfig
{
	const char* name;
	const char* bindir;
	const char* libdir;
} PrjConfig;

typedef struct tagFileConfig
{
	const char* buildaction;
} FileConfig;

typedef struct tagPkgConfig
{
	PrjConfig*   prjConfig;
	const char** buildopts;
	const char** prebuildcmds;
	const char** prelinkcmds;
	const char** postbuildcmds;
	const char** defines;
	const char** resdefines;
	const char*  extension;
	const char** files;
	const char** flags;
	const char** incpaths;
	const char** respaths;
	const char** libpaths;
	const char** linkopts;
	const char** resopts;
	const char** links;
	const char*  bindir;
	const char*  libdir;
	const char*  objdir;
	const char*  prefix;
	const char*  target;
	const char*  kind;
	const char*  pchHeader;
	const char*  pchSource;
	const char*  implibname;
	FileConfig** fileconfigs;
} PkgConfig;

typedef struct tagPackage
{
	int index;
	const char*  name;
	const char*  path;
	const char*  script;
	const char*  lang;
	const char*  objdir;
	const char*  url;
	const char*  guid;
	PkgConfig**  configs;
	void*        data;
} Package;

typedef struct tagProject
{
	const char* name;
	const char* path;
	const char* script;
	Option**    options;
	PrjConfig** configs;
	Package**   packages;
} Project;

extern Project* project;


void         prj_open();
void         prj_close();

const char*  prj_find_filetype(const char* extension);
int          prj_find_package(const char* name);
const char*  prj_get_bindir();
const char*  prj_get_buildaction();
const char** prj_get_buildoptions();
const char** prj_get_prebuildcommands();
const char** prj_get_prelinkcommands();
const char** prj_get_postbuildcommands();
const char*  prj_get_cfgname();
PkgConfig*   prj_get_config_for(int i);
void*        prj_get_data();
void*        prj_get_data_for(int i);
const char** prj_get_defines();
const char*  prj_get_extension();
const char** prj_get_files();
const char*  prj_get_kind();
const char*  prj_get_kind_for(int i);
const char** prj_get_incpaths();
const char*  prj_get_language();
const char*  prj_get_language_for(int i);
const char*  prj_get_libdir();
const char*  prj_get_libdir_for(int i);
const char** prj_get_libpaths();
const char** prj_get_linkoptions();
const char** prj_get_links();
const char*  prj_get_name();
int          prj_get_numbuildoptions();
int          prj_get_numprebuildcommands();
int          prj_get_numprelinkcommands();
int          prj_get_numpostbuildcommands();
int          prj_get_numconfigs();
int          prj_get_numdefines();
int          prj_get_numincpaths();
int          prj_get_numlinkoptions();
int          prj_get_numlinks();
int          prj_get_numoptions();
int          prj_get_numpackages();
int          prj_get_numresdefines();
int          prj_get_numresoptions();
int          prj_get_numrespaths();
const char*  prj_get_objdir();
const char*  prj_get_optdesc();
const char*  prj_get_optname();
const char*  prj_get_outdir();
const char*  prj_get_outdir_for(int i);
Package*     prj_get_package();
Package*     prj_get_package_for(int i);
const char*  prj_get_path();
const char*  prj_get_pch_header();
const char*  prj_get_pch_source();
const char*  prj_get_pkgfilename(const char* extension);
const char*  prj_get_pkgfilename_for(int i, const char* extension);
const char*  prj_get_pkgname();
const char*  prj_get_pkgname_for(int i);
const char*  prj_get_pkgobjdir();
const char*  prj_get_pkgpath();
const char*  prj_get_pkgpath_for(int i);
const char*  prj_get_pkgscript();
const char*  prj_get_prefix();
const char** prj_get_resdefines();
const char** prj_get_resoptions();
const char** prj_get_respaths();
const char*  prj_get_script();
const char*  prj_get_target();
const char*  prj_get_target_for(int i);
const char*  prj_get_relativetarget_for(int i);
const char*  prj_get_target_raw();
const char*  prj_get_targetname_for(int i);
const char*  prj_get_url();
int          prj_has_file(const char* name);
int          prj_has_flag(const char* flag);
int          prj_has_flag_for(int i, const char* flag);
int          prj_has_libpath(const char* path);
int          prj_has_pch();
int          prj_is_buildaction(const char* action);
int          prj_is_kind(const char* kind);
int          prj_is_lang(const char* lang);
void         prj_select_config(int i);
void         prj_select_file(const char* name);
void         prj_select_option(int i);
void         prj_select_package(int i);
void         prj_set_buildaction(const char* action);
void         prj_set_data(void* data);

void**       prj_newlist(int len);
void         prj_freelist(void** list);
int          prj_getlistsize(void** list);

int          prj_has_guid();
const char*  prj_get_guid();

int          prj_has_importlibname();
const char*  prj_get_importlibname();
