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
#define MAX_POSX        FIX32(800)
#define MAX_POSY        FIX32(156)

// forward
static void handleInput();
static void joyEvent(u16 joy, u16 changed, u16 state);

static void updatePhysic();
static void updateAnim();
static void updateCamera(fix32 x, fix32 y);

Sprite* spr_sonic;
Map* map = NULL;

fix32 camposx;
fix32 camposy;
fix32 posx;
fix32 posy;
fix32 movx;
fix32 movy;
s16 xorder;
s16 yorder;

int main() {
    u16 palette[64];

    // disable interrupt when accessing VDP
    SYS_disableInts();
    // initialization
    VDP_setScreenWidth320();
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

    JOY_setEventHandler(joyEvent);

    while(TRUE) {
        handleInput();
        updatePhysic();
        updateAnim();
        SPR_update();
        VDP_waitVSync();
    }
    return 0;
}

static void updatePhysic()
{
    u16 i;

    // sonic physic
    if (xorder > 0)
    {
        movx += ACCEL;
        // going opposite side, quick breaking
        if (movx < 0) movx += ACCEL;

        if (movx >= MAX_SPEED) movx = MAX_SPEED;
    }
    else if (xorder < 0)
    {
        movx -= ACCEL;
        // going opposite side, quick breaking
        if (movx > 0) movx -= ACCEL;

        if (movx <= -MAX_SPEED) movx = -MAX_SPEED;
    }
    else
    {
        if ((movx < FIX32(0.1)) && (movx > FIX32(-0.1)))
            movx = 0;
        else if ((movx < FIX32(0.3)) && (movx > FIX32(-0.3)))
            movx -= movx >> 2;
        else if ((movx < FIX32(1)) && (movx > FIX32(-1)))
            movx -= movx >> 3;
        else
            movx -= movx >> 4;
    }

    posx += movx;
    posy += movy;

    if (movy)
    {
        if (posy > MAX_POSY)
        {
            posy = MAX_POSY;
            movy = 0;
        }
        else movy += GRAVITY;
    }

    if (posx >= MAX_POSX)
    {
        posx = MAX_POSX;
        movx = 0;
    }
    else if (posx <= MIN_POSX)
    {
        posx = MIN_POSX;
        movx = 0;
    }

  
    fix32 px_scr, py_scr;
    fix32 npx_cam, npy_cam;

    // get sprite position on screen
    px_scr = posx - camposx;
    py_scr = posy - camposy;

    // calculate new camera position
    if (px_scr > FIX32(240)) npx_cam = posx - FIX32(240);
    else if (px_scr < FIX32(40)) npx_cam = posx - FIX32(40);
    else npx_cam = camposx;
    if (py_scr > FIX32(160)) npy_cam = posy - FIX32(160);
    else if (py_scr < FIX32(100)) npy_cam = posy - FIX32(100);
    else npy_cam = camposy;

    // clip camera position
    if (npx_cam < FIX32(0)) npx_cam = FIX32(0);
    else if (npx_cam > FIX32(600)) npx_cam = FIX32(600);
    if (npy_cam < FIX32(0)) npy_cam = FIX32(0);
    else if (npy_cam > FIX32(100)) npy_cam = FIX32(100);

    // set camera position
    updateCamera(npx_cam, npy_cam);

    // set sprites position
    SPR_setPosition(spr_sonic, fix32ToInt(posx - camposx), fix32ToInt(posy - camposy));
  }

static void updateAnim()
{
    // jumping
    if (movy) SPR_setAnim(spr_sonic, ANIM_ROLL);
    else
    {
        if (((movx >= BRAKE_SPEED) && (xorder < 0)) || ((movx <= -BRAKE_SPEED) && (xorder > 0)))
        {
            if (spr_sonic->animInd != ANIM_BRAKE)
            {
                SPR_setAnim(spr_sonic, ANIM_BRAKE);
            }
        }
        else if ((movx >= RUN_SPEED) || (movx <= -RUN_SPEED))
            SPR_setAnim(spr_sonic, ANIM_RUN);
        else if (movx != 0)
            SPR_setAnim(spr_sonic, ANIM_WALK);
        else
        {
            if (yorder < 0)
                SPR_setAnim(spr_sonic, ANIM_UP);
            else if (yorder > 0)
                SPR_setAnim(spr_sonic, ANIM_CROUNCH);
            else
                SPR_setAnim(spr_sonic, ANIM_STAND);
        }
    }

    if (movx > 0) SPR_setHFlip(spr_sonic, FALSE);
    else if (movx < 0) SPR_setHFlip(spr_sonic, TRUE);

}

static void updateCamera(fix32 x, fix32 y)
{
    if ((x != camposx) || (y != camposy))
    {
        camposx = x;
        camposy = y;
        VDP_setHorizontalScroll(PLAN_A, fix32ToInt(-camposx));
        VDP_setVerticalScroll(PLAN_A, fix32ToInt(camposy));
    }
}


static void handleInput()
{
    u16 value = JOY_readJoypad(JOY_1);

    if (value & BUTTON_UP) yorder = -1;
    else if (value & BUTTON_DOWN) yorder = +1;
    else yorder = 0;

    if (value & BUTTON_LEFT) xorder = -1;
    else if (value & BUTTON_RIGHT) xorder = +1;
    else xorder = 0;

}

static void joyEvent(u16 joy, u16 changed, u16 state)
{
    // START button state changed
    if (changed & BUTTON_START)
    {

    }

    if (changed & state & (BUTTON_A | BUTTON_B | BUTTON_C))
    {
        if (movy == 0)
        {
            movy = JUMP_SPEED;
        }
    }
}
