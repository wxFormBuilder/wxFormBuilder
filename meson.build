project('wxformbuilder', 'cpp', default_options : ['cpp_std=c++17'])

cxx = meson.get_compiler('cpp')
add_project_arguments(cxx.get_supported_arguments([
	'-Wall',
	'-Wextra',
	'-Winit-self',
	'-Wimplicit-fallthrough',
	'-Wpedantic',
	'-Wdelete-non-virtual-dtor',
	'-Wlogical-op',
	'-Wduplicated-cond',
	'-Wduplicated-branches',
	'-Wno-overloaded-virtual',
	'-Wtautological-compare',
	'-Wno-inconsistent-missing-override',
	'-Wno-ignored-qualifiers',
	'-Wno-unused-parameter',
	'-Wno-cast-function-type',
]), language : 'cpp')

if get_option('buildtype') == 'debug'
	add_project_arguments('-D__WXFB_DEBUG__', language : 'cpp')
endif
if get_option('buildtype') == 'release'
	add_project_arguments('-DNDEBUG', language : 'cpp')
endif

wx_dep = dependency('wxwidgets', version : '>=3.0.3',
                    modules : ['std', 'stc', 'propgrid', 'aui', 'ribbon', 'richtext'])

inc = include_directories(['sdk/plugin_interface'])
ticpp_dep = subproject('ticpp').get_variable('ticpp_dep')

plugin_interface = static_library('plugin_interface', [
		'sdk/plugin_interface/xrcconv.cpp',
		'sdk/plugin_interface/forms/wizard.cpp',
	], dependencies : [ticpp_dep, wx_dep])

executable('wxformbuilder', [
		'src/codegen/codegen.cpp',
		'src/codegen/codeparser.cpp',
		'src/codegen/codewriter.cpp',
		'src/codegen/cppcg.cpp',
		'src/codegen/luacg.cpp',
		'src/codegen/phpcg.cpp',
		'src/codegen/pythoncg.cpp',
		'src/codegen/xrccg.cpp',
		'src/maingui.cpp',
		'src/md5/md5.cc',
		'src/model/database.cpp',
		'src/model/objectbase.cpp',
		'src/model/types.cpp',
		'src/model/xrcfilter.cpp',
		'src/rad/about.cpp',
		'src/rad/appdata.cpp',
		'src/rad/bitmaps.cpp',
		'src/rad/cmdproc.cpp',
		'src/rad/codeeditor/codeeditor.cpp',
		'src/rad/cpppanel/cpppanel.cpp',
		'src/rad/dataobject/dataobject.cpp',
		'src/rad/designer/innerframe.cpp',
		'src/rad/designer/menubar.cpp',
		'src/rad/designer/visualeditor.cpp',
		'src/rad/designer/visualobj.cpp',
		'src/rad/geninheritclass/geninhertclass_gui.cpp',
		'src/rad/geninheritclass/geninhertclass.cpp',
		'src/rad/inspector/objinspect.cpp',
		'src/rad/inspector/wxfbadvprops.cpp',
		'src/rad/luapanel/luapanel.cpp',
		'src/rad/mainframe.cpp',
		'src/rad/menueditor.cpp',
		'src/rad/objecttree/objecttree.cpp',
		'src/rad/palette.cpp',
		'src/rad/phppanel/phppanel.cpp',
		'src/rad/pythonpanel/pythonpanel.cpp',
		'src/rad/title.cpp',
		'src/rad/wxfbevent.cpp',
		'src/rad/wxfbmanager.cpp',
		'src/rad/xrcpanel/xrcpanel.cpp',
		'src/rad/xrcpreview/xrcpreview.cpp',
		'src/utils/annoyingdialog.cpp',
		'src/utils/encodingutils.cpp',
		'src/utils/filetocarray.cpp',
		'src/utils/stringutils.cpp',
		'src/utils/typeconv.cpp',
		'src/utils/typeconv.cpp',
		'src/utils/wxfbipc.cpp',
	], dependencies : [ticpp_dep, wx_dep], include_directories : inc,
	link_with : plugin_interface, install : true)

foreach plugin : ['common', 'containers', 'forms', 'layout', 'additional']
	shared_library(plugin, 'plugins/@0@/@0@.cpp'.format(plugin), dependencies : [ticpp_dep, wx_dep],
		include_directories : inc, link_with : plugin_interface, install : true,
		install_dir : '@0@/wxformbuilder'.format(get_option('libdir')))
endforeach

install_subdir('output/xml', install_dir : 'share/wxformbuilder')
install_subdir('output/resources', install_dir : 'share/wxformbuilder')
install_subdir('output/plugins', install_dir : 'share/wxformbuilder')
