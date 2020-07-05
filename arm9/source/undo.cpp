#include <stdlib.h>
#include <string.h>
#include <nds.h>

#include "undo.h"
#include "tools.h"

// utils

static bool checkBoundsInSong(Song *song, u8 potpos, u16 row, u8 channel)
{
    if (channel >= song->getChannels())
        return false;
    if (potpos >= song->getPotLength())
        return false;
    u8 potEntry = song->getPotEntry(potpos);
    if (row >= song->getPatternLength(potEntry))
        return false;
    return true;
}

// actions

UndoAction::~UndoAction() { }
LocatedUndoAction::~LocatedUndoAction() { }

LocatedUndoAction::LocatedUndoAction(State *state)
    :potpos(state->potpos), row(state->row), channel(state->channel)
{

}

LocatedUndoAction::LocatedUndoAction(State *state, u16 row_, u8 channel_)
    :potpos(state->potpos), row(row_), channel(channel_)
{

}

SingleCellUndoAction::SingleCellUndoAction(State *state, Song *song)
    :LocatedUndoAction(state)
{
    cell = song->getPattern(song->getPotEntry(potpos))[channel][row];
}

SingleCellUndoAction::SingleCellUndoAction(State *state, Cell cell_)
    :LocatedUndoAction(state), cell(cell_)
{

}

bool SingleCellUndoAction::undo(Song *song)
{
    if (!checkBoundsInSong(song, potpos, row, channel))
        return false;

    song->getPattern(song->getPotEntry(potpos))[channel][row] = cell;
    return true;
}

MultiCellUndoAction::MultiCellUndoAction(State *state, Song *song)
    :LocatedUndoAction(state, 0, 0)
{
    Cell **ptn = song->getPattern(song->getPotEntry(potpos));
    array = new CellArray(ptn, 0, 0, song->getChannels(), song->getPatternLength(song->getPotEntry(potpos)));  
}

MultiCellUndoAction::MultiCellUndoAction(State *state, Song *song, u16 x1, u16 y1, u16 x2, u16 y2)
    :LocatedUndoAction(state, y1, x1)
{
    Cell **ptn = song->getPattern(song->getPotEntry(potpos));
    array = new CellArray(ptn, x1, y1, x2, y2);  
}

MultiCellUndoAction::MultiCellUndoAction(State *state, Song *song, u16 x1, u16 y1, CellArray *array_bounds)
    :LocatedUndoAction(state, y1, x1)
{
    int x2 = min(song->getChannels(), x1 + array_bounds->width());
    int y2 = min(song->getPatternLength(song->getPotEntry(potpos)), y1 + array_bounds->height());

    array = new CellArray(song->getPattern(song->getPotEntry(potpos)), x1, y1, x2, y2);  
}

MultiCellUndoAction::~MultiCellUndoAction()
{
    if (array != NULL) delete array;
}

bool MultiCellUndoAction::undo(Song *song)
{
    if (!checkBoundsInSong(song, potpos, row + array->height() - 1, channel + array->width() - 1))
        return false;

    Cell **ptn = song->getPattern(song->getPotEntry(potpos));
    array->paste(ptn, song->getChannels(), song->getPatternLength(song->getPotEntry(potpos)), channel, row);
    return true;
}

bool MultiCellUndoAction::valid()
{
    return array != NULL && array->valid();
}

// buffer

void UndoBuffer::clear_at(int i)
{
    if (actions[i] != NULL)
    {
        delete actions[i];
        actions[i] = NULL;
    }
}

UndoBuffer::UndoBuffer(int size)
    :action_size(size)
{
    actions = (UndoAction**) malloc(sizeof(UndoAction*) * action_size);
    memset(actions, 0, sizeof(UndoAction*) * action_size);
    clear();
}

UndoBuffer::~UndoBuffer()
{
    if (actions != NULL) free(actions);
}

bool UndoBuffer::valid()
{
    return actions != NULL;
}

void UndoBuffer::clear()
{
    for (int i = 0; i < action_size; i++)
        clear_at(i);

    a_head = 0;
    a_tail = 0;
}

void UndoBuffer::insert(UndoAction *action)
{
    clear_at(a_tail);
    actions[a_tail] = action;
    a_tail = (a_tail + 1) % action_size;
#ifdef DEBUG
    iprintf("undo push => %d\n", queue_length());
#endif
}

bool UndoBuffer::undo(Song *song)
{
    if (a_head == a_tail) return false;

    int a_pos = a_tail == 0 ? (action_size - 1) : (a_tail - 1);
    sassert(actions[a_pos] != NULL, "action is null! (%d %d)", a_head, a_tail);

    bool res = actions[a_pos]->undo(song);
    if (res)
    {
        // undo successful, descend
        clear_at(a_pos);
        a_tail = a_pos;
#ifdef DEBUG
        iprintf("undo pop => %d\n", queue_length());
#endif
        return true;
    }
    else
    {
        // undo failed, clear all
#ifdef DEBUG
        iprintf("undo buffer trashed!\n");
#endif
        clear();
        return false;
    }
}

int UndoBuffer::queue_length()
{
    int x = a_tail - a_head;
    return x < 0 ? (action_size + x) : x;
}

int UndoBuffer::size()
{
    return action_size;
}