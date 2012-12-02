#ifndef PIECES_STR_SET_INL_H
#define PIECES_STR_SET_INL_H

#include "chain_set.h"

#include <cstring>

#include <algorithm>

using namespace std;


template <BoardLen BOARD_LEN>
ChainSet<BOARD_LEN>::ChainSet()
{
    memset(nodes_, 0, sizeof(nodes_));
    memset(lists_, 0, sizeof(lists_));

    for (int i=0; i<BOARD_LEN*BOARD_LEN; ++i) {
        nodes_[i].next_ = ChainSet<BOARD_LEN>::NONE_LIST;
    }
}


template <BoardLen BOARD_LEN>
AirCount ChainSet<BOARD_LEN>::GetAirCountOfAPiece(PointIndex piece_i) const
{
    return this->GetAirCountOfAChain(this->GetListHead(piece_i));
}


template <BoardLen BOARD_LEN>
PntIndxVector ChainSet<BOARD_LEN>::GetPieces(PointIndex piece_i) const
{
    return this->GetPiecesOfAChain(this->GetListHead(piece_i));
}


template <BoardLen BOARD_LEN>
void ChainSet<BOARD_LEN>::AddAPiece(const Position &pos,
                                    const ChainSet<BOARD_LEN>::AirSet &air_set)
{
    PointIndex piece_i = this->GetIndexByPos(pos);
    this->CreateList(piece_i, air_set);
    PointIndex list_i = piece_i;

    for (int i=0; i<4; ++i) {
        Position adj_pos = pos.AdjcntPos(i);
        PointIndex adj_i = this->GetIndexByPos(adj_pos);
        PointIndex adj_list = this->GetListHead(adj_i);
        if (adj_list == ChainSet<BOARD_LEN>::NONE_LIST) continue;
        if (adj_list == list_i) continue;
        
        list_i = this->MergeLists(list_i, adj_list);
    }
}


template <BoardLen BOARD_LEN>
void ChainSet<BOARD_LEN>::DeleteAnAir(const Position &pos)
{
}


template <BoardLen BOARD_LEN>
void
ChainSet<BOARD_LEN>::CreateList(PointIndex node_i,
                                const ChainSet<BOARD_LEN>::AirSet &air_set)
{
    nodes_[node_i].list_head_ = node_i;
    lists_[node_i].tail_ = node_i;
    lists_[node_i].len_ = 1;
    lists_[node_i].air_set_ = air_set;
}


//template <BoardLen BOARD_LEN>
//void
//ChainSet<BOARD_LEN>::AppendToList(PointIndex node_i,
//                                  const ChainSet<BOARD_LEN>::AirSet &air_set,
//                                  PointIndex list_i)
//{
//    List *pl = lists_ + list_i;
//    nodes_[pl->tail_].next_ = node_i;
//    nodes_[node_i].list_head_ = list_i;
//    pl->tail_ = node_i;
//    ++(pl->len_);
//    pl->air_set_ |= air_set;
//}


template <BoardLen BOARD_LEN>
PointIndex ChainSet<BOARD_LEN>::MergeLists(PointIndex list_a, PointIndex list_b)
{
    FOO_ASSERT(list_a != list_b);
    if (lists_[list_a].len_ > lists_[list_b].len_) swap(list_a, list_b);

    for (int i=list_a; ; i=nodes_[i].next_) {
        nodes_[i].list_head_ = list_b;
        if (i == lists_[list_a].tail_) break;
    }

    nodes_[lists_[list_b].tail_].next_ = list_a;
    lists_[list_b].tail_ = lists_[list_a].tail_;
    lists_[list_b].len_ += lists_[list_a].len_;
    lists_[list_b].air_set_ |= lists_[list_a].air_set_;
    return list_b;
}


template <BoardLen BOARD_LEN>
void ChainSet<BOARD_LEN>::DeleteList(PointIndex head)
{
    for (int i=head; ; i=nodes_[i].next_) {
        nodes_[i].list_head_ = ChainSet<BOARD_LEN>::NONE_LIST;
        if (i == lists_[head].tail_) break;
    }
}


template <BoardLen BOARD_LEN>
AirCount ChainSet<BOARD_LEN>::GetAirCountOfAChain(PointIndex list_i) const
{
    return lists_[list_i].air_set_.count();
}


template <BoardLen BOARD_LEN>
PntIndxVector ChainSet<BOARD_LEN>::GetPiecesOfAChain(PointIndex list_i) const
{
    const List *pl = lists_ + list_i;
    PntIndxVector v(pl->len_);
    int vi = 0;

    for (int i=list_i; ; i=nodes_[i].next_) {
        v[vi++] = i;
        if (i == pl->tail_) break;
    }

    return v;
}


#endif
