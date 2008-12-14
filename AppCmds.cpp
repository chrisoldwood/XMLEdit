////////////////////////////////////////////////////////////////////////////////
//! \file   AppCmds.cpp
//! \brief  The AppCmds class definition.
//! \author Chris Oldwood

#include "Common.hpp"
#include "AppCmds.hpp"
#include "TheApp.hpp"
#include "TheDoc.hpp"
#include "TheView.hpp"
#include "AboutDlg.hpp"
#include "FindDlg.hpp"
#include "ShowPathDlg.hpp"
#include <XML/XPathIterator.hpp>

//! The ID of the first MRU command.
const int ID_MRU_FIRST = ID_FILE_MRU_1;

//! The ID of the last MRU command.
const int ID_MRU_LAST = ID_FILE_MRU_4;

////////////////////////////////////////////////////////////////////////////////
//! Constructor.

AppCmds::AppCmds()
{
	// Define the command table.
	DEFINE_CMD_TABLE
		// File menu.
		CMD_ENTRY(ID_FILE_NEW,					&AppCmds::OnFileNew,		&AppCmds::OnUIFileNew,		 0)
		CMD_ENTRY(ID_FILE_OPEN,					&AppCmds::OnFileOpen,		NULL,						 1)
		CMD_ENTRY(ID_FILE_SAVE,					&AppCmds::OnFileSave,		&AppCmds::OnUIFileSave,		 2)
		CMD_ENTRY(ID_FILE_SAVEAS,				&AppCmds::OnFileSaveAs,		&AppCmds::OnUIFileSaveAs,	-1)
		CMD_ENTRY(ID_FILE_CLOSE,				&AppCmds::OnFileClose,		&AppCmds::OnUIFileClose,	 1)
		CMD_RANGE(ID_MRU_FIRST,	ID_MRU_LAST,	&AppCmds::OnFileOpenMRU,	&AppCmds::OnUIFileOpenMRU,	-1)
		CMD_ENTRY(ID_FILE_EXIT,					&AppCmds::OnFileExit,		NULL,						-1)
		// Edit menu.
		CMD_ENTRY(ID_EDIT_FIND,					&AppCmds::OnEditFind,		&AppCmds::OnUIEditFind,		-1)
		CMD_ENTRY(ID_EDIT_FIND_NEXT,			&AppCmds::OnEditFindNext,	&AppCmds::OnUIEditFindNext,	-1)
		// View menu.
		CMD_ENTRY(ID_VIEW_HORZ,					&AppCmds::OnViewHorz,		&AppCmds::OnUIViewHorz,		-1)
		CMD_ENTRY(ID_VIEW_VERT,					&AppCmds::OnViewVert,		&AppCmds::OnUIViewVert,		-1)
		CMD_ENTRY(ID_VIEW_NODE_PATH,			&AppCmds::OnViewNodePath,	&AppCmds::OnUIViewNodePath,	-1)
		// Help menu.
		CMD_ENTRY(ID_HELP_ABOUT,				&AppCmds::OnHelpAbout,		NULL,						10)
	END_CMD_TABLE
}

////////////////////////////////////////////////////////////////////////////////
//! Destructor.

AppCmds::~AppCmds()
{
}

////////////////////////////////////////////////////////////////////////////////
//! Create a new document.

void AppCmds::OnFileNew()
{
	NewFile();
}

////////////////////////////////////////////////////////////////////////////////
//! Open an existing document.

void AppCmds::OnFileOpen()
{
	OpenFile();
}

////////////////////////////////////////////////////////////////////////////////
//! Save the current document.

void AppCmds::OnFileSave()
{
	SaveFile();
}

////////////////////////////////////////////////////////////////////////////////
//! Save the current document with another filename.

void AppCmds::OnFileSaveAs()
{
	SaveFileAs();
}

////////////////////////////////////////////////////////////////////////////////
//! Close the current document.

void AppCmds::OnFileClose()
{
	CloseFile();
}

////////////////////////////////////////////////////////////////////////////////
//! Open one of the previous documents.

void AppCmds::OnFileOpenMRU(uint iCmdID)
{
	OpenMRUFile(iCmdID - ID_MRU_FIRST);
}

////////////////////////////////////////////////////////////////////////////////
//! Close the application.

void AppCmds::OnFileExit()
{
	App.m_oAppWnd.Close();
}

////////////////////////////////////////////////////////////////////////////////
//! Find the first node that matches an XPath expression.

void AppCmds::OnEditFind()
{
	ASSERT(App.Document() != nullptr);

	FindDlg dlgFind;

	dlgFind.m_strQuery = App.m_strLastSearch;

	// Query user for the expression.
	if (dlgFind.RunModal(App.m_oAppWnd) == IDOK)
	{
		App.m_lstQueryNodes.clear();

		try
		{
			// Evaluate the expression.
			XML::XPathIterator it(dlgFind.m_strQuery, App.Document()->DOM());
			XML::XPathIterator end;

			for (; it != end; ++it)
				App.m_lstQueryNodes.push_back(*it);

			// Remember valid queries.
			App.m_strLastSearch = dlgFind.m_strQuery;
		}
		catch (const Core::Exception& e)
		{
			App.FatalMsg(TXT("Failed to evaluate the XPath expression:-\n\n%s"), e.What());
			return;
		}

		// No results?
		if (App.m_lstQueryNodes.empty())
		{
			App.NotifyMsg(TXT("The query did not match any nodes"));
			return;
		}

		// Display the first node.
		OnEditFindNext();
	}
}

////////////////////////////////////////////////////////////////////////////////
//! Find the next node that matches the previous expression. We treat the result
//! set as a cyclic buffer.

void AppCmds::OnEditFindNext()
{
	if (!App.m_lstQueryNodes.empty())
	{
		// Get head and move to tail.
		XML::NodePtr pNode = App.m_lstQueryNodes.front();

		App.m_lstQueryNodes.pop_front();
		App.m_lstQueryNodes.push_back(pNode);

		// Display it.
		App.Document()->View()->SetSelection(pNode);
	}
}

////////////////////////////////////////////////////////////////////////////////
//! Change the layout to the horizontal one.

void AppCmds::OnViewHorz()
{
	ASSERT(App.m_pView != nullptr);

	App.m_eDefLayout = TheView::HORIZONTAL;
	App.Document()->View()->SetLayout(App.m_eDefLayout);

	UpdateUI();
}

////////////////////////////////////////////////////////////////////////////////
//! Change the layout to the vertical one.

void AppCmds::OnViewVert()
{
	ASSERT(App.m_pView != nullptr);

	App.m_eDefLayout = TheView::VERTICAL;
	App.Document()->View()->SetLayout(App.m_eDefLayout);

	UpdateUI();
}

////////////////////////////////////////////////////////////////////////////////
//! Show the full path to the select node.

void AppCmds::OnViewNodePath()
{
	tstring strPath;

	// Derive the simple path.
	XML::NodePtr pNode = App.Document()->View()->Selection();

	while (pNode.Get() != nullptr)
	{
		if (pNode->Type() == XML::ELEMENT_NODE)
		{
			XML::ElementNodePtr pElemNode = Core::static_ptr_cast<XML::ElementNode>(pNode);

			strPath = TXT("/") + pElemNode->Name() + strPath;
		}

		pNode = pNode->Parent();
	}

	// Display it.
	ShowPathDlg dlgPath;

	dlgPath.m_strPath = strPath;

	dlgPath.RunModal(App.m_oAppWnd);
}

////////////////////////////////////////////////////////////////////////////////
//! Show the about dialog.

void AppCmds::OnHelpAbout()
{
	AboutDlg Dlg;

	Dlg.RunModal(App.m_oAppWnd);
}

////////////////////////////////////////////////////////////////////////////////
//! Update the command UI.

void AppCmds::OnUIFileNew()
{
	App.m_oAppWnd.m_oMenu.EnableCmd(ID_FILE_NEW, false);
	App.m_oAppWnd.m_oToolbar.m_btnNew.Enable(false);
}

////////////////////////////////////////////////////////////////////////////////
//! Update the command UI.

void AppCmds::OnUIFileSave()
{
//	bool bDocOpen  = (App.m_pDoc != nullptr);
//	bool bModified = (bDocOpen && App.m_pDoc->Modified());

//	App.m_oAppWnd.m_oMenu.EnableCmd(ID_FILE_SAVE, (bDocOpen && bModified));
//	App.m_oAppWnd.m_oToolbar.m_btnSave.Enable(bDocOpen && bModified);
	App.m_oAppWnd.m_oMenu.EnableCmd(ID_FILE_SAVE, false);
	App.m_oAppWnd.m_oToolbar.m_btnSave.Enable(false);
}

////////////////////////////////////////////////////////////////////////////////
//! Update the command UI.

void AppCmds::OnUIFileSaveAs()
{
//	bool bDocOpen = (App.m_pDoc != nullptr);

//	App.m_oAppWnd.m_oMenu.EnableCmd(ID_FILE_SAVEAS, bDocOpen);
	App.m_oAppWnd.m_oMenu.EnableCmd(ID_FILE_SAVEAS, false);
}

////////////////////////////////////////////////////////////////////////////////
//! Update the command UI.

void AppCmds::OnUIFileClose()
{
	bool bDocOpen = (App.m_pDoc != nullptr);

	App.m_oAppWnd.m_oMenu.EnableCmd(ID_FILE_CLOSE, bDocOpen);
}

////////////////////////////////////////////////////////////////////////////////
//! Update the command UI.

void AppCmds::OnUIFileOpenMRU()
{
	App.m_MRUList.UpdateMenu(App.m_oAppWnd.m_oMenu, ID_MRU_FIRST);
}

////////////////////////////////////////////////////////////////////////////////
//! Update the command UI.

void AppCmds::OnUIEditFind()
{
	bool bDocOpen = (App.m_pDoc != nullptr);

	App.m_oAppWnd.m_oMenu.EnableCmd(ID_EDIT_FIND, bDocOpen);
}

////////////////////////////////////////////////////////////////////////////////
//! Update the command UI.

void AppCmds::OnUIEditFindNext()
{
	bool bDocOpen = (App.m_pDoc != nullptr);

	App.m_oAppWnd.m_oMenu.EnableCmd(ID_EDIT_FIND_NEXT, bDocOpen);
}

////////////////////////////////////////////////////////////////////////////////
//! Update the command UI.

void AppCmds::OnUIViewHorz()
{
	bool docOpen  = (App.m_pDoc != nullptr);
	bool isLayout = (App.m_eDefLayout == TheView::HORIZONTAL);

	App.m_oAppWnd.m_oMenu.EnableCmd(ID_VIEW_HORZ, docOpen);
	App.m_oAppWnd.m_oMenu.CheckCmd(ID_VIEW_HORZ, docOpen && isLayout);
}

////////////////////////////////////////////////////////////////////////////////
//! Update the command UI.

void AppCmds::OnUIViewVert()
{
	bool docOpen  = (App.m_pDoc != nullptr);
	bool isLayout = (App.m_eDefLayout == TheView::VERTICAL);

	App.m_oAppWnd.m_oMenu.EnableCmd(ID_VIEW_VERT, docOpen);
	App.m_oAppWnd.m_oMenu.CheckCmd(ID_VIEW_VERT, docOpen && isLayout);
}

////////////////////////////////////////////////////////////////////////////////
//! Update the command UI.

void AppCmds::OnUIViewNodePath()
{
	bool bDocOpen = (App.m_pDoc != nullptr);

	App.m_oAppWnd.m_oMenu.EnableCmd(ID_VIEW_NODE_PATH, bDocOpen);
}
