////////////////////////////////////////////////////////////////////////////////
//! \file   TheApp.cpp
//! \brief  The TheApp class definition.
//! \author Chris Oldwood

#include "Common.hpp"
#include "TheApp.hpp"
#include "TheDoc.hpp"
#include "TheView.hpp"
//#include <WCL/IniFile.hpp>
//#include <WCL/StrCvt.hpp>
#include <WCL/AppConfig.hpp>
#include <Core/ConfigurationException.hpp>
#include <Core/StringUtils.hpp>
#include <WCL/BusyCursor.hpp>

////////////////////////////////////////////////////////////////////////////////
// Global variables.

//! The application singleton instance.
TheApp App;

////////////////////////////////////////////////////////////////////////////////
// Constants.

//! The configuration data publisher name.
const tchar* PUBLISHER = TXT("Chris Oldwood");
//! The configuration data application name.
const tchar* APPLICATION = TXT("XML Editor");
//! The configuration data format version.
const tchar* CONFIG_VERSION = TXT("1.0");

//! The maximum size of the MRU list.
const int MRU_LIST_SIZE = ID_FILE_MRU_9-ID_FILE_MRU_1+1;

////////////////////////////////////////////////////////////////////////////////
//! Constructor.

TheApp::TheApp()
	: CSDIApp(m_oAppWnd, m_oAppCmds, MRU_LIST_SIZE)
	, m_oAppWnd(m_MainThread, m_oAppCmds)
	, m_oAppCmds(m_oAppWnd)
	, m_nDefMaxItemLen(150)
	, m_eDefLayout(TheView::VERTICAL)
	, m_nDefSplitPos(0)
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

	try
	{
		// Load settings.
		LoadConfig();
	}
	catch (const Core::Exception& e)
	{
		FatalMsg(TXT("Failed to configure the application:-\n\n%s"), e.twhat());
		return false;
	}
	
	// Create the main window.
	if (!m_oAppWnd.Create())
		return false;

	// Show it.
	if (!m_rcLastPos.Empty())
		m_oAppWnd.Move(m_rcLastPos);

	m_oAppWnd.Show(m_iCmdShow);

	// Update UI.
	m_oAppCmds.InitialiseUI();

	return CSDIApp::OnOpen();
}

////////////////////////////////////////////////////////////////////////////////
//! Handle application shutdown.

bool TheApp::OnClose()
{
	try
	{
		// Save settings.
		SaveConfig();
	}
	catch (const Core::Exception& e)
	{
		FatalMsg(TXT("Failed to save the application configuration:-\n\n%s"), e.twhat());
		return false;
	}

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
	CBusyCursor    busyCursor;
	WCL::AppConfig appConfig(PUBLISHER, APPLICATION);

	// Read the config data version.
	tstring version = appConfig.readString(appConfig.DEFAULT_SECTION, TXT("Version"), CONFIG_VERSION);

	if (version != CONFIG_VERSION)
		throw Core::ConfigurationException(Core::fmt(TXT("The configuration data is incompatible - '%s'"), version.c_str()));

	// Read the MRU list.
	m_MRUList.Read(appConfig);

	// Read the UI settings.
	m_rcLastPos = appConfig.readValue<CRect>(TXT("UI"), TXT("MainWindow"), m_rcLastPos);
	m_eDefLayout = static_cast<TheView::Layout>(appConfig.readValue<int>(TXT("UI"), TXT("Layout"), m_eDefLayout));
	m_nDefSplitPos = appConfig.readValue<uint>(TXT("UI"), TXT("SplitterPos"), m_nDefSplitPos);

	WCL::AppConfig::StringArray widths;
	appConfig.readList(TXT("UI"), TXT("AttribWidths"), TXT("100, 100"), widths);

	if (widths.size() == m_vecDefColWidths.size())
	{
		for (size_t i = 0; i < m_vecDefColWidths.size(); ++i)
			m_vecDefColWidths[i] = Core::parse<uint>(widths[i]);
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
	CBusyCursor    busyCursor;
	WCL::AppConfig appConfig(PUBLISHER, APPLICATION);

	// Write the config data version.
	appConfig.writeString(appConfig.DEFAULT_SECTION, TXT("Version"), CONFIG_VERSION);

	// Save the MRU list.
	m_MRUList.Write(appConfig);

	WCL::AppConfig::StringArray widths;

	for (Widths::const_iterator itWidth = m_vecDefColWidths.begin(); itWidth != m_vecDefColWidths.end(); ++itWidth)
		widths.push_back(Core::format<size_t>(*itWidth));

	// Write the UI settings.
	appConfig.writeValue<CRect>(TXT("UI"), TXT("MainWindow"), m_rcLastPos);
	appConfig.writeValue<uint>(TXT("UI"), TXT("Layout"), m_eDefLayout);
	appConfig.writeValue<uint>(TXT("UI"), TXT("SplitterPos"), m_nDefSplitPos);
	appConfig.writeList(TXT("UI"), TXT("AttribWidths"), widths);
}
