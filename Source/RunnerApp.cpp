// SPDX-License-Identifier: MIT
// SPDX-FileCopyrightText: 2022 Chris Roberts

#include "RunnerApp.h"
#include "CommandsWindow.h"
#include "Constants.h"
#include "PreferencesWindow.h"

#include <Alert.h>
#include <Application.h>


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
		}
			break;
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
RunnerApp::AboutRequested()
{
	(new BAlert("AboutWindow", "TrackRunner\nWritten by Chris Roberts", "OK", NULL, NULL, B_WIDTH_FROM_LABEL))->Go();
}


void
RunnerApp::_ShowPreferencesWindow()
{
	BString title(kAppTitle);
	title << " Preferences";
	PreferencesWindow* window = new PreferencesWindow(title);
	window->Lock();
	window->CenterOnScreen();
	window->Show();
	window->Unlock();
}


void
RunnerApp::_ShowManageWindow()
{
	BString title(kAppTitle);
	title << " Commands";
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
