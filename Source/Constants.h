// SPDX-License-Identifier: MIT
// SPDX-FileCopyrightText: 2022 Chris Roberts

#ifndef _CONSTANTS_H_
#define _CONSTANTS_H_

#ifdef __GNUC__
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunused-variable"
#endif

static const char* kAppSignature		= "application/x-vnd.cpr.TrackRunner";
static const char* kTerminalSignature		= "application/x-vnd.Haiku-Terminal";

static const char* kAppTitle = "TrackRunner";

static const char* kGithubUrl = "https://github.com/augiedoggie/trackrunner";

enum {
	kLaunchPrefsWhat = 'PREF',
	kLaunchManageWhat = 'MNGE'
};

// keys for our preference message entries
static const char* kMenuLabelKey			= "MenuLabel";
static const char* kAddOnWhatKey			= "addon:what";
static const char* kEntryKey				= "CommandEntry";
static const char* kEntryNameKey			= "Name";
static const char* kEntryCommandKey			= "Command";
static const char* kEntryUseTerminalKey		= "UseTerminal";
static const char* kIconMenusKey			= "UseIcons";
static bool kIconMenusDefault = true;

#ifdef __GNUC__
#pragma GCC diagnostic pop
#endif

#endif // _CONSTANTS_H_
