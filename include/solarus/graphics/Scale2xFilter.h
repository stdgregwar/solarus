/*
 * Copyright (C) 2006-2018 Christopho, Solarus - http://www.solarus-games.org
 *
 * Solarus is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * Solarus is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License along
 * with this program. If not, see <http://www.gnu.org/licenses/>.
 */
#ifndef SOLARUS_SCALE2X_FILTER_H
#define SOLARUS_SCALE2X_FILTER_H

#include "solarus/graphics/SoftwareVideoMode.h"

namespace Solarus {
SoftwareVideoMode::FilterSource SCALE2X_FILTER{
  .vertex_source = R"(
                   #version 120
                   uniform vec2 sol_input_size;
                   uniform vec2 sol_output_size;
                   vec2 sol_texture_size = sol_input_size;

                   void main() {
                   vec4 offsetx;
                   vec4 offsety;

                   gl_Position = gl_ModelViewProjectionMatrix * gl_Vertex;

                   offsetx.x = 1.0 / sol_texture_size.x;
                   offsetx.y = 0.0;
                   offsetx.w = 0.0;
                   offsetx.z = 0.0;
                   offsety.y = 1.0 / sol_texture_size.y;
                   offsety.x = 0.0;
                   offsety.w = 0.0;
                   offsety.z = 0.0;

                   gl_TexCoord[0] = gl_MultiTexCoord0;         //center
                   gl_TexCoord[1] = gl_TexCoord[0] - offsetx;  //left
                   gl_TexCoord[2] = gl_TexCoord[0] + offsetx;  //right
                   gl_TexCoord[3] = gl_TexCoord[0] - offsety;  //top
                   gl_TexCoord[4] = gl_TexCoord[0] + offsety;  //bottom
                   }
                   )",
      .fragment_source = R"(
                         #version 120

                         uniform sampler2D sol_texture;
                         uniform vec2 sol_input_size;
                         uniform vec2 sol_output_size;
                         vec2 sol_texture_size = sol_input_size;

                         void main() {
                         vec4 colD, colF, colB, colH, col, tmp;
                         vec2 sel;

                         col  = texture2D(sol_texture, gl_TexCoord[0].xy);  //central (can be E0-E3)
                         colD = texture2D(sol_texture, gl_TexCoord[1].xy);  //D (left)
                         colF = texture2D(sol_texture, gl_TexCoord[2].xy);  //F (right)
                         colB = texture2D(sol_texture, gl_TexCoord[3].xy);  //B (top)
                         colH = texture2D(sol_texture, gl_TexCoord[4].xy);  //H (bottom)

                         sel = fract(gl_TexCoord[0].xy * sol_texture_size.xy);       //where are we (E0-E3)?
                         //E0 is default
                         if(sel.y >= 0.5) { tmp = colB; colB = colH; colH = tmp; }  //E1 (or E3): swap B and H
                         if(sel.x >= 0.5) { tmp = colF; colF = colD; colD = tmp; }  //E2 (or E3): swap D and F

                         if(colB == colD && colB != colF && colD != colH) {  //do the Scale2x rule
                         col = colD;
                         }

                         gl_FragColor = col;
                         )"
};
}

#endif

