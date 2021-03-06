////////////////////////////////////////////////////////////////////////////////
//! \file   TheDoc.cpp
//! \brief  The TheDoc class definition.
//! \author Chris Oldwood

#include "Common.hpp"
#include "TheDoc.hpp"
#include "TheView.hpp"
#include <WCL/App.hpp>
#include <WCL/FrameWnd.hpp>
#include <WCL/File.hpp>
#include <XML/Reader.hpp>
#include <XML/Writer.hpp>

////////////////////////////////////////////////////////////////////////////////
//! Constructor.

TheDoc::TheDoc()
	: m_pDOM(new XML::Document)
{
}

////////////////////////////////////////////////////////////////////////////////
//! Destructor.

TheDoc::~TheDoc()
{
}

////////////////////////////////////////////////////////////////////////////////
//! Get the view.

TheView* TheDoc::View() const
{
	return static_cast<TheView*>(m_pView);
}

////////////////////////////////////////////////////////////////////////////////
//! Checks if the document has been modified.

bool TheDoc::Modified() const
{
	return false;
}

////////////////////////////////////////////////////////////////////////////////
//! Load the document.

bool TheDoc::Load()
{
	try
	{
		CString strContents;

		CFile::ReadTextFile(m_Path, strContents);

		m_pDOM = XML::Reader::readDocument(strContents.begin(), strContents.end(), XML::Reader::DISCARD_WHITESPACE);
	}
	catch (const Core::Exception& e)
	{
		// Notify user.
		CApp::This().m_rMainWnd.AlertMsg(TXT("Failed to open the XML document:-\n\n%s"), e.twhat());
		return false;
	}

	return true;
}

////////////////////////////////////////////////////////////////////////////////
//! Save the document.

bool TheDoc::Save()
{
	try
	{
		CString contents = XML::Writer::writeDocument(m_pDOM).c_str();

		CFile::WriteTextFile(m_Path, contents, ANSI_TEXT);
	}
	catch (const Core::Exception& e)
	{
		// Notify user.
		CApp::This().m_rMainWnd.AlertMsg(TXT("Failed to save the XML document:-\n\n%s"), e.twhat());
		return false;
	}

	return true;
}
