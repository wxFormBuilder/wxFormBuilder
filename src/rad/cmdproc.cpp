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

#include "cmdproc.h"

void CommandProcessor::Execute(PCommand command)
{
  command->Execute();
  m_undoStack.push(command);
  
  while (!m_redoStack.empty())
    m_redoStack.pop();
}

void CommandProcessor::Undo()
{
  if (!m_undoStack.empty())
  {
    PCommand command = m_undoStack.top();
    m_undoStack.pop();
    
    command->Restore();
    m_redoStack.push(command);
  }
}

void CommandProcessor::Redo()
{
  if (!m_redoStack.empty())
  {
    PCommand command = m_redoStack.top();
    m_redoStack.pop();
    
    command->Execute();
    m_undoStack.push(command);
  }
}

void CommandProcessor::Reset()
{
  while (!m_redoStack.empty())
    m_redoStack.pop();
    
  while (!m_undoStack.empty())
    m_undoStack.pop();  
}

bool CommandProcessor::CanUndo()
{
  return (!m_undoStack.empty());
}
bool CommandProcessor::CanRedo()
{
  return (!m_redoStack.empty());
}

///////////////////////////////////////////////////////////////////////////////
Command::Command()
{
  m_executed = false;
}

void Command::Execute()
{
  if (!m_executed)
  {
    DoExecute();
    m_executed = true;
  }
}

void Command::Restore()
{
  if (m_executed)
  {
    DoRestore();
    m_executed = false;
  }
}

