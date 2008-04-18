////////////////////////////////////////////////////////////////////////////////
//! \file   TheView.hpp
//! \brief  The TheView class declaration.
//! \author Chris Oldwood

// Check for previous inclusion
#ifndef APP_THEVIEW_HPP
#define APP_THEVIEW_HPP

#include <WCL/View.hpp>
#include <WCL/SplitWnd.hpp>
#include <WCL/ListView.hpp>
#include <WCL/EditBox.hpp>
#include "TheDoc.hpp"
#include "XmlTreeView.hpp"

////////////////////////////////////////////////////////////////////////////////
//! The view for the document.

class TheView : public CView
{
public:
	//! Constructor.
	TheView(TheDoc& rDoc);

	//! Destructor.
	virtual ~TheView();
	
	//
	// Properties.
	//

	//! Get the document.
	TheDoc& Document();

private:
	//
	// Members.
	//
	CSplitWnd		m_wndMainSplit;		//!< The tree/details split window.
	XmlTreeView		m_tvNodeTree;		//!< The DOM tree view.
	CListView		m_lvAttributes;		//!< The node attributes view.
	CEditBox		m_ebValue;			//!< The node value view.

	//! The ID of the main split window.
	static const uint IDC_MAIN_SPLIT = 100;
	//! The ID of the DOM tree control.
	static const uint IDC_DOM_TREE = 102;
	//! The ID of the node attributes control.
	static const uint IDC_ATTRIBUTES = 103;
	//! The ID of the node value control.
	static const uint IDC_VALUE = 104;

	//
	// Message handlers.
	//

	//! Handle window creation.
	virtual void OnCreate(const CRect& rcClient);

	//! Handle window resizing.
	virtual void OnResize(int iFlag, const CSize& rNewSize);

	//! Handle a selection change in the node tree.
	void OnNodeSelected(NMTREEVIEW& oMsg);

	//
	// Internal methods.
	//

	//! Initialise the view from the DOM.
	void InitialiseView();

	//
	// Friends.
	//

	//! Allow the DOM tree-view to reflect events back.
	friend class XmlTreeView;
};

////////////////////////////////////////////////////////////////////////////////
//! Get the document.

inline TheDoc& TheView::Document()
{
	return static_cast<TheDoc&>(m_Doc);
}

#endif // APP_THEVIEW_HPP
