#include <genesis.h>

#include "sprite.h"
#include "gfx.h"

#define ANIM_STAND      0
#define ANIM_WAIT       1
#define ANIM_WALK       2
#define ANIM_RUN        3
#define ANIM_BRAKE      4
#define ANIM_UP         5
#define ANIM_CROUNCH    6
#define ANIM_ROLL       7
#define ANIM_DAMAGE     8

#define MAX_SPEED       FIX32(8)
#define RUN_SPEED       FIX32(6)
#define BRAKE_SPEED     FIX32(2)

#define JUMP_SPEED      FIX32(-7)
#define GRAVITY         FIX32(0.3)
#define ACCEL           FIX32(0.1)
#define DE_ACCEL        FIX32(0.15)

#define MIN_POSX        FIX32(10)
#define MAX_POSX        FIX32(400)
#define MAX_POSY        FIX32(156)

Sprite* spr_sonic;
Map* map = NULL;

int main() {
    u16 palette[64];
    //u16 ind;

    // disable interrupt when accessing VDP
    SYS_disableInts();
    // initialization
    VDP_setScreenWidth320();

    //VDP_setTextPalette(PAL0);

    // init sprites engine
    SPR_init(16, 256, 256);

    VDP_setPalette(PAL2, sonic_sprite.palette->data);

    SYS_enableInts();

    // prepare palettes
    memcpy(&palette[0], sonic_sprite.palette->data, 16 * 2);


    VDP_setPalette(PAL1, cenario.palette->data);
    VDP_loadTileSet(cenario.tileset, 10, TRUE);
    map = unpackMap(cenario.map, NULL);
    VDP_setMapEx(PLAN_A, map, TILE_ATTR_FULL(PAL1, FALSE, FALSE, FALSE,10), 0, 0, 0, 13, 64, 32);


    // init sonic sprite
    spr_sonic = SPR_addSprite(&sonic_sprite, 128, 153, TILE_ATTR(PAL2, TRUE, FALSE,  FALSE));
    SPR_setAnim(spr_sonic, ANIM_STAND);

    while(TRUE) {
        SPR_update();
        VDP_waitVSync();
    }
    return 0;
}
