#include "hlayout.h"
#include "hdef.h"

HLayout::HLayout()
{

}

void HLayout::init(int row, int col){
    this->row = row;
    this->col = col;
    num = row * col;
    m_mapCells.clear();
    for (int r = 1; r <= row; ++r){
        for (int c = 1; c <= col; ++c){
            int id = (r-1) * col + c;
            m_mapCells[id] = HLayoutCell(r,r,c,c);
        }
    }
}

bool HLayout::getLayoutCell(int id, HLayoutCell& rst){
    if (m_mapCells.find(id) != m_mapCells.end()){
        rst = m_mapCells[id];
        return true;
    }
    return false;
}

HLayoutCell HLayout::merge(int lt, int rb){
    HLayoutCell cell_lt,cell_rb;
    if (getLayoutCell(lt, cell_lt) && getLayoutCell(rb, cell_rb)){
        int r1 = MIN(cell_lt.r1, cell_rb.r1);
        int r2 = MAX(cell_lt.r2, cell_rb.r2);
        int c1 = MIN(cell_lt.c1, cell_rb.c1);
        int c2 = MAX(cell_lt.c2, cell_rb.c2);

        HLayoutCell cell(r1, r2, c1, c2);
        std::map<int, HLayoutCell>::iterator iter = m_mapCells.begin();
        while (iter != m_mapCells.end()){
            if (cell.contain(iter->second)){
                iter = m_mapCells.erase(iter);
            }else
                ++iter;
        }
        m_mapCells[lt] = cell;

        return cell;
    }
}
