////////////////////////////////////////////////////////////////////////////////
//! \file   AppCmds.hpp
//! \brief  The AppCmds class declaration.
//! \author Chris Oldwood

// Check for previous inclusion
#ifndef APP_APPCMDS_HPP
#define APP_APPCMDS_HPP

#if _MSC_VER > 1000
#pragma once
#endif

#include <WCL/SDICmds.hpp>

////////////////////////////////////////////////////////////////////////////////
//! The application command controller.

class AppCmds : public CSDICmds
{
public:
	//! Constructor.
	AppCmds();

	//! Destructor.
	virtual ~AppCmds();

	//
	// Commands.
	//

	//! Create a new document.
	void OnFileNew();

	//! Open an existing document.
	void OnFileOpen();

	//! Save the current document.
	void OnFileSave();

	//! Save the current document with another filename.
	void OnFileSaveAs();

	//! Close the current document.
	void OnFileClose();

	//! Open one of the previous documents.
	void OnFileOpenMRU(uint iCmdID);

	//! Close the application.
	void OnFileExit();

	//! Find the first node that matches an XPath expression.
	void OnEditFind();

	//! Find the next node that matches the previous expression.
	void OnEditFindNext();

	//! Change the layout to the horizontal one.
	void OnViewHorz();

	//! Change the layout to the vertical one.
	void OnViewVert();

	//! Show the full path to the select node.
	void OnViewNodePath();

	//! Show the about dialog.
	void OnHelpAbout();

	//
	// UI handlers.
	//

	//! Update the command UI.
	void OnUIFileNew();

	//! Update the command UI.
	void OnUIFileSave();

	//! Update the command UI.
	void OnUIFileSaveAs();

	//! Update the command UI.
	void OnUIFileClose();

	//! Update the command UI.
	void OnUIFileOpenMRU();

	//! Update the command UI.
	void OnUIEditFind();

	//! Update the command UI.
	void OnUIEditFindNext();

	//! Update the command UI.
	void OnUIViewHorz();

	//! Update the command UI.
	void OnUIViewVert();

	//! Update the command UI.
	void OnUIViewNodePath();
};

#endif // APP_APPCMDS_HPP
