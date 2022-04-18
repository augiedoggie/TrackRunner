// SPDX-License-Identifier: MIT
// SPDX-FileCopyrightText: 2022 Chris Roberts

#ifndef _COMMANDSELECTWINDOW_H_
#define _COMMANDSELECTWINDOW_H_

#include <Window.h>


class BButton;
class BCheckBox;
class BFilePanel;
class BListView;
class BTextControl;


class CommandSelectWindow : public BWindow {
public:
					CommandSelectWindow(BMessage* message);
					~CommandSelectWindow();

	virtual void	MessageReceived(BMessage* message);
	virtual bool	QuitRequested();

private:
		void		_BrowseCommand();
		void		_BrowseWorkingDirectory();
		void		_Run();
		void		_UpdateControls();
		status_t	_LoadCommands();

		BButton*		fRunButton;
		BCheckBox*		fUseTerminalCheckBox;
		BFilePanel*		fCommandFilePanel;
		BFilePanel*		fDirectoryFilePanel;
		BListView*		fCommandListView;
		BMessage*		fRefsMessage;
		BTextControl*	fCommandTextControl;
		BTextControl*	fDirectoryTextControl;
};

#endif // _COMMANDSELECTWINDOW_H_
