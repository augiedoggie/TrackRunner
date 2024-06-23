// SPDX-License-Identifier: MIT
// SPDX-FileCopyrightText: 2022 Chris Roberts

#include "PreferencesWindow.h"
#include "Constants.h"
#include "Preferences.h"

#include <Application.h>
#include <Button.h>
#include <CheckBox.h>
#include <LayoutBuilder.h>


#ifdef HAIKU_ENABLE_I18N
#include <Catalog.h>

#undef B_TRANSLATION_CONTEXT
#define B_TRANSLATION_CONTEXT "PreferencesWindow"
#else
#define B_TRANSLATE(x) x
#endif


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

	fMenuLabelControl = new BTextControl(B_TRANSLATE("Menu label:"),
		message.GetString(kMenuLabelKey, kAppTitle), NULL);
	fMenuLabelControl->SetModificationMessage(new BMessage(kMenuLabelWhat));

#ifdef USE_MENUITEM_ICONS
	fIconMenuCheckBox = new BCheckBox(B_TRANSLATE("Use icons in menus"), new BMessage(kIconCheckBoxWhat));
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
			.Add(new BButton("DefaultsButton", B_TRANSLATE("Defaults"), new BMessage(kDefaultsButtonWhat)))
			.Add(new BButton("CloseButton", B_TRANSLATE("Close"), new BMessage(B_QUIT_REQUESTED)))
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
	// show the text and input background in red if it's too short
	rgb_color textColor = {255, 0, 0, 0};
	rgb_color viewColor = textColor;
	if (newLabel.Length() > 3) {
		// only save the new one if it's long enough
		message.SetString(kMenuLabelKey, newLabel);
		textColor = ui_color(B_DOCUMENT_TEXT_COLOR);
		viewColor = ui_color(B_PANEL_BACKGROUND_COLOR);
	}
	BFont font;
	fMenuLabelControl->TextView()->GetFontAndColor(0, &font);
	fMenuLabelControl->TextView()->SetFontAndColor(&font, B_FONT_ALL, &textColor);
	fMenuLabelControl->SetViewColor(viewColor);

	return Preferences::WritePreferences(message);
}
