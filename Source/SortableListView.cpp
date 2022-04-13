// SPDX-License-Identifier: MIT
// SPDX-FileCopyrightText: 2022 Chris Roberts

#include "SortableListView.h"

#include <Window.h>


enum {
	kCommandDragAction = 'DrAg'
};

const char* kDragSourceIndexKey = "source_index";


SortableListView::SortableListView(const char* name, uint32 updateCommand = 0)
	:
	BListView(name),
	fDropTargetIndex(-1),
	fUpdateCommand(updateCommand)
{
}


bool
SortableListView::InitiateDrag(BPoint /*point*/, int32 /*index*/, bool wasSelected)
{
	if (!wasSelected || CountItems() == 1)
		return false;

	BRect dragRect(ItemFrame(CurrentSelection()));

	BMessage message(kCommandDragAction);
	message.AddInt32(kDragSourceIndexKey, CurrentSelection());
	DragMessage(&message, dragRect, this);

	return true;
}


void
SortableListView::Draw(BRect updateRect)
{
	BListView::Draw(updateRect);

	// check if we need to add highlighting
	if (fDropTargetIndex == -1)
		return;

	int32 frameIndex = fDropTargetIndex == CountItems() ? fDropTargetIndex - 1 : fDropTargetIndex;
	BRect dropFrame = ItemFrame(frameIndex);
	if (fDropTargetIndex == CountItems())
		// we're dragging past the last item, start highlight at the bottom of item
		dropFrame.top = dropFrame.bottom;

	dropFrame.bottom = dropFrame.top + 1; // equals 2 pixels
	dropFrame.InsetBySelf(5.0, 0);

	SetHighUIColor(B_CONTROL_HIGHLIGHT_COLOR);
	FillRect(dropFrame);
}


void
SortableListView::MouseMoved(BPoint where, uint32 code, const BMessage* dragMessage)
{
	fDropTargetIndex = -1;

	if (dragMessage == NULL || (code != B_ENTERED_VIEW && code != B_INSIDE_VIEW)) {
		BListView::MouseMoved(where, code, dragMessage);
		// redraw to remove any drop highlight
		Invalidate();
		return;
	}

	for (int32 index = 0; index < CountItems(); index++) {
		BRect itemFrame(ItemFrame(index));
		if (itemFrame.Contains(where)) {
			fDropTargetIndex = index;

			// if we're past the midpoint then drop on the next item
			itemFrame.top = itemFrame.bottom - (itemFrame.Height() / 2.0);
			if (itemFrame.Contains(where))
				fDropTargetIndex++;

			break;
		}
	}

	// check if we're being dropped past the last item
	if (fDropTargetIndex == -1)
		fDropTargetIndex = CountItems();

	// check if we're being dropped onto ourself
	int32 sourceIndex;
	if (dragMessage->FindInt32(kDragSourceIndexKey, &sourceIndex) == B_OK
		&& (sourceIndex == fDropTargetIndex - 1 || sourceIndex == fDropTargetIndex))
		fDropTargetIndex = -1;

	BListView::MouseMoved(where, code, dragMessage);
	Invalidate();
}


void
SortableListView::MessageReceived(BMessage* message)
{
	if (!message->WasDropped()) {
		BListView::MessageReceived(message);
		return;
	}

	if (fDropTargetIndex == -1)
		return;

	int32 sourceIndex;
	if (message->FindInt32(kDragSourceIndexKey, &sourceIndex) != B_OK)
		return;

	if (sourceIndex < fDropTargetIndex)
		fDropTargetIndex--;

	if (MoveItem(sourceIndex, fDropTargetIndex)
		&& fUpdateCommand != 0 // send a message if MoveItem() returns true
		&& Window() != NULL)
		Window()->PostMessage(fUpdateCommand);

	// we're done dropping, clean up and redraw to remove highlights
	fDropTargetIndex = -1;
	Invalidate();
}
