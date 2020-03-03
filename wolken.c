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

/*! \file wolken.c
 * This file contains to code for pattern generator.
 *
 * @author Bernhard R. Fischer
 * @version 2020/02/26
 */

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <sys/time.h>
#include <math.h>
#include <unistd.h>

#include "wolken.h"
#include "memimg.h"
#include "smlog.h"

#define VERSION_STRING "wolken (c) 2020 Bernhard R. Fischer, <bf@abenteuerland.at>"

static int maxval_ = MAXVAL;


/*! Generata pseudo random number.
 * @result Pseudo-random number between 0 and 1.
 */
double Random()
{
   return (double) random() / RAND_MAX;
}


/*! Make sure that value of f is between 0 and maxval_, 0 <= f < maxval_.
 * @param f Pointer to integer to check.
 */
void CheckColor(int *f)
{
   if (*f < 0)
      *f = 0;
   else if (*f > maxval_)
      *f = maxval_;
}


/*! Calculate new color value.
 * @param b Center value around which the color should be.
 * @param mf deviation to b.
 * @result The result will be b +/- mf at the maximum.
 */
int Farbe(int b, double mf)
{
  int f = b + (Random() * 2.0 * mf - mf);
  CheckColor(&f);
  return f;
}


/*! Calculate deviation value.
 */
double col_mf(int n, int sqbits)
{
   return (double) n * maxval_ / sqbits / 2;
}


/*! Generate image.
 * @param mem Pointer to memory image.
 */
void Generate(memimg_t *mem, int sqbits, double mu)
{
   int f1, f2, f3, f4, na, ni, n, n1, nk;
   int i, j, k;
   double mf;

  ni = sqbits - 1;
  na = 1 << sqbits;
  mf = col_mf(ni + 1, sqbits) * mu;

  memimg_put(mem, 0, 0, Farbe(maxval_ / 2, mf));
  memimg_put(mem, 0, na, Farbe(maxval_ / 2, mf));
  memimg_put(mem, na, 0, Farbe(maxval_ / 2, mf));
  memimg_put(mem, na, na, Farbe(maxval_ / 2, mf));
  n = na;
  for (i = 0; i <= ni; i++)
  {
    mf = col_mf(ni + 1 - i, sqbits) * mu;
    n1 = n >> 1;
    for (j = 1; j <= (1 << i); j++)
    {
      f1 = memimg_get(mem, (j - 1) * n, 0);
      f2 = memimg_get(mem, j * n, 0);
      memimg_put(mem, j * n - n1, 0, Farbe((f1 + f2) >> 1, mf));
      f1 = memimg_get(mem, (j - 1) * n, na);
      f2 = memimg_get(mem, j * n, na);
      memimg_put(mem, j * n - n1, na, Farbe((f1 + f2) >> 1, mf));
      f1 = memimg_get(mem, 0, (j - 1) * n);
      f2 = memimg_get(mem, 0, j * n);
      memimg_put(mem, 0, j * n - n1, Farbe((f1 + f2) >> 1, mf));
      f1 = memimg_get(mem, na, (j - 1) * n);
      f2 = memimg_get(mem, na, j * n);
      memimg_put(mem, na, j * n - n1, Farbe((f1 + f2) >> 1, mf));
    }
    n = n1;
  }
  n = na;
  for (i = 0; i <= ni; i++)
  {
    mf = col_mf(ni + 1 - i, sqbits) * mu;
    n1 = n >> 1;
    for (k = 1; k <= (1 << i); k++)
      for (j = 1; j <= (1 << i); j++)
      {
        f1 = memimg_get(mem, (j - 1) * n, (k - 1) * n);
        f2 = memimg_get(mem, (j - 1) * n, k * n);
        f3 = memimg_get(mem, j * n, (k - 1) * n);
        f4 = memimg_get(mem, j * n, k * n);
        memimg_put(mem, j * n - n1, k * n - n1, Farbe((f1 + f2 + f3 + f4) >> 2, mf));
      }
    nk = 0;
    for (k = 1; k <= ((1 << (i + 1)) - 1); k++)
    {
      nk ^= 1;
      for (j = 1; j <= ((1 << i) - nk); j++)
      {
        f1 = memimg_get(mem, j * n - n1 + nk * n1, (k - 1) * n1);
        f2 = memimg_get(mem, j * n + nk * n1, k * n1);
        f3 = memimg_get(mem, j * n - n1 + nk * n1, (k + 1) * n1);
        f4 = memimg_get(mem, (j - 1) * n + nk * n1, k * n1);
        memimg_put(mem, j * n - n1 + nk * n1, k * n1, Farbe((f1 + f2 + f3 + f4) >> 2, mf));
      }
    }
    n = n1;
  }
}


void usage(const char *s)
{
   printf("%s\nusage: %s [OPTIONS] [<filename>]\n", VERSION_STRING, s);
   printf("   OPTIONS\n"
          "      -b <bits> ..... Set image size to 1 << b (default = %d).\n"
          "      -h ............ Print this message.\n"
          "      -m <maxval> ... Maximum color value (default = %d).\n"
          "      -r <decval> ... Roughness, decimal value (default = %.2f).\n"
          "\n", SQBITS, MAXVAL, ROUGH);
}


int main(int argc, char **argv)
{
   memimg_t mem;
   struct timeval tv;
   char *s = "a.png";
   int sqbits = SQBITS, n;
   double mu = ROUGH;         //!< roughness

   init_log("stderr", LOG_INFO);

   while ((n = getopt(argc, argv, "b:hm:r:")) != -1)
      switch (n)
      {
         case 'b':
            sqbits = atoi(optarg);
            break;

         case 'h':
            usage(argv[0]);
            exit(EXIT_SUCCESS);

         case 'm':
            maxval_ = atoi(optarg);
            break;

         case 'r':
            mu = atof(optarg);
            break;
      }

   if (argv[optind] != NULL)
      s = argv[optind];

   gettimeofday(&tv, NULL);
   srandom(tv.tv_sec ^ tv.tv_usec);
   log_debug("iv = %ld", tv.tv_sec ^ tv.tv_usec);
   //srandom(1581554835);

   mem.width = mem.height = (1 << sqbits) + 1;
   if (memimg_init(&mem) == -1)
   {
      log_errno(LOG_ERR, "memimg_init()");
      exit(1);
   }

   Generate(&mem, sqbits, mu);
   memcairo(&mem, s);

   memimg_free(&mem);

   return 0;
}

