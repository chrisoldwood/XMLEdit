////////////////////////////////////////////////////////////////////////////////
//! \file   TheDoc.cpp
//! \brief  The TheDoc class definition.
//! \author Chris Oldwood

#include "Common.hpp"
#include "TheDoc.hpp"
#include <WCL/App.hpp>
#include <WCL/FrameWnd.hpp>
#include <WCL/File.hpp>
#include <XML/Reader.hpp>

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
		CString    strContents;
		TextFormat eFormat;

		// Read the entire file.
		CFile::ReadTextFile(m_Path, strContents, eFormat);

		XML::Reader oReader;

		// Parse the XML file.
		m_pDOM = oReader.ReadDocument(strContents.begin(), strContents.end(), XML::Reader::DISCARD_WHITESPACE);
	}
	catch (const Core::Exception& e)
	{
		// Notify user.
		CApp::This().m_rMainWnd.AlertMsg(TXT("Failed to open the XML document:-\n\n%s"), e.What());
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
	}
	catch (const Core::Exception& e)
	{
		// Notify user.
		CApp::This().m_rMainWnd.AlertMsg(TXT("Failed to save the XML document:-\n\n%s"), e.What());
		return false;
	}

	return true;
}