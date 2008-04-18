////////////////////////////////////////////////////////////////////////////////
//! \file   TheDoc.hpp
//! \brief  The TheDoc class declaration.
//! \author Chris Oldwood

// Check for previous inclusion
#ifndef APP_THEDOC_HPP
#define APP_THEDOC_HPP

#if _MSC_VER > 1000
#pragma once
#endif

#include <WCL/SDIDoc.hpp>
#include <XML/Document.hpp>

////////////////////////////////////////////////////////////////////////////////
//! The document.

class TheDoc : public CSDIDoc
{
public:
	//! Constructor.
	TheDoc();

	//! Destructor.
	virtual ~TheDoc();

	//
	// Properties.
	//

	//! Checks if the document has been modified.
	virtual bool Modified() const;

	//! Get the underlying XML DOM document.
	XML::DocumentPtr DOM() const;

	//
	// Methods.
	//

	//! Load the document.
	virtual bool Load();

	//! Save the document.
	virtual bool Save();

private:
	//
	// Members.
	//
	XML::DocumentPtr	m_pDOM;		//!< The XML DOM document.
};

////////////////////////////////////////////////////////////////////////////////
//! Get the underlying XML DOM document.

inline XML::DocumentPtr TheDoc::DOM() const
{
	return m_pDOM;
}

#endif // APP_THEDOC_HPP
