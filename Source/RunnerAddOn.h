// SPDX-License-Identifier: MIT
// SPDX-FileCopyrightText: 2022 Chris Roberts

#include <Message.h>

class RunnerAddOn {
public:
	static status_t RunCommand(BMessage* message);
	static status_t OpenUserGuide(bool useAppImage = false);
};
