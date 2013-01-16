#ifndef NRECT_H
#define NRECT_H

class NRect //прямоугольная область //нумерация строк и столбцов идет от 0
{
  public:
    NRect() {};
    NRect(int rows, int cols, int begrow, int begcol) {this->begrow = begrow; this->begcol = begcol; this->rows = rows; this->cols = cols;};
    int begrow;
    int begcol;
    int rows; //высота (число строк)
    int cols; //ширина (число колонок)
};

#endif //NRECT_H