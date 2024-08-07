// SPDX-License-Identifier: MIT
// SPDX-FileCopyrightText: 2022 Chris Roberts

#include "CommandSelectWindow.h"
#include "CommandListItem.h"
#include "Constants.h"
#include "Settings.h"
#include "RunnerAddOn.h"

#include <Alert.h>
#include <Application.h>
#include <Box.h>
#include <Button.h>
#include <CheckBox.h>
#include <FilePanel.h>
#include <FindDirectory.h>
#include <LayoutBuilder.h>
#include <ListView.h>
#include <Path.h>
#include <ScrollView.h>
#include <StringView.h>


#ifdef HAIKU_ENABLE_I18N
#include <Catalog.h>

#undef B_TRANSLATION_CONTEXT
#define B_TRANSLATION_CONTEXT "CommandSelectWindow"
#else
#define B_TRANSLATE(x) x
#endif


enum {
	kBrowseCommandAction = 'BrCm',
	kBrowseDirectoryAction = 'BrDr',
	kCommandRefReceived = 'CmRr',
	kCommandTextAction = 'CmTx',
	kDirectoryRefReceived = 'DrRr',
	kListSelectAction = 'LsTs',
	kRunCommandAction = 'RuNc'
};


// workaround Haiku bug #13721 where a file is sent if the user double clicks it
class DirRefFilter : public BRefFilter {
public:
	DirRefFilter() {};
	virtual bool Filter(const entry_ref* ref, BNode* node, struct stat_beos* /*st*/, const char* /*filetype*/)
	{
		if (node->IsDirectory())
			return true;

		if (node->IsSymLink()) {
			BEntry entry(ref, true);
			if (entry.InitCheck() == B_OK && entry.IsDirectory())
				return true;
		}

		return false;
	}
};


CommandSelectWindow::CommandSelectWindow(BMessage* message)
	:
	BWindow(BRect(100, 100, 650, 450), "Run command" B_UTF8_ELLIPSIS, B_FLOATING_WINDOW_LOOK, B_NORMAL_WINDOW_FEEL, B_NOT_ZOOMABLE | B_NOT_MINIMIZABLE | B_AUTO_UPDATE_SIZE_LIMITS | B_CLOSE_ON_ESCAPE),
	fCommandFilePanel(NULL),
	fDirectoryFilePanel(NULL),
	fRefsMessage(new BMessage(*message))
{
	fCommandListView = new BListView("CommandList");
	fCommandListView->SetExplicitMinSize(BSize(300, 100));
	fCommandListView->SetSelectionMessage(new BMessage(kListSelectAction));

	BScrollView* scrollView = new BScrollView("CommandScrollView", fCommandListView, B_INVALIDATE_AFTER_LAYOUT, false, true);

	// find our working directory or fall back to /boot/home
	BPath cwdPath;
	entry_ref cwd_ref;
	if (message->FindRef("TrackRunner:cwd", &cwd_ref) == B_OK)
		cwdPath.SetTo(&cwd_ref);
	else
		find_directory(B_USER_DIRECTORY, &cwdPath);

	fDirectoryTextControl = new BTextControl("CWDTextControl", B_TRANSLATE("Folder:"), cwdPath.Path(), NULL);
	fCommandTextControl = new BTextControl("CommandTextControl", B_TRANSLATE("Command:"), "", NULL);
	fCommandTextControl->SetModificationMessage(new BMessage(kCommandTextAction));
	// prevent the text control from growing vertically due to the button
	fCommandTextControl->TextView()->SetExplicitMaxSize(BSize(B_SIZE_UNSET, fCommandTextControl->TextView()->MinSize().Height()));

	fUseTerminalCheckBox = new BCheckBox(B_TRANSLATE("Use Terminal"));
	// allow the checkbox to grow horizontally as the window resizes
	BSize size(fUseTerminalCheckBox->ExplicitMaxSize());
	size.SetWidth(B_SIZE_UNLIMITED);
	fUseTerminalCheckBox->SetExplicitMaxSize(size);

	fRunButton = new BButton(B_TRANSLATE("Run"), new BMessage(kRunCommandAction));
	fRunButton->MakeDefault(true);

	// clang-format off
	BLayoutBuilder::Group<>(this, B_VERTICAL, 0)
		.SetInsets(B_USE_WINDOW_INSETS)
		.Add(scrollView)
		.AddStrut(10.0)
		.AddGrid(B_USE_HALF_ITEM_SPACING, B_USE_HALF_ITEM_SPACING)
			.AddTextControl(fCommandTextControl, 0, 0, B_ALIGN_RIGHT)
			.Add(new BButton(B_TRANSLATE("Browse" B_UTF8_ELLIPSIS), new BMessage(kBrowseCommandAction)), 2, 0)
			.AddTextControl(fDirectoryTextControl, 0, 1, B_ALIGN_RIGHT)
			.Add(new BButton(B_TRANSLATE("Browse" B_UTF8_ELLIPSIS), new BMessage(kBrowseDirectoryAction)), 2, 1)
			.Add(fUseTerminalCheckBox, 1, 2)
		.End()
		.AddGroup(B_HORIZONTAL, B_USE_HALF_ITEM_SPACING, 1.0)
			.AddGlue()
			.Add(new BButton(B_TRANSLATE("Cancel"), new BMessage(B_QUIT_REQUESTED)))
			.Add(fRunButton)
		.End()
	.End();
	// clang-format on

	fDirectoryTextControl->TextView()->MakeEditable(false);

	_LoadCommands();

	_UpdateControls();

	fCommandListView->MakeFocus();
}


CommandSelectWindow::~CommandSelectWindow()
{
	delete fRefsMessage;
}


bool
CommandSelectWindow::QuitRequested()
{
	if (fCommandFilePanel != NULL) {
		delete fCommandFilePanel;
		fCommandFilePanel = NULL;
	}

	if (fDirectoryFilePanel != NULL) {
		delete fDirectoryFilePanel;
		fDirectoryFilePanel = NULL;
	}

	if (be_app->CountWindows() == 1)
		be_app->PostMessage(B_QUIT_REQUESTED);

	return true;
}


void
CommandSelectWindow::MessageReceived(BMessage* message) {
	switch (message->what) {
		case kDirectoryRefReceived:
		{
			entry_ref ref;
			if (message->FindRef("refs", &ref) != B_OK)
				break;

			BPath cwdPath(&ref);
			fDirectoryTextControl->SetText(cwdPath.Path());
			break;
		}
		case kCommandRefReceived:
		{
			entry_ref ref;
			if (message->FindRef("refs", &ref) != B_OK)
				break;

			//TODO deselect listitem?
			BPath cmdPath(&ref);
			fCommandTextControl->SetText(cmdPath.Path());
			break;
		}
		case kCommandTextAction:
			//TODO deselect listitem?
			// disable run button if command is empty
			fRunButton->SetEnabled(strlen(fCommandTextControl->Text()));
			break;
		case kBrowseCommandAction:
			_BrowseCommand();
			break;
		case kBrowseDirectoryAction:
			_BrowseWorkingDirectory();
			break;
		case kListSelectAction:
			_UpdateControls();
			break;
		case kRunCommandAction:
			_Run();
			break;
		default:
			BWindow::MessageReceived(message);
	}
}


void
CommandSelectWindow::_Run()
{
	//TODO create a version of RunCommand() that takes direct arguments
	// build our message object for RunCommand()
	entry_ref cwd_ref;
	BEntry entry(fDirectoryTextControl->Text());
	if (entry.InitCheck() != B_OK)
		return;

	if (entry.GetRef(&cwd_ref) != B_OK)
		return;

	if (fRefsMessage->HasRef("dir_ref"))
		fRefsMessage->ReplaceRef("dir_ref", &cwd_ref);
	else
		fRefsMessage->AddRef("dir_ref", &cwd_ref);

	BMessage itemMessage;
	//TODO need the actual list item name as a title if the command hasn't been modified
	itemMessage.AddString(kEntryNameKey, "TrackRunner");
	itemMessage.AddString(kEntryCommandKey, fCommandTextControl->Text());
	itemMessage.AddBool(kEntryUseTerminalKey, fUseTerminalCheckBox->Value());

	fRefsMessage->AddMessage(kCommandDataKey, &itemMessage);

	if (RunnerAddOn::RunCommand(fRefsMessage) != B_OK) {
		//TODO improve error message
		(new BAlert("RunAlert", B_TRANSLATE("Error running the command!"), B_TRANSLATE("OK"),
			NULL, NULL, B_WIDTH_FROM_LABEL, B_STOP_ALERT))->Go();
	}

	PostMessage(B_QUIT_REQUESTED);
}


status_t
CommandSelectWindow::_LoadCommands()
{
	BMessage message;
	if (Settings::ReadSettings(message) != B_OK)
		return B_ERROR;

	BMessage itemMessage;
	for (int32 index = 0; message.FindMessage(kEntryKey, index, &itemMessage) == B_OK; index++) {
		if (itemMessage.what != 'CMND')
			continue;

		CommandListItem* item = new CommandListItem();
		item->SetText(itemMessage.FindString(kEntryNameKey));
		item->SetCommand(itemMessage.FindString(kEntryCommandKey));
		item->SetUseTerminal(itemMessage.FindBool(kEntryUseTerminalKey));

		fCommandListView->AddItem(item);
	}

	fCommandListView->Select(0);

	return B_OK;
}


void
CommandSelectWindow::_BrowseCommand()
{
	if (fCommandFilePanel == NULL) {
		BMessage message(kCommandRefReceived);
		fCommandFilePanel = new BFilePanel(B_OPEN_PANEL, new BMessenger(this), NULL, B_FILE_NODE, false, &message, NULL, true);
	}

	fCommandFilePanel->Show();
}


void
CommandSelectWindow::_BrowseWorkingDirectory()
{
	if (fDirectoryFilePanel == NULL) {
		BMessage message(kDirectoryRefReceived);
		fDirectoryFilePanel = new BFilePanel(B_OPEN_PANEL, new BMessenger(this), NULL, B_DIRECTORY_NODE, false, &message, new DirRefFilter(), true);
	}

	fDirectoryFilePanel->Show();
}


void
CommandSelectWindow::_UpdateControls()
{
	CommandListItem* item = dynamic_cast<CommandListItem*>(fCommandListView->ItemAt(fCommandListView->CurrentSelection()));
	if (item != NULL) {
		fCommandTextControl->SetText(item->Command());
		fUseTerminalCheckBox->SetValue(item->UseTerminal());
	} else {
		fCommandTextControl->SetText("");
		fUseTerminalCheckBox->SetValue(1);
	}

	// disable run button if command is empty
	fRunButton->SetEnabled(strlen(fCommandTextControl->Text()));
}
