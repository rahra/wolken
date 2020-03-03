/* Copyright 2020 Bernhard R. Fischer, 4096R/8E24F29D <bf@abenteuerland.at>
 *
 * This file is part of wolken.
 *
 * Wolken is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, version 3 of the License.
 *
 * Wolken is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with wolken. If not, see <http://www.gnu.org/licenses/>.
 */

/*! \file wolkenv0.3.c
 * This file contains the original code for the pattern generator. This code
 * was written on the Amiga 500 somewhen in the late 1980s. Really old since
 * there are some fragments of K&R syntax (see e.g. line numbers 100 and 101,
 * function PutPixel()).
 *
 * The code contains some strange constructs which I wonder about today but I'm
 * sure this is because of my lack of knowledge at the time then. I was about
 * 16 years old.
 *
 * @author Bernhard R. Fischer
 */

#include <exec/types.h>
#include <intuition/intuition.h>
#include <graphics/gfx.h>
#include <hardware/custom.h>

#define SUPER   register int
#define RND_DIV 37870.87381      /* 2^15 * ¶ / e */
#define FO      31
#define FU      2
#define FA      17

struct IntuitionBase *IntuitionBase;
struct GfxBase       *GfxBase;
struct ViewPort      *vport;
struct Screen        *scr;
struct Window        *win;
struct RastPort      *rport;

struct NewScreen ns =
{
  0, 0, 320, 257, 5, 0, 1,
  0, CUSTOMSCREEN, NULL, NULL, NULL, NULL
};

struct NewWindow nw =
{
  0, 0, 320, 257, 0, 1,
  MOUSEBUTTONS, ACTIVATE |BORDERLESS |RMBTRAP,
  NULL, NULL, NULL, NULL, NULL, 0, 0, 0, 0, CUSTOMSCREEN
};

SHORT f, f1, f2, f3, f4, na, ni, n, n1, nk;
FLOAT mf, mu;


Free()
{
  if (win)           CloseWindow(win);
  if (scr)           CloseScreen(scr);
  if (GfxBase)       CloseLibrary(GfxBase);
  if (IntuitionBase) CloseLibrary(IntuitionBase);
}


Alloc()
{
  if (!(IntuitionBase = (struct IntuitionBase*)
                        OpenLibrary("intuition.library", 0L))) Free();
  if (!(GfxBase       = (struct GfxBase*)
                        OpenLibrary("graphics.library", 0L))) Free();
  if (!(scr           = OpenScreen((struct NewScreen*) &ns))) Free();
  vport = (struct ViewPort*) &scr -> ViewPort;
  nw.Screen = scr;
  if (!(win           = OpenWindow((struct NewWindow*) &nw))) Free();
  rport = win -> RPort;
}


FLOAT Random()
{
  REGISTER FLOAT rnd;

  rnd = (FLOAT) custom.vhposr;
  rnd /= RND_DIV;
  rnd -= (LONG) rnd;
  return(rnd);
}


PutPixel(x, y)
SHORT x, y;
{
  SetAPen(rport, f);
  WritePixel(rport, x, y);
}


FReset()
{
  SUPER i;

  SetRGB4(vport, 0, 0, 0, 0);
  for (i = 1; i < 17; i++) SetRGB4(vport, i, i - 1, i - 1, 15);
  for (i = 17; i < 32; i++) SetRGB4(vport, i, 31 - i, 31 - i, 31 - i);
}


CheckColor()
{
  if (f > FO) f = FO;
  else if (f < FU) f = FU;
}


FarbeA()
{
  f = FA + (SHORT) (Random() * 2.0 * mf - mf + 0.5);
  CheckColor();
}


Farbe()
{
  f = ((f1 + f2) >> 1) + (SHORT) (Random() * 2.0 * mf - mf + 0.5);
  CheckColor();
}


Generate()
{
  struct IntuiMessage *msg;
  SUPER i, j, k;

  na = 256;
  ni = 7;
  mu = 1.6;
  mf = (FLOAT) (ni + 1) * mu;

  FarbeA();
  PutPixel(0, 0);
  FarbeA();
  PutPixel(0, na);
  FarbeA();
  PutPixel(na, 0);
  FarbeA();
  PutPixel(na, na);
  n = na;
  for (i = 0; i <= ni; i++)
  {
    mf = (FLOAT) (ni + 1 - i) * mu;
    n1 = n >> 1;
    for (j = 1; j <= (1 << i); j++)
    {
      f1 = ReadPixel(rport, (j - 1) * n, 0);
      f2 = ReadPixel(rport, j * n, 0);
      Farbe();
      PutPixel(j * n - n1, 0);
      f1 = ReadPixel(rport, (j - 1) * n, na);
      f2 = ReadPixel(rport, j * n, na);
      Farbe();
      PutPixel(j * n - n1, na);
      f1 = ReadPixel(rport, 0, (j - 1) * n);
      f2 = ReadPixel(rport, 0, j * n);
      Farbe();
      PutPixel(0, j * n - n1);
      f1 = ReadPixel(rport, na, (j - 1) * n);
      f2 = ReadPixel(rport, na, j * n);
      Farbe();
      PutPixel(na, j * n - n1);
    }
    n = n1;
  }
  n = na;
  for (i = 0; i <= ni; i++)
  {
    mf = (FLOAT) (ni + 1 - i) * mu;
    n1 = n >> 1;
    for (k = 1; k <= (1 << i); k++)
      for (j = 1; j <= (1 << i); j++)
      {
        f1 = ReadPixel(rport, (j - 1) * n, (k - 1) * n);
        f2 = ReadPixel(rport, (j - 1) * n, k * n);
        f3 = ReadPixel(rport, j * n, (k - 1) * n);
        f4 = ReadPixel(rport, j * n, k * n);
        f = ((f1 + f2 + f3 + f4) >> 2) + (SHORT) (Random() * 2.0 * mf - mf + 0.5);
        CheckColor();
        PutPixel(j * n - n1, k * n - n1);
      }
    nk = 0;
    for (k = 1; k <= ((1 << (i + 1)) - 1); k++)
    {
      nk ^= 1;
      for (j = 1; j <= ((1 << i) - nk); j++)
      {
        f1 = ReadPixel(rport, j * n - n1 + nk * n1, (k - 1) * n1);
        f2 = ReadPixel(rport, j * n + nk * n1, k * n1);
        f3 = ReadPixel(rport, j * n - n1 + nk * n1, (k + 1) * n1);
        f4 = ReadPixel(rport, (j - 1) * n + nk * n1, k * n1);
        f = ((f1 + f2 + f3 + f4) >> 2) + (SHORT) (Random() * 2.0 * mf - mf + 0.5);
        CheckColor();
        PutPixel(j * n - n1 + nk * n1, k * n1);
      }
    }
    n = n1;
    if (msg = (struct IntuiMessage*) GetMsg(win -> UserPort))
    {
      ReplyMsg(msg);
      break;
    }
  }
}


main()
{
  struct IntuiMessage *msg;

  Alloc();
  FReset();
  Generate();
  Wait(1 << win -> UserPort -> mp_SigBit);
  if (msg = GetMsg(win -> UserPort)) ReplyMsg(msg);
  Free();
}
