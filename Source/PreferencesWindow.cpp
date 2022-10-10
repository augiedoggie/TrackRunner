// SPDX-License-Identifier: MIT
// SPDX-FileCopyrightText: 2022 Chris Roberts

#include "PreferencesWindow.h"
#include "Constants.h"
#include "Preferences.h"

#include <Application.h>
#include <Button.h>
#include <CheckBox.h>
#include <LayoutBuilder.h>


enum {
	kDefaultsButtonWhat = 'DfLt',
#ifdef USE_MENUITEM_ICONS
	kIconCheckBoxWhat = 'IkOn',
#endif
	kMenuLabelWhat = 'LaBl'
};


PreferencesWindow::PreferencesWindow(BString& title)
	:
	BWindow(BRect(100, 100, 400, 300), title, B_TITLED_WINDOW,
		B_ASYNCHRONOUS_CONTROLS | B_AUTO_UPDATE_SIZE_LIMITS | B_CLOSE_ON_ESCAPE)
{
	BMessage message;
	Preferences::ReadPreferences(message);

	fMenuLabelControl = new BTextControl("Menu label:", message.GetString(kMenuLabelKey, kAppTitle), NULL);
	fMenuLabelControl->SetModificationMessage(new BMessage(kMenuLabelWhat));

#ifdef USE_MENUITEM_ICONS
	fIconMenuCheckBox = new BCheckBox("Use icons in menus", new BMessage(kIconCheckBoxWhat));
	BSize size(fIconMenuCheckBox->ExplicitMaxSize());
	size.SetWidth(B_SIZE_UNLIMITED);
	fIconMenuCheckBox->SetExplicitMaxSize(size);
	fIconMenuCheckBox->SetValue(message.GetBool(kIconMenusKey, kIconMenusDefault));
#endif

	// clang-format off
	BLayoutBuilder::Group<>(this, B_VERTICAL, 0)
		.SetInsets(B_USE_HALF_ITEM_INSETS)
		.Add(fMenuLabelControl)
#ifdef USE_MENUITEM_ICONS
		.Add(fIconMenuCheckBox)
#endif
		.AddGlue()
		.AddGroup(B_HORIZONTAL)
			.AddGlue()
			.Add(new BButton("DefaultsButton", "Defaults", new BMessage(kDefaultsButtonWhat)))
			.Add(new BButton("CloseButton", "Close", new BMessage(B_QUIT_REQUESTED)))
		.End()
	.End();
	// clang-format on
}


bool
PreferencesWindow::QuitRequested()
{
	if (be_app->CountWindows() == 1)
		be_app->PostMessage(B_QUIT_REQUESTED);

	return true;
}


void
PreferencesWindow::MessageReceived(BMessage* message)
{
	switch (message->what) {
		case kDefaultsButtonWhat:
			fMenuLabelControl->SetText(kAppTitle);
#ifdef USE_MENUITEM_ICONS
			fIconMenuCheckBox->SetValue(kIconMenusDefault);
#endif
			_WritePreferences();
			break;
#ifdef USE_MENUITEM_ICONS
		case kIconCheckBoxWhat:
			_WritePreferences();
			break;
#endif
		case kMenuLabelWhat:
			_WritePreferences();
			break;
		default:
			BWindow::MessageReceived(message);
	}
}


status_t
PreferencesWindow::_WritePreferences()
{
	BMessage message;

	Preferences::ReadPreferences(message);

#ifdef USE_MENUITEM_ICONS
	message.SetBool(kIconMenusKey, fIconMenuCheckBox->Value());
#endif


	BString menuLabel(message.GetString(kMenuLabelKey, kAppTitle));
	BString newLabel(fMenuLabelControl->Text());
	if (newLabel != menuLabel && newLabel.Length() > 3)
		message.SetString(kMenuLabelKey, newLabel);
		//TODO else show textcontrol in red color?

	return Preferences::WritePreferences(message);
}
