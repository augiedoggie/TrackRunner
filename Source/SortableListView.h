// SPDX-License-Identifier: MIT
// SPDX-FileCopyrightText: 2022 Chris Roberts

#ifndef _SORTABLELISTVIEW_H_
#define _SORTABLELISTVIEW_H_

#include <ListView.h>


class SortableListView : public BListView {
public:
					SortableListView(const char* name, uint32 updateCommand);
	virtual void	Draw(BRect updateRect);
	virtual bool	InitiateDrag(BPoint point, int32 index, bool wasSelected);
	virtual void	MessageReceived(BMessage* message);
	virtual void	MouseMoved(BPoint where, uint32 code, const BMessage* dragMessage);

private:
		int32		fDropTargetIndex;
		uint32		fUpdateCommand;
};

#endif // _SORTABLELISTVIEW_H_
