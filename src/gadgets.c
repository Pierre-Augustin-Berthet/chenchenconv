#include "gadgets.h"

void    SecAnd              (MaskedB out, MaskedB ina, MaskedB inb);
void    SecOr               (MaskedB out, MaskedB ina, MaskedB inb);

void    RefreshMasks        ();
void    Refresh             ();

void    SecAdd              ();
void    SecMult             ();

void    A2B                 (MaskedB out, MaskedA in);
void    B2A                 (MaskedA out, MaskedB in);
void    B2A_bit             ();