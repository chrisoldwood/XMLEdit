////////////////////////////////////////////////////////////////////////////////
//! \file   ShowPathDlg.cpp
//! \brief  The ShowPathDlg class definition.
//! \author Chris Oldwood

#include "Common.hpp"
#include "ShowPathDlg.hpp"
#include "Resource.h"

////////////////////////////////////////////////////////////////////////////////
//! Default constructor.

ShowPathDlg::ShowPathDlg()
	: CDialog(IDD_NODE_PATH)
{
	DEFINE_CTRL_TABLE
		CTRL(IDC_PATH,	&m_ebPath)
	END_CTRL_TABLE

	DEFINE_CTRLMSG_TABLE
	END_CTRLMSG_TABLE
}

////////////////////////////////////////////////////////////////////////////////
//! Dialog initialisation handler.

void ShowPathDlg::OnInitDialog()
{
	// Initialise controls.
	m_ebPath.Text(m_strPath);
}

////////////////////////////////////////////////////////////////////////////////
//! OK button handler.

bool ShowPathDlg::OnOk()
{
	return true;
}
