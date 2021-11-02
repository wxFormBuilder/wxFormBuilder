## A brief summary of informations to others wxErlang users/devs

Here you have some informations you may need to know about in order to write the 
_*.erlangcode_ files (and any other _*.xxxxcode_) the way the source code will be 
generated. That includes the syntax used for the **wxFormBuilder** macros and tips
about the text formation.
<br>
I hope these informations can save someone's time when trying to help us to maintain the configuration files updated.

<br>

### wxFormBuilder's source code of interest
There are two files in the project _/src/codegen/**erlangcg.cpp/.h**_ and _/src/rad/erlangpanel/**erlangpanel.cpp/.h**_ and the first one is the most likely will need to be updated.
<br>That's because it handles the custom templates as well as has information about the controls not supported by Erlang.

<br>

### Controls available in wxFormBuilder, but unsupported by wxErlang

* **Common** tab {file: common.erlangcode}
	- _wxAnimationCtrl, wxBitmapComboBox_
* **Aditional** tab {file: aditional.erlangcode}
	- _wxRichTextCtrl, wxBitmapToggleButton, wxSearchCtrl, wxTimePickerCtrl, wxSpinCtrlDouble, wxHyperlinkCtrl, 
	wxTimer, CustomControl, wxMediaCtrl_
* **Data** tab {file: data.erlangcode}
	- _wxDataViewCtrl, wxDataViewTreeCtrl, wxDataViewListCtrl, dataViewListColumn, 
	wxDataViewColumn, wxTreeListCtrl, wxDataViewColumn, dataViewListCtrlColumn, 
	wxPropertyGrid, wxPropertyGridManager, propGridItem, propGridPage_ 
* **Containers** tab {file: containers.erlangcode}
	- _wxCollapsiblePane, wxSimplebook_
* **Menu**/**Toolbar** tab {file: menutoolbar.erlangcode}
	- _wxInfoBar, wxAuiToolBar_
* **Layout** tab {file: layout.erlangcode}
	- _wxWrapSizer_
* **Forms** tab {file: forms.erlangcode}
	- _wxWizard, wxWizardPageSimple_
* **Ribbon** tab {file: ribbon.erlangcode}
	- _wxRibbonBar_

<br>

### The configuration files
The files where the template for Erlang code construction are defined can be found under the folder **./wxFormBuilder/output**.

* In the sub-directory **/xml** we find the **default.xml** which is related to the languages properties and setting that we see on the GUI by selecting the **_Project_** item in the _Object Tree_ window.
<br>The templates to create the code skeleton (preamble, include, ..., epilogue) is found in  **default.erlangcode**.
* In the sub-directory **/plugins** we find the sub-directories where the templates to create the controls classes and set its properties and events are stored. They are _additional_, _common_, _containers_, _forms_ and _layout_.
<br>Each one has a sub-directory **/xml** where we find the ***.erlangcode** files.

<br>

#### Identifiers, variable, types and conditional code

* _$field_name_
```
The property/event name must be preceded by '$'
```
* _"value"_
```
The values to be used in the 'if' statement should be quoted
```
* _@{ code_block @}_ 
```
Code block delimiters
```
* _||_
```
Logical operator 'or'
```
* Controls _'type'_ processed by **_wxFormBuilder_**
```
notebook, flatnotebook, listbook, simplebook, choicebook, auinotebook, 
widget, expanded_widget, propgrid, propgridman, statusbar, component, 
container, menubar, menu, menuitem, submenu, toolbar, ribbonbar, ribbonpage, 
ribbonpanel, ribbonbuttonbar, ribbonbutton, ribbondropdownbutton, 
ribbonhybridbutton, ribbontogglebutton, ribbontoolbar, ribbontool, 
ribbondropdowntool, ribbonhybridtool, ribbontoggletool, ribbongallery
ribbongalleryitem, dataviewctrl, dataviewtreectrl, dataviewlistctrl, 
dataviewlistcolumn, dataviewcolumn, tool, splitter, treelistctrl, sizer, 
nonvisual, gbsizer, propgriditem, propgridpage, gbsizer, wizardpagesimple
```
<br>

**Exclusive** for Erlang - Code generator for macro substitution
* _*option_
```
It's used in events templates to tell the erlangcg.cpp code where to replace
the connect parameters accordingly.
```
* _*event_, _*wx_, _*state_, _*controls_, _*fun_ and _*create_win_fun_
```
They are used in the export templates (default.erlangcode) to tell the erlangcg.cpp code
where to replace the parameters to build the events/function headers accordingly.
```
<br>

#### Conditional statement
```
#ifnull $field_name
#ifnotnull $field_name

    Ex. #ifnull $style


#ifequal $field_name "value"
#ifnotequal $field_name "value"

    Ex.: #ifnotequal $maxlength "0"


#ifparentclassequal "value"
#ifparentclassnotequal "value"

    Ex.: #ifparentclassequal "wxAuiToolBar"


#ifparenttypeequal "type"
#ifparenttypenotequal "type"
#iftypeequal "type"
#iftypenotequal "type"

    Ex.: #ifparenttypeequal "widget || container || expanded_widget"
```

#### New line and indentation
```
#nl - It appends a new line to the template after it be evaluated by the parser.

    Ex.:
    <template name="evt_connect_OnMouseEvents">
      wxPanel:connect($name, left_down, *option), #nl
      wxPanel:connect($name, left_up, *option), #nl
      wxPanel:connect($name, middle_down, *option), #nl
      wxPanel:connect($name, middle_up, *option), #nl
      wxPanel:connect($name, right_down, *option), #nl
      wxPanel:connect($name, right_up, *option), #nl
    </template>

TIP: 1) When a template has many lines with conditionals put the #nl inside the code_block;
     2) When a template has many lines put the #nl after each line except in the last one;
     3) Let a blank space between it and the last char on the line;

#indent, #unindent - It cause the text to be indented and unindented in accordance with
    the value set 'statically' in erlangpanel.cpp (via InitStyledTextCtrl/1) - tab width is 4.
    
    Ex.:
    <template>
      %% wx_object callbacks #nl
      -export([init/1, terminate/2, code_change/3, #indent #indent #nl
      handle_sync_event/3, handle_event/2,
      handle_info/2, handle_call/3, handle_cast/2]). #unindent #unindent #nl
    </template>
    
    TIP: the indent should be placed before the new line in order to get 
     it applied in the next lines.
```

#### Accessing objects and classes name
```
#wxparent (for all object that goes inside a sizer),
#parent, #child, #class - They are used in all templates
    
    Ex.1:
    <templates class="wxStaticLine">
    <template name="construction">
      $name = #class:new(#wxparent $name, [#ifnotnull $id
                                           @{{id,$id }, @}
      :
    </template>
    </template>
    
    Ex.2:
    <template name="spacer_add">
    #ifequal #child $width "0"
    @{#ifnotequal #child $height "0"
        @{ wxSizer:addSpacer(#parent $name, #child $height), @}
    @}
    :
    </template>
    
    TIP: 1) When using the brackets as bellow ({id,$id }), the blank space before 
     the closing bracket is needed otherwise wxFormBuilder will faill to 
     parse the template. (it will be removed by the code in Erlanggc.cpp)
     On the other hand, the blank space before the code_block closing will be 
     respected and will be concatenated in the parsed template;
         2) Closing nested code_block may generate a blank space when something 
     comes after the last one like this:

    #ifnull $style
    @{ #ifnotnull $window_style
        @{{style,$window_style }@}
    @}]),
    
     It will be translated into: {style,?wxBORDER_DEFAULT} ]
     Prefer to write it like this:

    #ifnull $style
    @{ #ifnotnull $window_style
        @{{style,$window_style }@}@}]),	

#form and #wizard - Not used by Erlang code generation
```

#### Composing variables names
```
#append - Allow us to compose names for variables to be inserted in the code

    Ex.:
    #ifequal $Save "1"
    @{
      #nl $name #append _Save = wxButton:new(#wxparent $name, ?wxID_SAVE),
      #nl wxStdDialogButtonSizer:addButton($name, $name #append _Save),
    @}
```

#### Looping a list
```
#foreach
#pred (predefined)
#npred (predefined index)

    Ex.1:
    #ifnotnull $growablecols
    @{
      #foreach $growablecols
      @{ #class:addGrowableCol($name, #pred), @}
    @}
    
    Ex.2:
    #foreach $column_sizes
    @{ #class:setColSize($name, #npred, #pred), @}
```
<br>

### Testing the templates/generated code
We can use the werl/erl build the _.beam_ file and test the prototyped module:
1) **c**(_module_name_).
This will compile the module and tell us if our template is well defined or not; 

2) _module_name_**:start()**.
This will start the module and show the prototyped window

3) _module_name_**:init(** _[ {**mode**, [{debug, trace}] } | [ {**parent**, parent_window}] ]_ **)**. 
This option allow us to start the module with parameters.

Obs:
1) The **_parent_** option will be mandatory when the AUI management is used, otherwise we are going to get the error message: _"can't create wxWindow without parent"_ 
2) For AUI managed window we will need provide a parent windows otherwise we are get a wx error. So, a test would look like this:
```
wx:new([{debug,trace}]),
Frame = wxFrame:new(wx:null(), 6000, "wxErlang widgets", [{size,{1000,500}}]),
Aui = aui:start([{parent,Frame}]),
wxFrame:show(Frame).
:
% after close the window...
wxFrame:destroy(),
wx:destroy().
``` 

<br>

### Note:
There are some **'class'** templates which they does not references 
a wxWidgets control class as we use to know. For instance:
```
<templates class="Frame">
    <template name="base">wxFrame
    </template>
    <template name="cons_call">
        wxFrame:new(wx:null(), $id, $title, [#ifnotnull $pos
        :
        :
    </template>
```
In these cases we cannot use the creation method (_new_) using
the _#class_ tag. We must use the proper class name - in this case _wxFrame_.
That is valid for the event connections too.

The explanation is available in this [answer](https://github.com/wxFormBuilder/wxFormBuilder/issues/638#issuecomment-689953477) I got from the @sodevel user at GitHub.
