// SPDX-License-Identifier: MIT
// SPDX-FileCopyrightText: 2022 Chris Roberts

#ifndef _SETTINGS_H_
#define _SETTINGS_H_

#include <SupportDefs.h>

class BMessage;

class Settings {
public:
	static status_t ReadSettings(BMessage& message);
	static status_t WriteSettings(BMessage& message);
};

#endif // _SETTINGS_H_
