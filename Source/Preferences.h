// SPDX-License-Identifier: MIT
// SPDX-FileCopyrightText: 2022 Chris Roberts

#ifndef _PREFERENCES_H_
#define _PREFERENCES_H_

#include <SupportDefs.h>

class BMessage;

class Preferences {
public:
	static status_t ReadPreferences(BMessage& message);
	static status_t WritePreferences(BMessage& message);
};

#endif // _PREFERENCES_H_
