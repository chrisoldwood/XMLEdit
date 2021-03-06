////////////////////////////////////////////////////////////////////////////////
//! \file   AppWnd.cpp
//! \brief  The AppWnd class definition.
//! \author Chris Oldwood

#include "Common.hpp"
#include "AppWnd.hpp"
#include "TheApp.hpp"
#include "TheDoc.hpp"

////////////////////////////////////////////////////////////////////////////////
//! Constructor.

AppWnd::AppWnd(WCL::IMsgThread& thread, WCL::ICmdController& controller)
	: CSDIFrame(IDR_APPICON)
	, m_oToolbar(thread, controller)
{
}

////////////////////////////////////////////////////////////////////////////////
//! Destructor.

AppWnd::~AppWnd()
{
}

////////////////////////////////////////////////////////////////////////////////
//! Handle window creation.

void AppWnd::OnCreate(const CRect& rcClient)
{
	//
	// Create and attach the components.
	//
	m_oAccel.LoadRsc(IDR_APPACCEL);
	Accel(&m_oAccel);

	m_oMenu.LoadRsc(IDR_APPMENU);
	Menu(&m_oMenu);

	m_oToolbar.Create(*this, IDC_TOOL_BAR, rcClient);
	ToolBar(&m_oToolbar);

	m_oStatusbar.Create(*this, IDC_STATUS_BAR, rcClient);
	StatusBar(&m_oStatusbar);

	// Call base class.
	CSDIFrame::OnCreate(rcClient);
}

////////////////////////////////////////////////////////////////////////////////
//! Handle the window closing.

void AppWnd::OnClose()
{
	// Fetch windows final placement.
	App.m_rcLastPos = Placement();
}

////////////////////////////////////////////////////////////////////////////////
//! Handle the window being activated.

void AppWnd::OnActivate(bool bActivating)
{
	if ( (bActivating) && (App.Document() != nullptr) )
		App.Document()->View()->Activate();
}
