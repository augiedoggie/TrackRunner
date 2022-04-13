// SPDX-License-Identifier: MIT
// SPDX-FileCopyrightText: 2022 Chris Roberts

#include "CommandListItem.h"

#include <String.h>


CommandListItem::CommandListItem()
	:
	BStringItem("New Command"),
	fUseTerminal(true),
	fCommand(new BString("/bin/ls"))
{
}


bool
CommandListItem::UseTerminal()
{
	return fUseTerminal;
}


void
CommandListItem::SetUseTerminal(bool enable)
{
	fUseTerminal = enable;
}


const char*
CommandListItem::Command()
{
	return fCommand->String();
}


void
CommandListItem::SetCommand(const char* command)
{
	if (command == NULL)
		fCommand->SetTo("");
	else
		fCommand->SetTo(command);
}
