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
#include <XML/CDataNode.hpp>
#include <WCL/ScreenDC.hpp>

////////////////////////////////////////////////////////////////////////////////
//! Constructor.

TheView::TheView(TheDoc& rDoc)
	: CView(rDoc)
	, m_wndMainSplit(CSplitWnd::RESIZEABLE)
	, m_tvNodeTree(*this)
	, m_fntControls(ANSI_FIXED_FONT)
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
//! Get the document.

TheDoc& TheView::Document()
{
	return static_cast<TheDoc&>(m_Doc);
}

////////////////////////////////////////////////////////////////////////////////
//! Get the selected node.

XML::NodePtr TheView::Selection() const
{
	return m_tvNodeTree.Selection();
}

////////////////////////////////////////////////////////////////////////////////
//! Set the selected node.

void TheView::SetSelection(const XML::NodePtr& pNode)
{
	m_tvNodeTree.SetSelection(pNode);
}

////////////////////////////////////////////////////////////////////////////////
//! Set the layout of the panes.

void TheView::SetLayout(Layout eLayout)
{
	// Switch to a horizontal layout AND not already?
	if ( (eLayout == HORIZONTAL) && (m_wndMainSplit.SplitStyle() != CSplitWnd::VERTICAL) )
	{
		m_wndMainSplit.SetSplitStyle(CSplitWnd::VERTICAL);
	}
	// Switch to a vertical layout AND not already?
	else if ( (eLayout == VERTICAL) && (m_wndMainSplit.SplitStyle() != CSplitWnd::HORIZONTAL) )
	{
		m_wndMainSplit.SetSplitStyle(CSplitWnd::HORIZONTAL);
	}
}

////////////////////////////////////////////////////////////////////////////////
//! Activate the view.

void TheView::Activate()
{
	m_tvNodeTree.Focus();
}

////////////////////////////////////////////////////////////////////////////////
//! Handle window creation.

void TheView::OnCreate(const CRect& rcClient)
{
	// On first run ensure splitter is visible.
	if (App.m_nDefSplitPos == 0)
		App.m_nDefSplitPos = (rcClient.Height() * 3) / 4;

	// Create the splitter windows that fill the view.
	m_wndMainSplit.Create(*this, IDC_MAIN_SPLIT, rcClient);

	CRect rcEmpty;

	// Create the child controls.
	m_tvNodeTree.Create(m_wndMainSplit, IDC_DOM_TREE, rcEmpty, WS_EX_CLIENTEDGE,
										WS_CHILD | WS_CLIPSIBLINGS | WS_CLIPCHILDREN | WS_VISIBLE
										| WS_BORDER | TVS_HASBUTTONS | TVS_HASLINES | TVS_LINESATROOT | TVS_SHOWSELALWAYS);

	m_lvAttributes.Create(m_wndMainSplit, IDC_ATTRIBUTES, rcEmpty, WS_EX_CLIENTEDGE, 
										WS_CHILD | WS_CLIPSIBLINGS | WS_CLIPCHILDREN | WS_VISIBLE
										| WS_BORDER | LVS_REPORT | LVS_SINGLESEL | LVS_SHOWSELALWAYS | LVS_NOSORTHEADER);

	m_ebValue.Create(m_wndMainSplit, IDC_VALUE, rcEmpty, WS_EX_CLIENTEDGE,
										WS_CHILD | WS_CLIPSIBLINGS | WS_CLIPCHILDREN | WS_VISIBLE
										| ES_MULTILINE | ES_AUTOVSCROLL | ES_LEFT);

	// Add the child controls to the splitters.
	m_wndMainSplit.SetPane(CSplitWnd::LEFT_PANE, &m_tvNodeTree);
	m_wndMainSplit.SetSizingBarPos(App.m_nDefSplitPos);
	SetLayout(App.m_eDefLayout);

	// Initialise the controls.
	m_fntControls.Create(CLogFont(TXT("Lucida Console"), -CScreenDC().PointSizeToPixels(10)));

	m_tvNodeTree.Font(m_fntControls);
	m_tvNodeTree.SetImageList(TVSIL_NORMAL, IDB_NODE_ICONS, 16, RGB(255, 255, 255));

	m_lvAttributes.Font(m_fntControls);
	m_lvAttributes.InsertColumn(NAME_COLUMN,  TXT("Name"),  App.m_vecDefColWidths[NAME_COLUMN],  LVCFMT_LEFT);
	m_lvAttributes.InsertColumn(VALUE_COLUMN, TXT("Value"), App.m_vecDefColWidths[VALUE_COLUMN], LVCFMT_LEFT);
	m_lvAttributes.FullRowSelect(true);

	m_ebValue.Font(m_fntControls);
	m_ebValue.ReadOnly(true);

	// Update the menu.
	App.m_oAppWnd.m_oMenu.CheckCmd(ID_VIEW_HORZ, true);

	// Display the current DOM.
	InitialiseView();
}

////////////////////////////////////////////////////////////////////////////////
//! Handle window destruction.

void TheView::OnDestroy()
{
	// Save window settings.
	App.m_nDefSplitPos = m_wndMainSplit.SizingBarPos();
	App.m_vecDefColWidths[NAME_COLUMN]  = m_lvAttributes.ColumnWidth(NAME_COLUMN);
	App.m_vecDefColWidths[VALUE_COLUMN] = m_lvAttributes.ColumnWidth(VALUE_COLUMN);
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

	ASSERT(pNode.get() != nullptr);

	XML::NodeType eType = pNode->type();

	// Has attributes?
	if ( (eType == XML::ELEMENT_NODE) || (eType == XML::PROCESSING_NODE) )
	{
		XML::Attributes vAttribs;

		// Extract the attributes.
		if (eType == XML::ELEMENT_NODE)
		{
			vAttribs = Core::static_ptr_cast<XML::ElementNode>(pNode)->getAttributes();
		}
		else if (eType == XML::PROCESSING_NODE)
		{
			vAttribs = Core::static_ptr_cast<XML::ProcessingNode>(pNode)->getAttributes();
		}
		else
		{
			ASSERT_FALSE();
		}

		// Switch info controls and display attributes.
		m_wndMainSplit.SetPane(CSplitWnd::RIGHT_PANE, &m_lvAttributes);

		m_lvAttributes.DeleteAllItems();

		for (XML::Attributes::const_iterator it = vAttribs.begin(); it != vAttribs.end(); ++it)
		{
			const XML::AttributePtr& pAttribute = *it;

			size_t n = m_lvAttributes.ItemCount();
			m_lvAttributes.InsertItem(n,    pAttribute->name().c_str());
			m_lvAttributes.ItemText  (n, 1, pAttribute->value().c_str());
		}
	}
	// Is content?
	else if ( (eType == XML::TEXT_NODE) || (eType == XML::COMMENT_NODE)
		   || (eType == XML::DOCTYPE_NODE) || (eType == XML::CDATA_NODE) )
	{
		tstring strText;

		// Extract text value.
		if (eType == XML::TEXT_NODE)
		{
			strText = Core::static_ptr_cast<XML::TextNode>(pNode)->text();
		}
		else if (eType == XML::COMMENT_NODE)
		{
			strText = Core::static_ptr_cast<XML::CommentNode>(pNode)->comment();
		}
		else if (eType == XML::DOCTYPE_NODE)
		{
			strText = Core::static_ptr_cast<XML::DocTypeNode>(pNode)->declaration();
		}
		else if (eType == XML::CDATA_NODE)
		{
			strText = Core::static_ptr_cast<XML::CDataNode>(pNode)->text();
		}
		else
		{
			ASSERT_FALSE();
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
	// Load the DOM.
	m_tvNodeTree.Refresh();
	m_tvNodeTree.Select(m_tvNodeTree.Root());

	// Set initial focus.
	Activate();
}
