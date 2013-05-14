// =============================================================================
// This file is part of boinctui.
// http://boinctui.googlecode.com
// Copyright (C) 2012,2013 Sergey Suslov
//
// boinctui is free software; you can redistribute it and/or modify it  under
// the terms of the GNU General Public License as published by the
// Free Software Foundation, either version 3 of the License, or (at your
// option) any later version.
//
// boinctui is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
// See the GNU General Public License for more details
// <http://www.gnu.org/licenses/>.
// =============================================================================

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