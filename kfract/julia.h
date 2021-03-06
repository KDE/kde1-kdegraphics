
    /*

    julia.h  version 0.1.2

    Copyright (C) 1997 Uwe Thiem   
                       uwe@uwix.alt.na

    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program; if not, write to the Free Software
    Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.

    */

#ifndef JULIA_H
# define JULIA_H

#include "fract.h"


class Julia : public Fract 
  {
  public:
    Julia();
    void init( double center_x, double center_y, 
                       double dx, double dy,
                       int x_max, int y_max, 
                       int iter_max, double bail_out,
                       double param_x, double param_y );
    int calcPoint( int x, int y );
    double defaultCenterX();
    double defaultCenterY();
    double defaultWidth();
    double defaultBailout();
    double defaultExtraX();
    double defaultExtraY();
  protected:
  private:
    double julia_x, julia_y;
  };


#endif   // JULIA_H
