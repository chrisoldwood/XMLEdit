////////////////////////////////////////////////////////////////////////////////
//! \file   TheApp.hpp
//! \brief  The TheApp class declaration.
//! \author Chris Oldwood

// Check for previous inclusion
#ifndef APP_THEAPP_HPP
#define APP_THEAPP_HPP

#if _MSC_VER > 1000
#pragma once
#endif

#include <WCL/SDIApp.hpp>
#include "AppWnd.hpp"
#include "AppCmds.hpp"
#include "TheView.hpp"

// Forward declarations.
class TheDoc;

////////////////////////////////////////////////////////////////////////////////
//! The application singleton.

class TheApp : public CSDIApp
{
public:
	//! Constructor.
	TheApp();

	//! Destructor.
	virtual ~TheApp();

	//! Get the current open document.
	TheDoc* Document() const;

	//! The array of ListView column widths.
	typedef std::vector<uint> Widths;

	//
	// Application objects..
	//
	AppWnd		m_oAppWnd;			//!< The main window.
	AppCmds		m_oAppCmds;			//!< The command handler.

	//
	// Application settings.
	//
	CRect			m_rcLastPos;		//!< Main window previous position.
	uint			m_nDefMaxItemLen;	//!< The default max chars in a tree item summary.
	TheView::Layout	m_eDefLayout;		//!< The default main view layout.
	uint			m_nDefSplitPos;		//!< The default splitter bar position.
	Widths			m_vecDefColWidths;	//!< The default attributes view column widths.

	//
	// Find state.
	//
	typedef std::list<XML::NodePtr> NodesList;

	tstring			m_strLastSearch;	//!< The last find XPath query.
	NodesList		m_lstQueryNodes;	//!< The list of nodes found in the last query.

private:
	//
	// Template methods.
	//

	//! Handle application startup.
	virtual	bool OnOpen();

	//! Handle application shutdown.
	virtual	bool OnClose();

	//! Create a document object.
	virtual CSDIDoc* CreateDoc() const;

	//! Create a view for the document.
	virtual CView* CreateView(CDoc& rDoc) const;

	//! Get the list of supported file extendsions.
	virtual const tchar* FileExts() const;

	//! Get the default file extension.
	virtual const tchar* DefFileExt() const;

	//
	// Internal methods.
	//

	//! Load the application settings.
	void LoadConfig();

	//! Save the application settings.
	void SaveConfig();
};

//! The application singleton instance.
extern TheApp App;

#endif // APP_THEAPP_HPP
