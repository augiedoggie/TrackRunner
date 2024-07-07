// SPDX-License-Identifier: MIT
// SPDX-FileCopyrightText: 2022 Chris Roberts

#include "Settings.h"
#include "Constants.h"

#include <File.h>
#include <FindDirectory.h>
#include <Path.h>


status_t
Settings::ReadSettings(BMessage& message)
{
	BPath prefsPath;
	if (find_directory(B_USER_SETTINGS_DIRECTORY, &prefsPath) != B_OK)
		return B_ERROR;

	prefsPath.Append(kAppTitle);
	BFile prefsFile;

	if (prefsFile.SetTo(prefsPath.Path(), B_READ_WRITE | B_CREATE_FILE) != B_OK)
		return B_ERROR;

	// ignore Unflatten() errors because the file might be newly created
	message.Unflatten(&prefsFile);

	return B_OK;
}


status_t
Settings::WriteSettings(BMessage& message)
{
	BPath prefsPath;
	if (find_directory(B_USER_SETTINGS_DIRECTORY, &prefsPath) != B_OK)
		return B_ERROR;

	prefsPath.Append(kAppTitle);
	BFile prefsFile;

	if (prefsFile.SetTo(prefsPath.Path(), B_READ_WRITE | B_CREATE_FILE | B_ERASE_FILE) != B_OK)
		return B_ERROR;

	return message.Flatten(&prefsFile);
}
