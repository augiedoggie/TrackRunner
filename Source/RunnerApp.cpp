// SPDX-License-Identifier: MIT
// SPDX-FileCopyrightText: 2022 Chris Roberts

#include "RunnerApp.h"
#include "CommandSelectWindow.h"
#include "CommandsWindow.h"
#include "Constants.h"
#include "PreferencesWindow.h"

#include <Alert.h>
#include <Application.h>
#include <Catalog.h>


#undef B_TRANSLATION_CONTEXT
#define B_TRANSLATION_CONTEXT "RunnerApp"


RunnerApp::RunnerApp()
	:
	BApplication(kAppSignature),
	fInitialWindowShown(false)
{
}


void
RunnerApp::MessageReceived(BMessage* message)
{
	switch (message->what) {
		case kLaunchPrefsWhat:
		{
			// search for existing preferences window
			for (int32 index = 0; index < CountWindows(); index++) {
				PreferencesWindow* window = dynamic_cast<PreferencesWindow*>(WindowAt(index));
				if (window != NULL) {
					window->Activate();
					return;
				}
			}

			fInitialWindowShown = true;
			_ShowPreferencesWindow();
		}
			break;
		case kLaunchManageWhat:
		{
			// search for existing commands window
			for (int32 index = 0; index < CountWindows(); index++) {
				CommandsWindow* window = dynamic_cast<CommandsWindow*>(WindowAt(index));
				if (window != NULL) {
					window->Activate();
					return;
				}
			}

			fInitialWindowShown = true;
			_ShowManageWindow();
			break;
		}
		default:
			BApplication::MessageReceived(message);
			break;
	}
}


void
RunnerApp::ReadyToRun()
{
	if (fInitialWindowShown)
		return;

	// we didn't receive a launch message so open a command window
	//TODO show about window?
	_ShowManageWindow();
}


void
RunnerApp::RefsReceived(BMessage* message)
{
	CommandSelectWindow* window = new CommandSelectWindow(message);
	window->Lock();
	window->CenterOnScreen();
	window->Show();
	window->Unlock();
	fInitialWindowShown = true;
}


void
RunnerApp::AboutRequested()
{
	(new BAlert("AboutWindow", B_TRANSLATE("TrackRunner\nWritten by Chris Roberts"),
		B_TRANSLATE("OK"), NULL, NULL, B_WIDTH_FROM_LABEL))->Go();
}


void
RunnerApp::_ShowPreferencesWindow()
{
	BString title(B_TRANSLATE("%trackrunner% settings"));
	title.ReplaceFirst("%trackrunner%", kAppTitle);
	PreferencesWindow* window = new PreferencesWindow(title);
	window->Lock();
	window->CenterOnScreen();
	window->Show();
	window->Unlock();
}


void
RunnerApp::_ShowManageWindow()
{
	BString title(B_TRANSLATE("%trackrunner% commands"));
	title.ReplaceFirst("%trackrunner%", kAppTitle);
	CommandsWindow* window = new CommandsWindow(title);
	window->Lock();
	window->CenterOnScreen();
	window->Show();
	window->Unlock();
}


int
main(int /*argc*/, char** /*argv*/)
{
	RunnerApp app;
	app.Run();

	return 0;
}
