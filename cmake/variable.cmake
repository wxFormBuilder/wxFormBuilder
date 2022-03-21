cmake_minimum_required(VERSION 3.20)

#[[
Set the value of a variable and backup its previous value.

stash_variable(<variable>
               [VALUE <value>])

Stores the current value of <variable> and sets its new value to <value>.
The <variable> does not need to be defined.
If <value> is not specified, <variable> is unset.

Implementation note: The variable that holds the backup value is placed in the parent scope
                     and named stash_variable_backup_<variable>
]]
function(stash_variable arg_VARIABLE)
  set(options "")
  set(singleValues VALUE)
  set(multiValues "")
  cmake_parse_arguments(PARSE_ARGV 1 arg "${options}" "${singleValues}" "${multiValues}")

  if(DEFINED ${arg_VARIABLE})
    message(DEBUG "stash_variable: backing up ${arg_VARIABLE} (${${arg_VARIABLE}})")
    set(stash_variable_backup_${arg_VARIABLE} "${${arg_VARIABLE}}" PARENT_SCOPE)
  else()
    message(DEBUG "stash_variable: skipping backup ${arg_VARIABLE}")
    unset(stash_variable_backup_${arg_VARIABLE} PARENT_SCOPE)
  endif()

  message(DEBUG "stash_variable: setting ${arg_VARIABLE} (${arg_VALUE})")
  if(DEFINED arg_VALUE)
    set(${arg_VARIABLE} "${arg_VALUE}" PARENT_SCOPE)
  else()
    unset(${arg_VARIABLE} PARENT_SCOPE)
  endif()
endfunction()

#[[
Restore the backup value of a variable.

restore_variable(<variable>)

Restores the value of a stashed variable.
The value of <variable> must have been stored by a previous call of stash_variable().
]]
function(restore_variable arg_VARIABLE)
  if(DEFINED stash_variable_backup_${arg_VARIABLE})
    message(DEBUG "restore_variable: restoring ${arg_VARIABLE} (${stash_variable_backup_${arg_VARIABLE}})")
    set(${arg_VARIABLE} "${stash_variable_backup_${arg_VARIABLE}}" PARENT_SCOPE)
    unset(stash_variable_backup_${arg_VARIABLE} PARENT_SCOPE)
  else()
    message(DEBUG "restore_variable: unsetting ${arg_VARIABLE}")
    unset(${arg_VARIABLE} PARENT_SCOPE)
  endif()
endfunction()
