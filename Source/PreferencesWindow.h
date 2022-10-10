// SPDX-License-Identifier: MIT
// SPDX-FileCopyrightText: 2022 Chris Roberts

#ifndef _PREFERENCESWINDOW_H_
#define _PREFERENCESWINDOW_H_

#include <Window.h>

class BCheckBox;
class BTextControl;


class PreferencesWindow : public BWindow {

public:
					PreferencesWindow(BString& title);

	virtual void	MessageReceived(BMessage* message);
	virtual bool	QuitRequested();

private:
		status_t	_WritePreferences();

#ifdef USE_MENUITEM_ICONS
	BCheckBox*		fIconMenuCheckBox;
#endif
	BTextControl*	fMenuLabelControl;
};

#endif // _PREFERENCESWINDOW_H_
