#include "htable.h"

#include "hdef.h"

void combine(HTableCell& a, HTableCell& b, HTableCell& comb) {
    comb.r1 = MIN(a.r1, b.r1);
    comb.r2 = MAX(a.r2, b.r2);
    comb.c1 = MIN(a.c1, b.c1);
    comb.c2 = MAX(a.c2, b.c2);
}

bool overlap(HTableCell& a, HTableCell& b, HTableCell& inter, HTableCell& comb) {
    inter.r1 = MAX(a.r1, b.r1);
    inter.r2 = MIN(a.r2, b.r2);
    inter.c1 = MAX(a.c1, b.c1);
    inter.c2 = MIN(a.c2, b.c2);

    if (inter.r1 < inter.r2 && inter.c1 < inter.c2) {
        combine(a, b, comb);
        return true;
    }

    return false;
}

HTable::HTable() {
    row = col = 0;
}

void HTable::init(int row, int col) {
    this->row = row;
    this->col = col;
    m_mapCells.clear();
    for (int r = 1; r <= row; ++r) {
        for (int c = 1; c <= col; ++c) {
            int id = (r-1) * col + c;
            m_mapCells[id] = HTableCell(r, r+1, c, c+1);
        }
    }
}

bool HTable::getTableCell(int id, HTableCell& rst) {
    if (m_mapCells.find(id) != m_mapCells.end()) {
        rst = m_mapCells[id];
        return true;
    }
    return false;
}

HTableCell HTable::merge(int lt, int rb) {
    HTableCell cell_lt, cell_rb, inter, comb;
    if (getTableCell(lt, cell_lt) && getTableCell(rb, cell_rb)) {
        combine(cell_lt, cell_rb, comb);

        std::map<int, HTableCell>::iterator iter = m_mapCells.begin();
        while (iter != m_mapCells.end()) {
            if (overlap(comb, iter->second, inter, comb)) {
                iter = m_mapCells.erase(iter);
            }
            else {
                ++iter;
            }
        }

        int id = (comb.r1-1) * col + comb.c1;
        m_mapCells[id] = comb;

        return comb;
    }

    return HTableCell();
}
