// SPDX-License-Identifier: MIT
// SPDX-FileCopyrightText: 2022 Chris Roberts

#ifndef _SETTINGSWINDOW_H_
#define _SETTINGSWINDOW_H_

#include <Window.h>

class BCheckBox;
class BTextControl;


class SettingsWindow : public BWindow {

public:
					SettingsWindow(BString& title);

	virtual void	MessageReceived(BMessage* message);
	virtual bool	QuitRequested();

private:
		status_t	_WriteSettings();

#ifdef USE_MENUITEM_ICONS
	BCheckBox*		fIconMenuCheckBox;
#endif
	BTextControl*	fMenuLabelControl;
};

#endif // _SETTINGSWINDOW_H_
