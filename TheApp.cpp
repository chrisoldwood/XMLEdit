////////////////////////////////////////////////////////////////////////////////
//! \file   TheApp.cpp
//! \brief  The TheApp class definition.
//! \author Chris Oldwood

#include "Common.hpp"
#include "TheApp.hpp"
#include "TheDoc.hpp"
#include "TheView.hpp"
#include <WCL/IniFile.hpp>

////////////////////////////////////////////////////////////////////////////////
// Global variables.

//! The application singleton instance.
TheApp App;

////////////////////////////////////////////////////////////////////////////////
// Constants.

#ifdef _DEBUG
//! The application version string.
const tchar* TheApp::VERSION = TXT("v1.0 [Debug]");
#else
//! The application version string.
const tchar* TheApp::VERSION = TXT("v1.0");
#endif

//! The .ini file format version number.
const tchar* TheApp::INI_FILE_VER = TXT("1.0");

const int MRU_LIST_SIZE = ID_FILE_MRU_4-ID_FILE_MRU_1+1;

////////////////////////////////////////////////////////////////////////////////
//! Constructor.

TheApp::TheApp()
	: CSDIApp(m_oAppWnd, m_oAppCmds, MRU_LIST_SIZE)
	, m_nMaxSummaryLen(50)
{

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

	// Write the UI settings.
	oIniFile.WriteRect(TXT("UI"), TXT("MainWindow"), m_rcLastPos);
}
