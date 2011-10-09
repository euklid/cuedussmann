/*
This file is part of cuedussmann.

    cuedussmann is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    cuedussmann is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with cuedussmann.  If not, see <http://www.gnu.org/licenses/>.

*/

#ifndef HOWTOUSE_H
#define HOWTOUSE_H

#include <QWidget>
#include "ui_howtouse.h"

namespace Ui {
    class HowToUse;
}

class HowToUse : public QWidget
{
    Q_OBJECT

public:
    explicit HowToUse(QWidget *parent = 0);
    ~HowToUse();
    Ui::HowToUse *ui;


};

#endif // HOWTOUSE_H
