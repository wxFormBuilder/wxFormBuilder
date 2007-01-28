
// interfaz para plugins
// la idea es proporcionar una interfaz para acceder a las propiedades del
// objeto de manera segura desde el plugin.

class IObject
{
 public:
  virtual int GetPropertyAsInteger(const wxString& property) = 0;
  virtual wxFont GetPropertyAsFont(const wxString& property) = 0;
  virtual wxColour GetPropertyAsColour(const wxString& property) = 0;
  virtual wxString GetPropertyAsString(const wxString& property) = 0;
  virtual wxPoint GetPropertyAsPoint(const wxString& property) = 0;
  virtual wxSize GetPropertyAsSize(const wxString& property) = 0;
  virtual wxBitmap GetPropertyAsBitmap(const wxString& property) = 0;
};



