///////////////////////////////////////////////////////////////////////////////
//
// wxFormBuilder - A Visual Dialog Editor for wxWidgets.
// Copyright (C) 2005 José Antonio Hurtado
//
// This program is free software; you can redistribute it and/or
// modify it under the terms of the GNU General Public License
// as published by the Free Software Foundation; either version 2
// of the License, or (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program; if not, write to the Free Software
// Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
//
// Written by
//   José Antonio Hurtado - joseantonio.hurtado@gmail.com
//   Juan Antonio Ortega  - jortegalalmolda@gmail.com
//
///////////////////////////////////////////////////////////////////////////////

#ifndef __MENUEDITOR__
#define __MENUEDITOR__
#include <wx/wx.h>
#include <wx/button.h>
#include <wx/listctrl.h>
#include "model/objectbase.h"
#include "model/database.h"

/**
 * Class MenuEditor
 */
class MenuEditor : public wxDialog
{
  private:
    /** Devuelve el índice del elemento seleccionado en el wxListCtrl */
    long GetSelectedItem();
    
    /** Devuelve la profundidad del elemento "n" del wxListCtrl */
    int GetItemIdentation(long n);
    
    /** Inserta en la posición "n" un elemento en el wxListCtrl. No añade
    sangría en el campo label. */
    long InsertItem(long n, const wxString& label, const wxString& shortcut,
        const wxString& id, const wxString& name, const wxString& helpString,
        const wxString& kind);
        
    /** Inserta debajo del elemento seleccionado (o en la última posición en
    caso de no haber selección) un nuevo elemento en el wxListCtrl. El campo
    label se sangra al mismo nivel que el elemento que le precede */
    void AddItem(const wxString& label, const wxString& shortcut, 
        const wxString& id, const wxString& name, const wxString &help,
        const wxString& kind);
        
    /** Determina el índice en el que acaba el menú que empieza en la posición
    "n" */
    long GetEndIndex(long n);
    
    /** Devuelve los campos del wxListCtrl del elemento "n". El campo label
    lo devuelve con sangría */
    void GetItem(long n, wxString& label, wxString& shortcut, wxString& id, 
        wxString& name, wxString& help, wxString& kind);
        
    /** Inserta en la posición "n" del wxListCtrl, los hijos de "obj". El primer
    hijo de "obj" estará sangrado con el valor de "ident". En "n" se devuelve
    la siguiente posición libre del wxListCtrl tras hacer la inserción */
    void AddChild(long& n, int ident, shared_ptr<ObjectBase> obj);
    
    /** Determina si el elemento "n" del wxListCtrl tiene submenús o por el 
    contrario es un menuitem */
    bool HasChildren(long n);
    
    /** Devuelve el ObjectBase correspondiente al menú que empieza en la posición
    "n" del wxListCtrl, así como todos los hijos de ese menú. En "n" devuelve
    la posición en el wxListCtrl del siguiente menú. isSubMenu debe ser true
    para los menús de primer nivel (aquellos que aparecerán en la barra de menús)
    y false para el resto (submenús dentro de los menús). */
    shared_ptr<ObjectBase> GetMenu(long& n, PObjectDatabase base, bool isSubMenu = true);
    
    /** Toma los valores introducidos por el usuario en los campos e inserta
    el elemento en el wxListCtrl. Tras la inserción, vacía todos los campos
    y pone el foco en el campo "label". */
    void AddNewItem();
  
  protected:
    wxListCtrl *m_menuList;
    wxTextCtrl *m_tcId;
    wxTextCtrl *m_tcLabel;
    wxTextCtrl *m_tcName;
    wxTextCtrl *m_tcHelpString;
    wxTextCtrl *m_tcShortcut;
    wxRadioBox *m_rbItemKind;
    
    DECLARE_EVENT_TABLE()
  
  public:
    
    /** Constructor */
    MenuEditor(wxWindow *parent, int id = -1);
    
    /** Rellena el wxListCtrl con los datos de "obj", que debe ser de tipo
    menubar */
    void Populate(shared_ptr<ObjectBase> obj);
    
    /** Devuelve la jerarquía de ObjectBase correspondiente a la barra de 
    menús que hay en el wxListCtrl. El objeto devuelto será, por tanto, de
    tipo wxMenuBar */
    shared_ptr<ObjectBase> GetMenubar(PObjectDatabase base);
    
    /** Eventos de la interfaz de usuario */
    void OnAddMenuItem(wxCommandEvent& e);
    void OnAddSeparator(wxCommandEvent& e);
    void OnModifyMenuItem(wxCommandEvent& e);
    void OnRemoveMenuItem(wxCommandEvent& e);
    void OnMenuDown(wxCommandEvent& e);
    void OnMenuLeft(wxCommandEvent& e);
    void OnMenuRight(wxCommandEvent& e);
    void OnMenuUp(wxCommandEvent& e);
    void OnEnter(wxCommandEvent& e);
    void OnLabelChanged(wxCommandEvent& e);
    void OnUpdateMovers(wxUpdateUIEvent& e);
    void OnItemActivated(wxListEvent& e);
};

#endif //__menueditor__

