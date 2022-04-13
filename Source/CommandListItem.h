// SPDX-License-Identifier: MIT
// SPDX-FileCopyrightText: 2022 Chris Roberts

#ifndef _COMMANDLISTITEM_H_
#define _COMMANDLISTITEM_H_

#include <StringItem.h>


class CommandListItem : public BStringItem {

public:
				CommandListItem();

	bool		UseTerminal();
	void		SetUseTerminal(bool enable);
	const char*	Command();
	void		SetCommand(const char* command);

private:
	bool		fUseTerminal;
	BString*	fCommand;
};

#endif // _COMMANDLISTITEM_H_
