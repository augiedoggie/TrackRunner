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
		void		_UpdateMenuLabel();
		status_t	_WritePreferences();

	BCheckBox*		fIconMenuCheckBox;
	BTextControl*	fMenuLabelControl;
};

#endif // _PREFERENCESWINDOW_H_
