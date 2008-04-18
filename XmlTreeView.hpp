////////////////////////////////////////////////////////////////////////////////
//! \file   XmlTreeView.hpp
//! \brief  The XmlTreeView class declaration.
//! \author Chris Oldwood

// Check for previous inclusion
#ifndef XMLTREEVIEW_HPP
#define XMLTREEVIEW_HPP

#if _MSC_VER > 1000
#pragma once
#endif

#include <WCL/TreeView.hpp>
#include <XML/Node.hpp>
#include <XML/Attributes.hpp>
#include <map>

// Forward declarations.
class TheView;

////////////////////////////////////////////////////////////////////////////////
//! The tree view derived control used to display the DOM.

class XmlTreeView : public WCL::TreeView
{
public:
	//! Default constructor.
	XmlTreeView(TheView& oView);

	//! Destructor.
	virtual	~XmlTreeView();

	//
	// Methods.
	//

	//! Refresh the entire document.
	void Refresh();

	//! Get the XML node for the tree item.
	XML::NodePtr GetItemNode(HTREEITEM hItem); // throw()

private:
	//! A map of tree item to node ptr.
	typedef std::map<HTREEITEM, XML::Node*> ItemNodeMap;
	//! A map of node ptr to tree item.
	typedef std::map<XML::Node*, HTREEITEM> NodeItemMap;

	//
	// Members.
	//
	TheView&	m_oView;		//!< The document view.
	ItemNodeMap	m_mapItemNode;	//!< The map of tree item to XML node.
	NodeItemMap	m_mapNodeItem;	//!< The map of xml node to tree item.

	//
	// Message handlers.
	//

	//! Handle child message reflected back from the parent.
	virtual void OnReflectedCtrlMsg(NMHDR& oMsgHdr);

	//
	// Internal methods.
	//

	//! Create a mapping between the item and node.
	void AddItemNodeMapping(HTREEITEM hItem, const XML::NodePtr& pNode);

	//! Add the node container to the tree.
	void AddNodeTree(HTREEITEM hParent, const XML::NodeContainer& oContainer);

	//! Add a node to the tree.
	HTREEITEM AddNode(HTREEITEM hParent, const XML::NodePtr& pNode);

	//! Update a node in the tree.
	void UpdateNode(HTREEITEM hItem, const XML::NodePtr& pNode);

	//! Generate a summary of the attributes.
	static tstring MakeAttribSummary(XML::Attributes& vAttribs);

	//! Post-process the summary.
	static void PostProcessSummary(tstring& str);
};

#endif // XMLTREEVIEW_HPP
