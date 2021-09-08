function(add_plugin pluginName)
  add_library("wxFormBuilder_${pluginName}" MODULE)
  add_library("wxFormBuilder::${pluginName}" ALIAS "wxFormBuilder_${pluginName}")
  set_target_properties("wxFormBuilder_${pluginName}" PROPERTIES
      OUTPUT_NAME "$<$<BOOL:${WIN32}>:lib>${pluginName}"
  )

  target_sources("wxFormBuilder_${pluginName}"
    PRIVATE
      "${pluginName}/${pluginName}.cpp"
  )
  target_link_libraries("wxFormBuilder_${pluginName}"
    PRIVATE
      wxFormBuilder::plugin-interface
  )
endfunction()
