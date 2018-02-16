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
#ifndef SOLARUS_HQ4X_FILTER_H
#define SOLARUS_HQ4X_FILTER_H

#include "solarus/graphics/SoftwareVideoMode.h"

namespace Solarus {
const SoftwareVideoMode::FilterSource HQ4X_FILTER{
  .vertex_source = R"(
                   void main() {
                   // transform the vertex position
                       gl_Position = gl_ModelViewProjectionMatrix * gl_Vertex;

                       // transform the texture coordinates
                       gl_TexCoord[0] = gl_TextureMatrix[0] * gl_MultiTexCoord0;

                       // forward the vertex color
                       gl_FrontColor = gl_Color;
                   }
                   )",
  .fragment_source = R"(
                     uniform sampler2D sol_texture;
                           uniform vec2 sol_input_size;
                           uniform vec2 sol_output_size;
                           vec2 sol_texture_size = sol_input_size;

                           const float mx = 0.325;    // start smoothing factor
                           const float k = -0.250;    // smoothing decrease factor
                           const float max_w = 0.25;
                           const float min_w =-0.10;  // min smoothing/sharpening weigth
                           const float lum_add = 0.2; // effects smoothing

                           vec2 texcoord = gl_TexCoord[0].xy;

                           void main()
                           {
                               float x = 0.5 / sol_texture_size.x;
                               float y = 0.5 / sol_texture_size.y;

                               vec2 dg1 = vec2( x,y);
                               vec2 dg2 = vec2(-x,y);
                               vec2 sd11 = dg1*0.5;
                               vec2 sd21 = dg2*0.5;

                               vec3 c  = texture2D(sol_texture, texcoord).xyz;
                               vec3 i1 = texture2D(sol_texture, texcoord - sd11).xyz;
                               vec3 i2 = texture2D(sol_texture, texcoord - sd21).xyz;
                               vec3 i3 = texture2D(sol_texture, texcoord + sd11).xyz;
                               vec3 i4 = texture2D(sol_texture, texcoord + sd21).xyz;
                               vec3 o1 = texture2D(sol_texture, texcoord - dg1).xyz;
                               vec3 o3 = texture2D(sol_texture, texcoord + dg1).xyz;
                               vec3 o2 = texture2D(sol_texture, texcoord - dg2).xyz;
                               vec3 o4 = texture2D(sol_texture, texcoord + dg2).xyz;

                               vec3 dt = vec3(1.0);

                               float ko1=dot(abs(o1-c),dt);
                               float ko2=dot(abs(o2-c),dt);
                               float ko3=dot(abs(o3-c),dt);
                               float ko4=dot(abs(o4-c),dt);

                               float sd1 = dot(abs(i1-i3),dt);
                               float sd2 = dot(abs(i2-i4),dt);

                               float w1 = sd2; if (ko3<ko1) w1 = 0.0;
                               float w2 = sd1; if (ko4<ko2) w2 = 0.0;
                               float w3 = sd2; if (ko1<ko3) w3 = 0.0;
                               float w4 = sd1; if (ko2<ko4) w4 = 0.0;

                               c = (w1*o1+w2*o2+w3*o3+w4*o4+0.1*c)/(w1+w2+w3+w4+0.1);

                               w3 = k/(0.4*dot(c,dt)+lum_add);

                               w1 = clamp(w3*sd1+mx,min_w,max_w);
                               w2 = clamp(w3*sd2+mx,min_w,max_w);

                               gl_FragColor = vec4(w1*(i1+i3) + w2*(i2+i4) + (1.0-2.0*(w1+w2))*c, 1.0);
                           }
                     )"
};
}

#endif

