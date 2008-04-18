////////////////////////////////////////////////////////////////////////////////
//! \file   XmlTreeView.cpp
//! \brief  The XmlTreeView class definition.
//! \author Chris Oldwood

#include "Common.hpp"
#include "XmlTreeView.hpp"
#include "TheView.hpp"
#include <XML/Document.hpp>
#include <XML/TextNode.hpp>
#include <XML/ElementNode.hpp>
#include <XML/CommentNode.hpp>
#include <XML/ProcessingNode.hpp>
#include "TheApp.hpp"

////////////////////////////////////////////////////////////////////////////////
//! Default constructor.

XmlTreeView::XmlTreeView(TheView& oView)
	: m_oView(oView)
{
}

////////////////////////////////////////////////////////////////////////////////
//! Destructor.

XmlTreeView::~XmlTreeView()
{
}

////////////////////////////////////////////////////////////////////////////////
//! Refresh the entire document.

void XmlTreeView::Refresh()
{
	// Clear the old DOM.
	m_mapItemNode.clear();
	m_mapNodeItem.clear();
	Clear();

	XML::DocumentPtr pDOM    = m_oView.Document().DOM();
	tstring          strItem = TXT("DOM");

	ASSERT(pDOM.Get() != nullptr);

	HTREEITEM hRoot = InsertRootItem(strItem, pDOM->HasChildren(), 0);

	AddItemNodeMapping(hRoot, pDOM);

	AddNodeTree(hRoot, *pDOM);
}

////////////////////////////////////////////////////////////////////////////////
//! Create a mapping between the item and node.

void XmlTreeView::AddItemNodeMapping(HTREEITEM hItem, const XML::NodePtr& pNode)
{
	ASSERT(m_mapItemNode.find(hItem)       == m_mapItemNode.end());
	ASSERT(m_mapNodeItem.find(pNode.Get()) == m_mapNodeItem.end());

	m_mapItemNode.insert(std::make_pair(hItem, pNode.Get()));
	m_mapNodeItem.insert(std::make_pair(pNode.Get(), hItem));
}

////////////////////////////////////////////////////////////////////////////////
//! Get the XML node for the tree item.

XML::NodePtr XmlTreeView::GetItemNode(HTREEITEM hItem)
{
	ItemNodeMap::const_iterator it = m_mapItemNode.find(hItem);

	ASSERT(it != m_mapItemNode.end());

	return XML::NodePtr(it->second, true);
}

////////////////////////////////////////////////////////////////////////////////
//! Handle child message reflected back from the parent.

void XmlTreeView::OnReflectedCtrlMsg(NMHDR& oMsgHdr)
{
	ASSERT(oMsgHdr.hwndFrom == m_hWnd);

	// Reflect back to the underlying view window.
	if (oMsgHdr.code == TVN_SELCHANGED)
		m_oView.OnNodeSelected(reinterpret_cast<NMTREEVIEW&>(oMsgHdr));
}

////////////////////////////////////////////////////////////////////////////////
//! Add the node container to the tree.

void XmlTreeView::AddNodeTree(HTREEITEM hParent, const XML::NodeContainer& oContainer)
{
	typedef XML::NodeContainer::const_iterator CIter;

	// Add all children to the parent node.
	for (CIter it = oContainer.BeginChild(); it != oContainer.EndChild(); ++it)
	{
		const XML::NodePtr& pNode = *it;

		HTREEITEM hItem = AddNode(hParent, pNode);

		// If a container node, recursively add it's sub-tree.
		if (pNode->Type() == XML::DOCUMENT_NODE)
			AddNodeTree(hItem, *Core::static_ptr_cast<XML::Document>(pNode));
		else if (pNode->Type() == XML::ELEMENT_NODE)
			AddNodeTree(hItem, *Core::static_ptr_cast<XML::ElementNode>(pNode));
	}
}

////////////////////////////////////////////////////////////////////////////////
//! Add a node to the tree.

HTREEITEM XmlTreeView::AddNode(HTREEITEM hParent, const XML::NodePtr& pNode)
{
	// Add it to the tree view.
	HTREEITEM hItem = InsertItem(hParent, TVI_LAST, TXT(""));

	AddItemNodeMapping(hItem, pNode);

	UpdateNode(hItem, pNode);

	return hItem;
}

////////////////////////////////////////////////////////////////////////////////
//! Update a node in the tree.

void XmlTreeView::UpdateNode(HTREEITEM hItem, const XML::NodePtr& pNode)
{
	XML::NodeType eType        = pNode->Type();
	tstring       strItem      = pNode->TypeStr();
	bool          bHasChildren = false;
	int           nImage       = -1;

	// Create a summary for the tree item.
	if (eType == XML::DOCUMENT_NODE)
	{
		XML::DocumentPtr pDoc = Core::static_ptr_cast<XML::Document>(pNode);

		bHasChildren = pDoc->HasChildren();
		nImage       = 0;
	}
	else if (eType == XML::ELEMENT_NODE)
	{
		XML::ElementNodePtr pElement = Core::static_ptr_cast<XML::ElementNode>(pNode);

		strItem      = pElement->Name();
		strItem     += TXT(' ');
		strItem     += MakeAttribSummary(pElement->GetAttributes());
		bHasChildren = pElement->HasChildren();
		nImage       = 1;

		PostProcessSummary(strItem);
	}
	else if (eType == XML::TEXT_NODE)
	{
		XML::TextNodePtr pText = Core::static_ptr_cast<XML::TextNode>(pNode);

		strItem = pText->Text();
		nImage  = 6;

		PostProcessSummary(strItem);
	}
	else if (eType == XML::COMMENT_NODE)
	{
		nImage = 5;
	}
	else if (eType == XML::PROCESSING_NODE)
	{
		XML::ProcessingNodePtr pProcInst = Core::static_ptr_cast<XML::ProcessingNode>(pNode);

		strItem  = pProcInst->Target();
		strItem += TXT(' ');
		strItem += MakeAttribSummary(pProcInst->GetAttributes());
		nImage   = 4;

		PostProcessSummary(strItem);
	}
	else if (eType == XML::DOCTYPE_NODE)
	{
		strItem = TXT("DOCTYPE");
		nImage  = 7;
	}
	else
	{
		ASSERT_FALSE();
	}

	// Add it to the tree view.
	UpdateItem(hItem, strItem, bHasChildren, nImage);
}

////////////////////////////////////////////////////////////////////////////////
//! Generate a summary of the attributes.

tstring XmlTreeView::MakeAttribSummary(XML::Attributes& vAttribs)
{
	// Type aliases.
	typedef XML::Attributes::const_iterator ConstIter;

	tstring str;

	// For all attributes...
	for (ConstIter it = vAttribs.Begin(); it != vAttribs.End(); ++it)
	{
		const XML::AttributePtr& pAttrib = *it;

		if (!str.empty())
			str += TXT(' ');

		str += pAttrib->Name();
		str += TXT("=\"");
		str += pAttrib->Value();
		str += TXT("\"");
	}

	return str;
}

////////////////////////////////////////////////////////////////////////////////
//! Post-process the summary.

void XmlTreeView::PostProcessSummary(tstring& str)
{
	// Replace empty strings.
	if (str.empty())
	{
		str = TXT("(empty)");
		return;
	}

	bool bWhitespaceOnly = true;

	// Find if only whitespace characters.
	for (tstring::const_iterator it = str.begin(); ((it != str.end()) && bWhitespaceOnly); ++it)
	{
		if (!isspace(static_cast<uint>(*it)))
			bWhitespaceOnly = false;
	}

	// Replace "invisible" strings.
	if (bWhitespaceOnly)
	{
		str = TXT("(whitespace)");
		return;
	}

	// Trim string.
	if (str.length() > App.m_nMaxSummaryLen)
	{
		str.erase(App.m_nMaxSummaryLen, str.length()-App.m_nMaxSummaryLen);

		str += TXT("...");
	}
}
