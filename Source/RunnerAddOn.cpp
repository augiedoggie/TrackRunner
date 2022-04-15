// SPDX-License-Identifier: MIT
// SPDX-FileCopyrightText: 2022 Chris Roberts

#include "Constants.h"
#include "Preferences.h"

#include <Alert.h>
#include <InterfaceDefs.h>
#include <LayoutBuilder.h>
#include <Menu.h>
#include <Mime.h>
#include <NodeInfo.h>
#include <Path.h>
#include <Roster.h>
#include <TrackerAddOn.h>
#include <private/shared/CommandPipe.h>
#include <private/tracker/IconMenuItem.h>


enum {
	kCommandWhat = 'CMND',
	kUserGuideWhat = 'GUID',
	kManageCommandsWhat = 'MCMD',
	kGithubWhat = 'GTHB',
	kSuperMenuWhat = 'SUPR'
};

const char* kCommandDataKey = "command:data";


status_t
_RunCommand(BMessage* message)
{
	BMessage itemMessage;
	if (message->FindMessage(kCommandDataKey, &itemMessage) != B_OK)
		return B_ERROR;

	BString commandString;
	if (itemMessage.FindString(kEntryCommandKey, &commandString) != B_OK)
		return B_ERROR;

	// for now we assume that an empty command is intentional
	if (commandString.Length() == 0)
		return B_OK;

	BString nameString;
	if (itemMessage.FindString(kEntryNameKey, &nameString) != B_OK)
		return B_ERROR;

	entry_ref cwd_ref;
	if (message->FindRef("dir_ref", &cwd_ref) != B_OK)
		return B_ERROR;

	BPath cwdPath(&cwd_ref);
	if (cwdPath.InitCheck() != B_OK)
		return B_ERROR;

	// add selected files as command arguments
	entry_ref ref;
	for (int32 index = 0; message->FindRef("refs", index, &ref) == B_OK; index++) {
		BPath path(&ref);
		BString pathString(path.Path());
		// escape any single quotes
		pathString.ReplaceAll("'", "'\\''");
		commandString << " '" << pathString << "'";
	}

	if (itemMessage.GetBool(kEntryUseTerminalKey)) {
		//TODO make the read command optional but enabled by default
		commandString << "\n echo \"<<< Finished with status: $? >>>\"; read -p '<<<  Press ENTER to close!  >>>'";

		// give our Terminal a nice title
		BString titleString(nameString);
		titleString << " : " << cwdPath.Path();

		const char* argv[] = { "-w", cwdPath.Path(), "-t", titleString.String(), "/bin/sh", "-c", commandString, NULL };
		be_roster->Launch(kTerminalSignature, 7, argv);
	} else {
		BString cd;
		BString pathString(cwdPath.Path());
		// escape any single quotes
		pathString.ReplaceAll("'", "'\\''");
		cd.SetToFormat("cd '%s' && ", pathString.String());
		commandString.Prepend(cd);

		BPrivate::BCommandPipe pipe;
		pipe.AddArg("/bin/sh");
		pipe.AddArg("-c");
		pipe.AddArg(commandString);
		pipe.RunAsync();
	}

	return B_OK;
}


void
populate_menu(BMessage* message, BMenu* menu, BHandler* handler)
{
	if (menu == NULL)
		return;

	// get rid of our old menu and start from scratch
	BMenuItem* menuItem = menu->FindItem(kSuperMenuWhat);
	if (menuItem != NULL)
		menu->RemoveItem(menuItem);

	BMessage prefsMessage;
	Preferences::ReadPreferences(prefsMessage);

	BMenu* xMenu = new BMenu(prefsMessage.GetString(kMenuLabelKey, kAppTitle));
	BLayoutBuilder::Menu<> builder = BLayoutBuilder::Menu<>(xMenu);

	bool useIcons = prefsMessage.GetBool(kIconMenusKey, kIconMenusDefault);

	if (prefsMessage.HasMessage(kEntryKey)) {
		BMessage itemMessage;
		for (int32 index = 0; prefsMessage.FindMessage(kEntryKey, index, &itemMessage) == B_OK; index++) {
			if (itemMessage.what != kCommandWhat)
				continue;

			// add each command with a copy of its configuration
			BMessage* menuMessage = new BMessage(*message);
			menuMessage->AddInt32(kAddOnWhatKey, kCommandWhat);
			menuMessage->AddMessage(kCommandDataKey, &itemMessage);
			BString commandString;
			if (itemMessage.FindString(kEntryCommandKey, &commandString) != B_OK)
				continue;

			if (useIcons) {
				IconMenuItem* item = NULL;
				BString commandFile(commandString);
				// remove escape characters from spaces
				BEntry entry(commandFile.ReplaceAll("\\ ", " "));
				int32 spacePosition = -1;
				int32 spaceOffset = 0;
				// attempt to find the actual file by splitting the command/arguments at the spaces
				while (!entry.Exists() && (spacePosition = commandString.FindFirst(' ', spaceOffset)) != B_ERROR) {
					commandFile = commandString;
					commandFile.Remove(spacePosition, commandFile.Length() - spacePosition);
					commandFile.ReplaceAll("\\ ", " ");
					spaceOffset = spacePosition + 1;
					entry.SetTo(commandFile);
				}

				BNode node(&entry);
				if (node.InitCheck() == B_OK) {
					BNodeInfo info(&node);
					if (info.InitCheck() == B_OK)
						item = new IconMenuItem(itemMessage.FindString(kEntryNameKey), menuMessage, &info, B_MINI_ICON);
				}

				// fall back to text/plain if we couldn't look up the proper type
				if (item == NULL)
					item = new IconMenuItem(itemMessage.FindString(kEntryNameKey), menuMessage, "text/plain");

				builder.AddItem(item);
			} else {
				// add a standard(no icon) menuitem
				builder.AddItem(itemMessage.FindString(kEntryNameKey), menuMessage);
			}
		}
	} else {
		builder.AddItem("<no commands>", 'MPTY').SetEnabled(false);
	}

	BMessage* prefsMenuMessage = new BMessage(*message);
	prefsMenuMessage->AddInt32(kAddOnWhatKey, kLaunchPrefsWhat);

	BMessage* guideMenuMessage = new BMessage(*message);
	guideMenuMessage->AddInt32(kAddOnWhatKey, kUserGuideWhat);

	BMessage* githubMenuMessage = new BMessage(*message);
	githubMenuMessage->AddInt32(kAddOnWhatKey, kGithubWhat);

	BMessage* commandsMenuMessage = new BMessage(*message);
	commandsMenuMessage->AddInt32(kAddOnWhatKey, kManageCommandsWhat);

	// clang-format off
	BMenu* subMenu = NULL;
	builder
		.AddSeparator()
		.AddMenu("Preferences & Help")
			.GetMenu(subMenu)
			.AddItem("Manage Commands" B_UTF8_ELLIPSIS, commandsMenuMessage)
			.AddItem("Preferences" B_UTF8_ELLIPSIS, prefsMenuMessage)
			.AddSeparator()
			.AddItem("User Guide", guideMenuMessage)
			.AddItem("Github Page", githubMenuMessage)
		.End()
		.End();
	// clang-format on

	subMenu->SetTargetForItems(handler);
	xMenu->SetTargetForItems(handler);

	if (useIcons) {
		IconMenuItem* xItem = new IconMenuItem(xMenu, new BMessage(kSuperMenuWhat), kAppSignature, B_MINI_ICON);
		menu->AddItem(xItem);
	} else {
		menu->AddItem(xMenu);
		// set a message for our menu so we can find and delete it again the next time through
		menu->FindItem(xMenu->Name())->SetMessage(new BMessage(kSuperMenuWhat));
	}
}


void
message_received(BMessage* message)
{
	int32 what;
	if (message->FindInt32(kAddOnWhatKey, &what) != B_OK)
		return;

	switch (what) {
		case kCommandWhat:
			//TODO improve alert message
			if (_RunCommand(message) != B_OK)
				(new BAlert("ErrorAlert", "Error running command", "OK", NULL, NULL, B_WIDTH_FROM_LABEL, B_STOP_ALERT))->Go();
			break;
		case kManageCommandsWhat:
		{
			BMessage launchMessage(kLaunchManageWhat);
			status_t rc = be_roster->Launch(kAppSignature, &launchMessage);
			if (rc != B_OK && rc != B_ALREADY_RUNNING)
				(new BAlert("ErrorAlert", "Unable to launch TrackRunner application", "OK", NULL, NULL, B_WIDTH_FROM_LABEL, B_STOP_ALERT))->Go();
		}
			break;
		case kLaunchPrefsWhat:
		{
			BMessage launchMessage(kLaunchPrefsWhat);
			status_t rc = be_roster->Launch(kAppSignature, &launchMessage);
			if (rc != B_OK && rc != B_ALREADY_RUNNING)
				(new BAlert("ErrorAlert", "Unable to launch TrackRunner application", "OK", NULL, NULL, B_WIDTH_FROM_LABEL, B_STOP_ALERT))->Go();
		}
			break;
		case kUserGuideWhat:
			(new BAlert("ErrorAlert", "User guide not written yet!", "OK", NULL, NULL, B_WIDTH_FROM_LABEL, B_STOP_ALERT))->Go();
			//TODO handle user guide message
			break;
		case kGithubWhat:
		{
			const char* args[] = { kGithubUrl, NULL };
			status_t rc = be_roster->Launch("application/x-vnd.Be.URL.https", 1, args);
			if (rc != B_OK && rc != B_ALREADY_RUNNING)
				(new BAlert("Error", "Failed to launch URL", "Ok", NULL, NULL, B_WIDTH_AS_USUAL, B_STOP_ALERT))->Go();
		}
			break;
		default:
			break;
	}
}


void
process_refs(entry_ref /*directory*/, BMessage* /*refs*/, void* /*reserved*/)
{
	//TODO process refs
}
