////////////////////////////////////////////////////////////////////////////////
//! \file   FindDlg.hpp
//! \brief  The FindDlg class declaration.
//! \author Chris Oldwood

// Check for previous inclusion
#ifndef FINDDLG_HPP
#define FINDDLG_HPP

#if _MSC_VER > 1000
#pragma once
#endif

#include <WCL/CommonUI.hpp>

////////////////////////////////////////////////////////////////////////////////
//! The dialog used to enter the XPath expression used to find nodes.

class FindDlg : public CDialog
{
public:
	//! Default constructor.
	FindDlg();
	
	//
	// Members.
	//
	tstring		m_strQuery;		//!< The find XPath expression.

private:
	//
	// Controls.
	//
	CEditBox	m_ebQuery;		//!< The input control for the query.

	//
	// Message handlers.
	//

	//! Dialog initialisation handler.
	virtual void OnInitDialog();

	//! OK button handler.
	virtual bool OnOk();
};

#endif // FINDDLG_HPP
