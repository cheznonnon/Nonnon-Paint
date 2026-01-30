// Project Nonnon
// copyright (c) nonnon all rights reserved
// License : GPL http://www.gnu.org/copyleft/gpl.html




#define N_GITHUB_REPO


#ifdef N_GITHUB_REPO


#include "../nonnon/neutral/bmp/all.c"
#include "../nonnon/neutral/curico.c"
#include "../nonnon/neutral/jpg.c"
#include "../nonnon/neutral/png.c"


#include "../nonnon/mac/_mac.c"
#include "../nonnon/mac/draw.c"
#include "../nonnon/mac/image.c"
#include "../nonnon/mac/window.c"

#include "../nonnon/mac/n_button.c"
#include "../nonnon/mac/n_scrollbar.c"
#include "../nonnon/mac/n_txtbox.c"

#include "../nonnon/bridge/ini2gdi.c"


#else  // #ifdef N_GITHUB_REPO


#include "../../nonnon/neutral/bmp/all.c"
#include "../../nonnon/neutral/curico.c"
#include "../../nonnon/neutral/jpg.c"
#include "../../nonnon/neutral/png.c"


#include "../../nonnon/mac/_mac.c"
#include "../../nonnon/mac/draw.c"
#include "../../nonnon/mac/image.c"
#include "../../nonnon/mac/window.c"

#include "../../nonnon/mac/n_button.c"
#include "../../nonnon/mac/n_scrollbar.c"
#include "../../nonnon/mac/n_txtbox.c"

#include "../../nonnon/bridge/ini2gdi.c"


#endif // #ifdef N_GITHUB_REPO

