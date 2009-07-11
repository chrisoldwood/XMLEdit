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
#include "TheDoc.hpp"

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
//! Get the current selected node.

XML::NodePtr XmlTreeView::Selection() const
{
	HTREEITEM hSelItem = TreeView::Selection();

	XML::NodePtr pNode;

	if (hSelItem != NULL)
		pNode = GetItemNode(hSelItem);

	return pNode;
}

////////////////////////////////////////////////////////////////////////////////
//! Set the selected node.

void XmlTreeView::SetSelection(const XML::NodePtr& pNode)
{
	HTREEITEM hItem = GetNodeItem(pNode);

	Select(hItem);
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

	ASSERT(pDOM.get() != nullptr);

	HTREEITEM hRoot = InsertRootItem(strItem, pDOM->hasChildren(), 0);

	AddItemNodeMapping(hRoot, pDOM);

	AddNodeTree(hRoot, *pDOM);
}

////////////////////////////////////////////////////////////////////////////////
//! Create a mapping between the item and node.

void XmlTreeView::AddItemNodeMapping(HTREEITEM hItem, const XML::NodePtr& pNode)
{
	ASSERT(m_mapItemNode.find(hItem)       == m_mapItemNode.end());
	ASSERT(m_mapNodeItem.find(pNode.get()) == m_mapNodeItem.end());

	m_mapItemNode.insert(std::make_pair(hItem, pNode.get()));
	m_mapNodeItem.insert(std::make_pair(pNode.get(), hItem));
}

////////////////////////////////////////////////////////////////////////////////
//! Get the XML node for the tree item.

XML::NodePtr XmlTreeView::GetItemNode(HTREEITEM hItem) const
{
	ItemNodeMap::const_iterator it = m_mapItemNode.find(hItem);

	ASSERT(it != m_mapItemNode.end());

	return XML::NodePtr(it->second, true);
}

////////////////////////////////////////////////////////////////////////////////
//! Get the tree item for the XML node.

HTREEITEM XmlTreeView::GetNodeItem(const XML::NodePtr& pNode) const
{
	NodeItemMap::const_iterator it = m_mapNodeItem.find(pNode.get());

	ASSERT(it != m_mapNodeItem.end());

	return it->second;
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
	for (CIter it = oContainer.beginChild(); it != oContainer.endChild(); ++it)
	{
		const XML::NodePtr& pNode = *it;

		HTREEITEM hItem = AddNode(hParent, pNode);

		// If a container node, recursively add it's sub-tree.
		if (pNode->type() == XML::DOCUMENT_NODE)
			AddNodeTree(hItem, *Core::static_ptr_cast<XML::Document>(pNode));
		else if (pNode->type() == XML::ELEMENT_NODE)
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
	XML::NodeType eType        = pNode->type();
	tstring       strItem      = pNode->typeStr();
	bool          bHasChildren = false;
	int           nImage       = -1;

	// Create a summary for the tree item.
	if (eType == XML::DOCUMENT_NODE)
	{
		XML::DocumentPtr pDoc = Core::static_ptr_cast<XML::Document>(pNode);

		bHasChildren = pDoc->hasChildren();
		nImage       = 0;
	}
	else if (eType == XML::ELEMENT_NODE)
	{
		XML::ElementNodePtr pElement = Core::static_ptr_cast<XML::ElementNode>(pNode);

		strItem      = pElement->name();
		strItem     += TXT(' ');
		strItem     += MakeAttribSummary(pElement->getAttributes());
		bHasChildren = pElement->hasChildren();
		nImage       = 1;

		PostProcessSummary(strItem);
	}
	else if (eType == XML::TEXT_NODE)
	{
		XML::TextNodePtr pText = Core::static_ptr_cast<XML::TextNode>(pNode);

		strItem = pText->text();
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

		strItem  = pProcInst->target();
		strItem += TXT(' ');
		strItem += MakeAttribSummary(pProcInst->getAttributes());
		nImage   = 4;

		PostProcessSummary(strItem);
	}
	else if (eType == XML::DOCTYPE_NODE)
	{
		strItem = TXT("DOCTYPE");
		nImage  = 7;
	}
	else if (eType == XML::CDATA_NODE)
	{
		strItem = TXT("CDATA");
		nImage  = 8;
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
	for (ConstIter it = vAttribs.begin(); it != vAttribs.end(); ++it)
	{
		const XML::AttributePtr& pAttrib = *it;

		if (!str.empty())
			str += TXT(' ');

		str += pAttrib->name();
		str += TXT("=\"");
		str += pAttrib->value();
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
		if (!tisspace(static_cast<utchar>(*it)))
			bWhitespaceOnly = false;
	}

	// Replace "invisible" strings.
	if (bWhitespaceOnly)
	{
		str = TXT("(whitespace)");
		return;
	}

	// Trim string.
	if (str.length() > App.m_nDefMaxItemLen)
	{
		str.erase(App.m_nDefMaxItemLen, str.length()-App.m_nDefMaxItemLen);

		str += TXT("...");
	}
}
