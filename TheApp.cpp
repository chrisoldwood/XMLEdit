////////////////////////////////////////////////////////////////////////////////
//! \file   TheApp.cpp
//! \brief  The TheApp class definition.
//! \author Chris Oldwood

#include "Common.hpp"
#include "TheApp.hpp"
#include "TheDoc.hpp"
#include "TheView.hpp"
#include <WCL/IniFile.hpp>
#include <WCL/StrCvt.hpp>

////////////////////////////////////////////////////////////////////////////////
// Global variables.

//! The application singleton instance.
TheApp App;

////////////////////////////////////////////////////////////////////////////////
// Constants.

//! The .ini file format version number.
const tchar* TheApp::INI_FILE_VER = TXT("1.0");

//! The maximum size of the MRU list.
const int MRU_LIST_SIZE = ID_FILE_MRU_4-ID_FILE_MRU_1+1;

////////////////////////////////////////////////////////////////////////////////
//! Constructor.

TheApp::TheApp()
	: CSDIApp(m_oAppWnd, m_oAppCmds, MRU_LIST_SIZE)
	, m_nDefMaxItemLen(150)
	, m_eDefLayout(TheView::VERTICAL)
	, m_nDefSplitPos(500)
	, m_vecDefColWidths(2)
{
	m_vecDefColWidths[0] = 100;
	m_vecDefColWidths[1] = 100;
}

////////////////////////////////////////////////////////////////////////////////
//! Destructor.

TheApp::~TheApp()
{

}

////////////////////////////////////////////////////////////////////////////////
//! Handle application startup.

bool TheApp::OnOpen()
{
	// Set the app title.
	m_strTitle = TXT("XML Editor");

	// Load settings.
	LoadConfig();
	
	// Load the toolbar bitmap.
	m_rCmdControl.CmdBitmap().LoadRsc(IDR_APPTOOLBAR);

	// Create the main window.
	if (!m_oAppWnd.Create())
		return false;

	// Show it.
	if (ShowNormal() && !m_rcLastPos.Empty())
		m_oAppWnd.Move(m_rcLastPos);

	m_oAppWnd.Show(m_iCmdShow);

	// Update UI.
	m_oAppCmds.UpdateUI();

	return true;
}

////////////////////////////////////////////////////////////////////////////////
//! Handle application shutdown.

bool TheApp::OnClose()
{
	// Save settings.
	SaveConfig();

	return true;
}

////////////////////////////////////////////////////////////////////////////////
//! Get the current open document.

TheDoc* TheApp::Document() const
{
	return static_cast<TheDoc*>(m_pDoc);
}

////////////////////////////////////////////////////////////////////////////////
//! Create a document object.

CSDIDoc* TheApp::CreateDoc() const
{
	return new TheDoc;
}

////////////////////////////////////////////////////////////////////////////////
//! Create a view for the document.

CView* TheApp::CreateView(CDoc& rDoc) const
{
	return new TheView(static_cast<TheDoc&>(rDoc));
}

////////////////////////////////////////////////////////////////////////////////
//! Get the list of supported file extendsions.

const tchar* TheApp::FileExts() const
{
	static tchar szExts[] = {	TXT("XML Files (*.xml)\0*.xml\0")
								TXT("All Files (*.*)\0*.*\0")
								TXT("\0\0")							};
									    
	return szExts;
}

////////////////////////////////////////////////////////////////////////////////
//! Get the default file extension.

const tchar* TheApp::DefFileExt() const
{
	static tchar szDefExt[] = { TXT("xml") };

	return szDefExt;
}

////////////////////////////////////////////////////////////////////////////////
//! Load the application settings.

void TheApp::LoadConfig()
{
	CIniFile oIniFile;

	// Read the file version.
	CString strVer = oIniFile.ReadString(TXT("Version"), TXT("Version"), INI_FILE_VER);

	// Read the MRU list.
	m_MRUList.Load(oIniFile);

	// Read the UI settings.
	m_rcLastPos = oIniFile.ReadRect(TXT("UI"), TXT("MainWindow"), m_rcLastPos);
	m_eDefLayout = static_cast<TheView::Layout>(oIniFile.ReadUInt(TXT("UI"), TXT("Layout"), m_eDefLayout));
	m_nDefSplitPos = oIniFile.ReadUInt(TXT("UI"), TXT("SplitterPos"), m_nDefSplitPos);

	CStrArray astrWidths = oIniFile.ReadStrings(TXT("UI"), TXT("AttribWidths"), TXT(','), TXT("100, 100"));

	if (astrWidths.Size() == m_vecDefColWidths.size())
	{
		for (size_t i = 0; i < m_vecDefColWidths.size(); ++i)
			m_vecDefColWidths[i] = tatoi(astrWidths[i]);
	}

	// Validate.
	if ( (m_eDefLayout != TheView::VERTICAL) && (m_eDefLayout != TheView::HORIZONTAL) )
		m_eDefLayout = TheView::VERTICAL;

	for (Widths::iterator itWidth = m_vecDefColWidths.begin(); itWidth != m_vecDefColWidths.end(); ++itWidth)
	{
		if (*itWidth == 0)
			*itWidth = 100;
	}
}

////////////////////////////////////////////////////////////////////////////////
//! Save the application settings.

void TheApp::SaveConfig()
{
	CIniFile oIniFile;

	// Write the file version.
	oIniFile.WriteString(TXT("Version"), TXT("Version"), INI_FILE_VER);

	// Save the MRU list.
	m_MRUList.Save(oIniFile);

	CStrArray astrWidths;

	for (Widths::const_iterator itWidth = m_vecDefColWidths.begin(); itWidth != m_vecDefColWidths.end(); ++itWidth)
		astrWidths.Add(CStrCvt::FormatUInt(*itWidth));

	// Write the UI settings.
	oIniFile.WriteRect(TXT("UI"), TXT("MainWindow"), m_rcLastPos);
	oIniFile.WriteUInt(TXT("UI"), TXT("Layout"), m_eDefLayout);
	oIniFile.WriteUInt(TXT("UI"), TXT("SplitterPos"), m_nDefSplitPos);
	oIniFile.WriteStrings(TXT("UI"), TXT("AttribWidths"), TXT(','), astrWidths);
}
