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

#ifndef __COMMAND_PROC__
#define __COMMAND_PROC__

#include <stack>
#include <boost/smart_ptr.hpp>

class Command;
typedef boost::shared_ptr<Command> PCommand;

class CommandProcessor
{
 private:
  typedef std::stack<PCommand> CommandStack;

  CommandStack m_undoStack;
  CommandStack m_redoStack;

 public:
   void Execute(PCommand command);

   void Undo();
   void Redo();
   void Reset();

   bool CanUndo();
   bool CanRedo();
};


class Command
{
 private:
  bool m_executed;

 protected:
  /**
   * Ejecuta el comando.
   */
  virtual void DoExecute() = 0;

  /**
   * Restaura el estado previo a la ejecución del comando.
   */
  virtual void DoRestore() = 0;

 public:
  Command();
  virtual ~Command() {};

  void Execute();
  void Restore();
};

#endif //__COMMAND_PROC__
