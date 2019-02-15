#include <genesis.h>

#include "sprite.h"

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

int main() {

    // disable interrupt when accessing VDP
    SYS_disableInts();
    // initialization
    VDP_setScreenWidth320();
    // init sprites engine
    SPR_init(0, 0, 0);
    // set sonic palette
    VDP_setPalette(PAL0,sonic_sprite.palette->data);

    //load background


    // VDP process done, we can re enable interrupts
    SYS_enableInts();

    // init sonic sprite
    spr_sonic = SPR_addSprite(&sonic_sprite, 128, 128, TILE_ATTR(PAL0, TRUE, FALSE,  FALSE));
    SPR_setAnim(spr_sonic, 7);

    while(TRUE) {
        SPR_update();
        VDP_waitVSync();
    }
    return 0;
}
