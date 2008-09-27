////////////////////////////////////////////////////////////////////////////////
//! \file   FindDlg.cpp
//! \brief  The FindDlg class definition.
//! \author Chris Oldwood

#include "Common.hpp"
#include "FindDlg.hpp"
#include "Resource.h"

////////////////////////////////////////////////////////////////////////////////
//! Default constructor.

FindDlg::FindDlg()
	: CDialog(IDD_FIND)
{
	DEFINE_CTRL_TABLE
		CTRL(IDC_PATH,	&m_ebQuery)
	END_CTRL_TABLE

	DEFINE_CTRLMSG_TABLE
	END_CTRLMSG_TABLE
}

////////////////////////////////////////////////////////////////////////////////
//! Dialog initialisation handler.

void FindDlg::OnInitDialog()
{
	// Initialise controls.
	m_ebQuery.Text(m_strQuery);
}

////////////////////////////////////////////////////////////////////////////////
//! OK button handler.

bool FindDlg::OnOk()
{
	// Validate controls.
	if (m_ebQuery.TextLength() == 0)
	{
		AlertMsg(TXT("Please enter an XPath expression query"));
		m_ebQuery.Focus();
		return false;
	}

	// Save parameters.
	m_strQuery = m_ebQuery.Text();

	return true;
}
