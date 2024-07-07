// SPDX-License-Identifier: MIT
// SPDX-FileCopyrightText: 2022 Chris Roberts

#ifndef _RUNNERAPP_H_
#define _RUNNERAPP_H_

#include <Application.h>


class RunnerApp : public BApplication {
public:
					RunnerApp();

	virtual void	AboutRequested();
	virtual void	MessageReceived(BMessage* message);
	virtual void	ReadyToRun();
	virtual void	RefsReceived(BMessage* message);

private:
		void		_ShowManageWindow();
		void		_ShowSettingsWindow();

	bool		fInitialWindowShown;
};

#endif // _RUNNERAPP_H_
