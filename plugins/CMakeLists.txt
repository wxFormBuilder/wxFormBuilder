# TODO: Since media is currently a required component always enable the control
add_plugin(additional
  DEFINITIONS
    USE_MEDIACTRL
)
add_plugin(common)
add_plugin(containers
  SOURCES
    bookutils.h
)
add_plugin(forms)
add_plugin(layout)
