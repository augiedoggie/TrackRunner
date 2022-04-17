// SPDX-License-Identifier: MIT
// SPDX-FileCopyrightText: 2022 Chris Roberts

#include "CommandsWindow.h"
#include "CommandListItem.h"
#include "Constants.h"
#include "Preferences.h"
#include "SortableListView.h"

#include <Alert.h>
#include <Application.h>
#include <Bitmap.h>
#include <Button.h>
#include <CheckBox.h>
#include <FilePanel.h>
#include <IconUtils.h>
#include <LayoutBuilder.h>
#include <ListView.h>
#include <Path.h>
#include <Resources.h>
#include <ScrollView.h>
#include <StringView.h>
#include <private/shared/ToolBar.h>


enum {
	kDeleteCommandAction = 'DeLc',
	kModificationAction = 'MoDa',
	kNewCommandAction = 'NeWc',
	kRevertAction = 'RvRt',
	kUserGuideAction = 'GiDe',
	kListSelectAction = 'LsTs',
	kBrowseCommandAction = 'BrWz',
	kListUpdateAction = 'LsUp'
};


CommandsWindow::CommandsWindow(BString& title)
	:
	BWindow(BRect(100, 100, 700, 450), title, B_TITLED_WINDOW,
			B_ASYNCHRONOUS_CONTROLS | B_AUTO_UPDATE_SIZE_LIMITS | B_CLOSE_ON_ESCAPE),
	fBrowsePanel(NULL)
{
	fListView = new SortableListView("CommandList", kListUpdateAction);
	fListView->SetSelectionMessage(new BMessage(kListSelectAction));

	BScrollView* scrollView = new BScrollView("CommandScrollView", fListView, B_INVALIDATE_AFTER_LAYOUT, false, true);
	scrollView->SetExplicitMinSize(BSize(150, 300));
	scrollView->SetExplicitMaxSize(BSize(250, B_SIZE_UNSET));

	fToolbar = new BToolBar();
	fToolbar->AddAction(kNewCommandAction, this, _ResourceBitmap("EntryAdd", 16, 16), "Create a new command entry", "New");
	fToolbar->AddAction(kDeleteCommandAction, this, _ResourceBitmap("EntryDelete", 16, 16), "Delete the selected command entry", "Delete");
	fToolbar->AddGlue();
	fToolbar->AddAction(kUserGuideAction, this, _ResourceBitmap("UserGuide", 16, 16), "Open the user guide", "User Guide");

	fNameControl = new BTextControl("Name:", NULL, NULL);
	fNameControl->SetModificationMessage(new BMessage(kModificationAction));

	fCommandControl = new BTextControl("Command:", NULL, NULL);
	fCommandControl->SetModificationMessage(new BMessage(kModificationAction));

	fTerminalCheckBox = new BCheckBox("Use Terminal", new BMessage(kModificationAction));
	BSize size(fTerminalCheckBox->ExplicitMaxSize());
	size.SetWidth(B_SIZE_UNLIMITED);
	fTerminalCheckBox->SetExplicitMaxSize(size);

	// clang-format off
	BLayoutBuilder::Group<>(this, B_VERTICAL, 0)
		.SetInsets(B_USE_HALF_ITEM_INSETS, 0, B_USE_HALF_ITEM_INSETS, B_USE_HALF_ITEM_INSETS)
		.Add(fToolbar)
		.AddGroup(B_HORIZONTAL)
			.Add(scrollView, 0.5)
			.AddGroup(B_VERTICAL)
				.AddStrut(10.0)
				.AddGrid(B_USE_HALF_ITEM_SPACING, B_USE_HALF_ITEM_SPACING)
					.AddTextControl(fNameControl, 0, 0, B_ALIGN_RIGHT)
					.AddTextControl(fCommandControl, 0, 1, B_ALIGN_RIGHT)
					.AddGroup(B_HORIZONTAL, 0, 1, 2)
						.SetInsets(0)
						.Add(fBrowseButton = new BButton("Browse" B_UTF8_ELLIPSIS, new BMessage(kBrowseCommandAction)))
						.AddGlue()
					.End()
					.Add(fTerminalCheckBox, 1, 3)
				.End()
				.AddGlue()
			.End()
		.End()
	.End();
	// clang-format on

	_LoadCommands();

	_InitControls();
}


bool
CommandsWindow::QuitRequested()
{
	if (fBrowsePanel != NULL) {
		delete fBrowsePanel;
		fBrowsePanel = NULL;
	}

	if (be_app->CountWindows() == 1)
		be_app->PostMessage(B_QUIT_REQUESTED);

	return true;
}


void
CommandsWindow::MessageReceived(BMessage* message)
{
	switch (message->what) {
		case B_REFS_RECEIVED:
			_RefsReceived(message);
			break;
		case kBrowseCommandAction:
			_BrowseCommand();
			break;
		case kDeleteCommandAction:
			_DeleteCommand();
			break;
		case kListSelectAction:
			_SelectItem();
			break;
		case kListUpdateAction:
			_SaveCommands();
			break;
		case kModificationAction:
			_UpdateItem();
			break;
		case kNewCommandAction:
			_InitNewCommand();
			break;
		case kUserGuideAction:
			//TODO open user guide
			(new BAlert("ErrorAlert", "User guide not written yet!", "OK", NULL, NULL, B_WIDTH_FROM_LABEL, B_STOP_ALERT))->Go();
			break;
		default:
			BWindow::MessageReceived(message);
	}
}


void
CommandsWindow::_RefsReceived(BMessage* message)
{
	entry_ref ref;
	if (message->FindRef("refs", &ref) != B_OK)
		return; //TODO show error alert?

	BPath path(&ref);
	if (path.InitCheck() != B_OK)
		return;

	BString escapeString(path.Path());
	//escape spaces in the path
	escapeString.ReplaceAll(" ", "\\ ");
	fCommandControl->SetText(escapeString);
}


void
CommandsWindow::_BrowseCommand()
{
	if (fBrowsePanel == NULL)
		fBrowsePanel = new BFilePanel(B_OPEN_PANEL, new BMessenger(this), NULL, B_FILE_NODE, false, NULL, NULL, true);

	fBrowsePanel->Show();
}


void
CommandsWindow::_InitNewCommand()
{
	CommandListItem* item = new CommandListItem();
	fListView->AddItem(item);
	fListView->Select(fListView->IndexOf(item));
	fListView->ScrollToSelection();

	_SaveCommands();
}


void
CommandsWindow::_DeleteCommand()
{
	int32 selection = fListView->CurrentSelection();

	CommandListItem* item = dynamic_cast<CommandListItem*>(fListView->ItemAt(selection));
	if (item == NULL)
		return;

	BString alertString;
	alertString.SetToFormat("Do you wish to delete %s ?", item->Text());
	if ((new BAlert("DeleteAlert", alertString, "Delete", "Cancel", NULL, B_WIDTH_FROM_LABEL, B_INFO_ALERT))->Go() == 1)
		return;

	fListView->RemoveItem(item);
	delete item;

	_SaveCommands();

	if (selection >= fListView->CountItems())
		selection--;

	fListView->Select(selection);
}


void
CommandsWindow::_SelectItem()
{
	CommandListItem* item = dynamic_cast<CommandListItem*>(fListView->ItemAt(fListView->CurrentSelection()));
	if (item == NULL) {
		// no selection
		_InitControls();
		return;
	}

	fToolbar->SetActionEnabled(kDeleteCommandAction, true);

	fNameControl->SetText(item->Text());
	fNameControl->SetEnabled(true);

	fCommandControl->SetText(item->Command());
	fCommandControl->SetEnabled(true);

	fBrowseButton->SetEnabled(true);

	fTerminalCheckBox->SetValue(item->UseTerminal());
	fTerminalCheckBox->SetEnabled(true);
}


void
CommandsWindow::_UpdateItem()
{
	CommandListItem* item = dynamic_cast<CommandListItem*>(fListView->ItemAt(fListView->CurrentSelection()));
	if (item == NULL)
		return;

	item->SetCommand(fCommandControl->Text());

	item->SetUseTerminal(fTerminalCheckBox->Value() != 0);

	if (strcmp(item->Text(), fNameControl->Text()) != 0) {
		item->SetText(fNameControl->Text());
		fListView->InvalidateItem(fListView->CurrentSelection());
	}

	_SaveCommands();
}


status_t
CommandsWindow::_LoadCommands()
{
	BMessage message;
	if (Preferences::ReadPreferences(message) != B_OK)
		return B_ERROR;

	BMessage itemMessage;
	for (int32 index = 0; message.FindMessage(kEntryKey, index, &itemMessage) == B_OK; index++) {
		if (itemMessage.what != 'CMND')
			continue;

		CommandListItem* item = new CommandListItem();
		item->SetText(itemMessage.FindString(kEntryNameKey));
		item->SetCommand(itemMessage.FindString(kEntryCommandKey));
		item->SetUseTerminal(itemMessage.FindBool(kEntryUseTerminalKey));

		fListView->AddItem(item);
	}

	return B_OK;
}


status_t
CommandsWindow::_SaveCommands()
{
	BMessage message;
	if (Preferences::ReadPreferences(message) != B_OK)
		return B_ERROR;

	// clear out old command entries
	message.RemoveName(kEntryKey);

	for (int32 index = 0; index < fListView->CountItems(); index++) {
		CommandListItem* item = dynamic_cast<CommandListItem*>(fListView->ItemAt(index));
		if (item == NULL)
			continue;

		BMessage itemMessage('CMND');
		itemMessage.AddString(kEntryNameKey, item->Text());
		itemMessage.AddString(kEntryCommandKey, item->Command());
		itemMessage.AddBool(kEntryUseTerminalKey, item->UseTerminal());

		message.AddMessage(kEntryKey, &itemMessage);
	}

	return Preferences::WritePreferences(message);
}


void
CommandsWindow::_InitControls()
{
	fToolbar->SetActionEnabled(kDeleteCommandAction, false);

	fNameControl->SetText("");
	fNameControl->SetEnabled(false);

	fCommandControl->SetText("");
	fCommandControl->SetEnabled(false);

	fBrowseButton->SetEnabled(false);

	fTerminalCheckBox->SetValue(1);
	fTerminalCheckBox->SetEnabled(false);
}


BBitmap*
CommandsWindow::_ResourceBitmap(const char* name, float width, float height)
{
	BResources* resources = be_app->AppResources();
	if (resources == NULL)
		return NULL;

	size_t size;
	const void* data = resources->LoadResource(B_VECTOR_ICON_TYPE, name, &size);
	if (data == NULL)
		return NULL;

	BBitmap* bitmap = new BBitmap(BRect(0, 0, width - 1, height - 1), B_RGBA32);
	if (bitmap == NULL)
		return NULL;

	if (BIconUtils::GetVectorIcon((const uint8*)data, size, bitmap) != B_OK) {
		delete bitmap;
		return NULL;
	}

	return bitmap;
}
