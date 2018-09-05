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
// Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
//
// Written by
//   José Antonio Hurtado - joseantonio.hurtado@gmail.com
//   Juan Antonio Ortega  - jortegalalmolda@gmail.com
//
///////////////////////////////////////////////////////////////////////////////

#include "encodingutils.h"

#include <wx/wfstream.h>

void UTF8ToAnsi(const wxString& filename) {
	wxString temp_filename = filename + wxT(".tmp");

	::wxCopyFile(filename, temp_filename);
	::wxRemoveFile(filename);

	{
		wxFileInputStream input(temp_filename);
		wxFileOutputStream output(filename);

		if (input.IsOk() && output.IsOk()) {
			while (!input.Eof()) {
				unsigned char c;

				input.Read(&c, 1);

				if (input.LastRead() != 1) break;

				if (c == 0xC2 || c == 0xC3) {
					unsigned char aux = c;
					input.Read(&c, 1);
					if (input.LastRead() != 1) break;

					if (aux == 0xC3) c += 64;
				}

				output.Write(&c, 1);
			}
		}
	}

	::wxRemoveFile(temp_filename);
}
