/* d10v ELF support for BFD.
   Copyright 1998, 2000 Free Software Foundation, Inc.

   This file is part of BFD, the Binary File Descriptor library.

   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 2 of the License, or
   (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program; if not, write to the Free Software Foundation,
   Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.  */

#ifndef _ELF_D10V_H
#define _ELF_D10V_H

#include "elf/reloc-macros.h"

/* Relocation types.  */
START_RELOC_NUMBERS (elf_d10v_reloc_type)
  RELOC_NUMBER (R_D10V_NONE, 0)
  RELOC_NUMBER (R_D10V_10_PCREL_R, 1)
  RELOC_NUMBER (R_D10V_10_PCREL_L, 2)
  RELOC_NUMBER (R_D10V_16, 3)
  RELOC_NUMBER (R_D10V_18, 4)
  RELOC_NUMBER (R_D10V_18_PCREL, 5)
  RELOC_NUMBER (R_D10V_32, 6)
  RELOC_NUMBER (R_D10V_GNU_VTINHERIT, 7)
  RELOC_NUMBER (R_D10V_GNU_VTENTRY, 8)
END_RELOC_NUMBERS (R_D10V_max)

#endif
