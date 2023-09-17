// SPDX-License-Identifier: MIT
// SPDX-FileCopyrightText: 2022 Chris Roberts

#ifndef _COMMANDSWINDOW_H_
#define _COMMANDSWINDOW_H_

#include <Window.h>


class BButton;
class BCheckBox;
class BFilePanel;
class BListView;
class BTextControl;

namespace BPrivate
{
class BToolBar;
}


class CommandsWindow : public BWindow {

public:
						CommandsWindow(BString& title);
	virtual void		MessageReceived(BMessage* message);
	virtual bool		QuitRequested();

private:
			void		_InitControls();
			void		_InitNewCommand();
			void		_DeleteCommand();
			void		_SelectItem();
			void		_UpdateItem();
			void		_BrowseCommand();
			void		_RefsReceived(BMessage* message);
			status_t	_LoadCommands();
			status_t	_SaveCommands();
			BBitmap*	_ResourceBitmap(const char* name, float width, float height);

		BButton*		fBrowseButton;
		BCheckBox*		fTerminalCheckBox;
		BListView*		fListView;
		BFilePanel*		fBrowsePanel;
		BTextControl*	fCommandControl;
		BTextControl*	fNameControl;
		BPrivate::BToolBar*	fToolbar;
};

#endif // _COMMANDSWINDOW_H_
