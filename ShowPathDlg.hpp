////////////////////////////////////////////////////////////////////////////////
//! \file   ShowPathDlg.hpp
//! \brief  The ShowPathDlg class declaration.
//! \author Chris Oldwood

// Check for previous inclusion
#ifndef SHOWPATHDLG_HPP
#define SHOWPATHDLG_HPP

#if _MSC_VER > 1000
#pragma once
#endif

#include <WCL/CommonUI.hpp>

////////////////////////////////////////////////////////////////////////////////
//! The dialog used to display the full path to the node.

class ShowPathDlg : public CDialog
{
public:
	//! Default constructor.
	ShowPathDlg();
	
	//
	// Members.
	//
	tstring		m_strPath;		//!< The path to the node.

private:
	//
	// Controls.
	//
	CEditBox	m_ebPath;		//!< The display for the path.

	//
	// Message handlers.
	//

	//! Dialog initialisation handler.
	virtual void OnInitDialog();

	//! OK button handler.
	virtual bool OnOk();
};

#endif // SHOWPATHDLG_HPP
