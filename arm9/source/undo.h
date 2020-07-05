#ifndef UNDO_H
#define UNDO_H

#include "state.h"
#include "cell_array.h"
#include "ntxm/song.h"

class UndoAction {
    public:
        virtual ~UndoAction() = 0;
        virtual bool valid() { return true; };
        virtual bool undo(Song *song) = 0;
};

class LocatedUndoAction: public UndoAction {
    public:
        LocatedUndoAction(State *state);
        LocatedUndoAction(State *state, u16 row, u8 channel);
        virtual ~LocatedUndoAction() = 0;

    protected:
		u8 potpos;
		u16 row;
		u8 channel;
};

class SingleCellUndoAction: public LocatedUndoAction {
    public:
        SingleCellUndoAction(State *state, Song *song);
        SingleCellUndoAction(State *state, Cell cell);
        bool undo(Song *song);

    protected:
        Cell cell;
};

class MultiCellUndoAction: public LocatedUndoAction {
    public:
        MultiCellUndoAction(State *state, Song *song);
        MultiCellUndoAction(State *state, Song *song, u16 x1, u16 y1, u16 x2, u16 y2);
        MultiCellUndoAction(State *state, Song *song, u16 x1, u16 y1, CellArray *array);
        ~MultiCellUndoAction();
        bool undo(Song *song);
        bool valid();

    protected:
		CellArray *array;
};

class UndoBuffer {
	public:
        UndoBuffer(int size);

		~UndoBuffer();
		
        bool valid();

        void clear();
        void insert(UndoAction *action);

        bool undo(Song *song);

        int queue_length();
        int size();

	private:
        void clear_at(int i);

        int action_size, a_head, a_tail;
		UndoAction **actions;
};

#endif