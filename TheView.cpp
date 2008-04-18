////////////////////////////////////////////////////////////////////////////////
//! \file   TheView.cpp
//! \brief  The TheView class definition.
//! \author Chris Oldwood

#include "Common.hpp"
#include "TheView.hpp"
#include "TheDoc.hpp"
#include "Resource.h"
#include "TheApp.hpp"
#include <XML/Document.hpp>
#include <XML/TextNode.hpp>
#include <XML/ElementNode.hpp>
#include <XML/CommentNode.hpp>
#include <XML/ProcessingNode.hpp>
#include <XML/DocTypeNode.hpp>

////////////////////////////////////////////////////////////////////////////////
//! Constructor.

TheView::TheView(TheDoc& rDoc)
	: CView(rDoc)
	, m_wndMainSplit(CSplitWnd::VERTICAL, CSplitWnd::RESIZEABLE)
	, m_tvNodeTree(*this)
{
/*
	DEFINE_CTRLMSG_TABLE
	END_CTRLMSG_TABLE
*/
}

////////////////////////////////////////////////////////////////////////////////
//! Destructor.

TheView::~TheView()
{
}

////////////////////////////////////////////////////////////////////////////////
//! Handle window creation.

void TheView::OnCreate(const CRect& /*rcClient*/)
{
	CFont fntFixed(ANSI_FIXED_FONT);

	// Create the splitter windows that fill the view.
	m_wndMainSplit.Create(*this, IDC_MAIN_SPLIT, ClientRect());

	// Create the child controls.
	m_tvNodeTree.Create(m_wndMainSplit, IDC_DOM_TREE, m_wndMainSplit.ClientRect());
	m_lvAttributes.Create(m_wndMainSplit, IDC_ATTRIBUTES, m_wndMainSplit.ClientRect());
	m_ebValue.Create(m_wndMainSplit, IDC_VALUE, m_wndMainSplit.ClientRect(), WS_EX_CLIENTEDGE,
										WS_CHILD | WS_CLIPSIBLINGS | WS_CLIPCHILDREN | WS_VISIBLE
										| ES_MULTILINE | ES_AUTOVSCROLL | ES_LEFT);

	// Add the child controls to the splitters.
	m_wndMainSplit.SetPane(CSplitWnd::LEFT_PANE, &m_tvNodeTree);
	m_wndMainSplit.SetSizingBarPos(500);

	// Initialise the controls.
	m_tvNodeTree.Font(fntFixed);
	m_tvNodeTree.SetImageList(TVSIL_NORMAL, IDB_NODE_ICONS, 16, RGB(255, 255, 255));

	m_lvAttributes.Font(fntFixed);
	m_lvAttributes.InsertColumn(0, TXT("Name"), 100, LVCFMT_LEFT);
	m_lvAttributes.InsertColumn(1, TXT("Value"), 100, LVCFMT_LEFT);
	m_lvAttributes.FullRowSelect(true);

	m_ebValue.Font(fntFixed);
	m_ebValue.ReadOnly(true);

	// Display the current DOM.
	InitialiseView();
}

////////////////////////////////////////////////////////////////////////////////
//! Handle window resizing.

void TheView::OnResize(int /*iFlag*/, const CSize& /*rNewSize*/)
{
	m_wndMainSplit.Move(ClientRect());
}

////////////////////////////////////////////////////////////////////////////////
//! Handle a selection change in the node tree.

void TheView::OnNodeSelected(NMTREEVIEW& oMsg)
{
	ASSERT(oMsg.hdr.hwndFrom == m_tvNodeTree.Handle());
	ASSERT(oMsg.hdr.code     == TVN_SELCHANGED);

	XML::NodePtr pNode = m_tvNodeTree.GetItemNode(oMsg.itemNew.hItem);

	ASSERT(pNode.Get() != nullptr);

	XML::NodeType eType = pNode->Type();

	// Has attributes?
	if ( (eType == XML::ELEMENT_NODE) || (eType == XML::PROCESSING_NODE) )
	{
		XML::Attributes vAttribs;

		// Extract the attributes.
		if (eType == XML::ELEMENT_NODE)
		{
			vAttribs = Core::static_ptr_cast<XML::ElementNode>(pNode)->GetAttributes();
		}
		else if (eType == XML::PROCESSING_NODE)
		{
			vAttribs = Core::static_ptr_cast<XML::ProcessingNode>(pNode)->GetAttributes();
		}

		// Switch info controls and display attributes.
		m_wndMainSplit.SetPane(CSplitWnd::RIGHT_PANE, &m_lvAttributes);

		m_lvAttributes.DeleteAllItems();

		for (XML::Attributes::const_iterator it = vAttribs.Begin(); it != vAttribs.End(); ++it)
		{
			const XML::AttributePtr& pAttribute = *it;

			size_t n = m_lvAttributes.ItemCount();
			m_lvAttributes.InsertItem(n,    pAttribute->Name().c_str());
			m_lvAttributes.ItemText  (n, 1, pAttribute->Value().c_str());
		}
	}
	// Is content?
	else if ( (eType == XML::TEXT_NODE) || (eType == XML::COMMENT_NODE)
		   || (eType == XML::DOCTYPE_NODE) )
	{
		tstring strText;

		// Extract text value.
		if (eType == XML::TEXT_NODE)
		{
			strText = Core::static_ptr_cast<XML::TextNode>(pNode)->Text();
		}
		else if (eType == XML::COMMENT_NODE)
		{
			strText = Core::static_ptr_cast<XML::CommentNode>(pNode)->Comment();
		}
		else if (eType == XML::DOCTYPE_NODE)
		{
			strText = Core::static_ptr_cast<XML::DocTypeNode>(pNode)->Declaration();
		}

		// Switch info controls and display text.
		m_wndMainSplit.SetPane(CSplitWnd::RIGHT_PANE, &m_ebValue);
		m_ebValue.Text(strText.c_str());
	}
	// No proprties.
	else
	{
		m_wndMainSplit.SetPane(CSplitWnd::RIGHT_PANE, nullptr);
	}
}

////////////////////////////////////////////////////////////////////////////////
//! Initialise the view from the DOM.

void TheView::InitialiseView()
{
	m_tvNodeTree.Refresh();
}
