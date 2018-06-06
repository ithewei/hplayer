#ifndef HLAYOUT_H
#define HLAYOUT_H

#include <map>

class HLayoutCell{
public:
    HLayoutCell(){r1=r2=c1=c2=0;}
    HLayoutCell(int r1, int r2, int c1, int c2){
        this->r1 = r1;
        this->r2 = r2;
        this->c1 = c1;
        this->c2 = c2;
    }

    int getRowspan() {return r2 - r1 + 1;}
    int getColspan() {return c2 - c1 + 1;}
    int getNums() {return getRowspan() * getColspan();}

    bool contain(HLayoutCell cell){
        if (cell.r1 >= r1 && cell.r2 <= r2 &&
                cell.c1 >= c1 && cell.c2 <= c2)
            return true;
        return false;
    }

    int r1,r2,c1,c2;
};

class HLayout
{
public:
    explicit HLayout();

    void init(int row, int col);
    bool getLayoutCell(int id, HLayoutCell& rst);
    HLayoutCell merge(int lt, int rb);

public:
    int row;
    int col;
    int num;
    std::map<int, HLayoutCell> m_mapCells; // id => HLayoutCell
};

#endif // HLAYOUT_H
