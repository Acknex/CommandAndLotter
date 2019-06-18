
#define DebugMode

var CamFPS;

#include <acknex.h>
#include "framework.h"
#include "map_loader.h"
#include "camera.h"
#include "global.h"
#include "unit.h"
#include "jps.h"
#include "effects2d.h"
#include "spawner.h"

SOUND * unit_management_taunt_generic01 = "taunt-galep-04.ogg";
SOUND * unit_management_taunt_generic02 = "taunt-galep-05.ogg";
SOUND * unit_management_taunt_generic03 = "taunt-jcl-04.ogg";
SOUND * unit_management_taunt_generic04 = "taunt-jcl-05.ogg";
SOUND * unit_management_taunt_generic05 = "taunt-jcl-06.ogg";

SOUND * unit_management_taunt_move01 = "taunt-fritz-01.ogg";
SOUND * unit_management_taunt_move02 = "taunt-fritz-04.ogg";
SOUND * unit_management_taunt_move03 = "taunt-galep-01.ogg";
SOUND * unit_management_taunt_move04 = "taunt-jcl-01.ogg";
SOUND * unit_management_taunt_move05 = "infinity.wav";

SOUND * unit_management_taunt_attack01 = "taunt-fritz-02.ogg";
SOUND * unit_management_taunt_attack02 = "taunt-jcl-02.ogg";
SOUND * unit_management_taunt_attack03 = "taunt-jcl-02.ogg";
SOUND * unit_management_taunt_attack04 = "taunt-jcl-03.ogg";

SOUND * unit_management_taunt_mine01 = "taunt-fritz-02.ogg";
SOUND * unit_management_taunt_mine02 = "taunt-fritz-03.ogg";
SOUND * unit_management_taunt_mine03 = "taunt-galep-03.ogg";
SOUND * unit_management_taunt_mine04 = "taunt-galep-06.ogg";

SOUND * unit_management_taunt_move[9];
SOUND * unit_management_taunt_attack[9];
SOUND * unit_management_taunt_mine[9];

#define UNIT_MANAGEMENT_TAINT_MOVE_COUNT   10
#define UNIT_MANAGEMENT_TAINT_ATTACK_COUNT 9
#define UNIT_MANAGEMENT_TAINT_MINE_COUNT   9

var unit_management_current_taunt_sound;

void UnitMangement_init(){

    unit_management_taunt_move[0] = unit_management_taunt_generic01;
    unit_management_taunt_move[1] = unit_management_taunt_generic02;
    unit_management_taunt_move[2] = unit_management_taunt_generic03;
    unit_management_taunt_move[3] = unit_management_taunt_generic04;
    unit_management_taunt_move[4] = unit_management_taunt_generic05;
    unit_management_taunt_move[5] = unit_management_taunt_move01;
    unit_management_taunt_move[6] = unit_management_taunt_move02;
    unit_management_taunt_move[7] = unit_management_taunt_move03;
    unit_management_taunt_move[8] = unit_management_taunt_move04;
    unit_management_taunt_move[9] = unit_management_taunt_move05;

    unit_management_taunt_attack[0] = unit_management_taunt_generic01;
    unit_management_taunt_attack[1] = unit_management_taunt_generic02;
    unit_management_taunt_attack[2] = unit_management_taunt_generic03;
    unit_management_taunt_attack[3] = unit_management_taunt_generic04;
    unit_management_taunt_attack[4] = unit_management_taunt_generic05;
    unit_management_taunt_attack[5] = unit_management_taunt_attack01;
    unit_management_taunt_attack[6] = unit_management_taunt_attack02;
    unit_management_taunt_attack[7] = unit_management_taunt_attack03;
    unit_management_taunt_attack[8] = unit_management_taunt_attack04;

    unit_management_taunt_mine[0] = unit_management_taunt_generic01;
    unit_management_taunt_mine[1] = unit_management_taunt_generic02;
    unit_management_taunt_mine[2] = unit_management_taunt_generic03;
    unit_management_taunt_mine[3] = unit_management_taunt_generic04;
    unit_management_taunt_mine[4] = unit_management_taunt_generic05;
    unit_management_taunt_mine[5] = unit_management_taunt_mine01;
    unit_management_taunt_mine[6] = unit_management_taunt_mine02;
    unit_management_taunt_mine[7] = unit_management_taunt_mine03;
    unit_management_taunt_mine[8] = unit_management_taunt_mine04;
}





void UnitMangement_open(){
    unit_management_current_taunt_sound = 0;
}


var ClickPoint2D_A[2];
var ClickPoint2D_B[2];



function PosToMap(VECTOR * vec, var x, var y)
{
    VECTOR temp;
    vec_set(temp, vector(x,y, camera.clip_far));
    if(vec_for_screen(temp,camera) != 0)
    {
        VECTOR * p = maploader_trace(camera.x, temp.x);
        if(p){
            vec_set(vec,p);
            return 1;
        }
    }
    return 0;
}

function CheckIsLeftFrom(VECTOR* Base, VECTOR* V1, VECTOR * V2)
{
    VECTOR Line1;
    VECTOR Line2;
    VECTOR Cross;
    vec_diff(Line1,Base, V1);
    vec_diff(Line2,Base, V2);



    vec_normalize (Line1, 1);
    vec_normalize (Line2, 1);

    vec_cross(Cross, Line1, Line2);
    if(Cross.z < 0){
        return 1;
    }
}

#define SEL_AMBIENT 40

function DeselectUnit(ENTITY * ent)
{
    ent.SELCTED_SKILL = 0;
    ent.ambient = 0;
}

function SelectUnit(ENTITY * ent)
{
    ent.SELCTED_SKILL = 1;
    ent.ambient = SEL_AMBIENT;
}

function SetUnitSelcted(ENTITY* ent, var isSelected)
{
    if(isSelected){
        SelectUnit(ent);
    }else{
        DeselectUnit(ent);
    }
}

function MarkUnits()
{
    VECTOR Posis[4];

    var success = 0;
    success += PosToMap(Posis[0], maxv(ClickPoint2D_A[0],ClickPoint2D_B[0]), minv(ClickPoint2D_A[1],ClickPoint2D_B[1]));
    success += PosToMap(Posis[1], maxv(ClickPoint2D_A[0],ClickPoint2D_B[0]), maxv(ClickPoint2D_A[1],ClickPoint2D_B[1]));
    success += PosToMap(Posis[2], minv(ClickPoint2D_A[0],ClickPoint2D_B[0]), maxv(ClickPoint2D_A[1],ClickPoint2D_B[1]));
    success += PosToMap(Posis[3], minv(ClickPoint2D_A[0],ClickPoint2D_B[0]), minv(ClickPoint2D_A[1],ClickPoint2D_B[1]));

    if(success != 4){return;}

    var i;

    ENTITY * ent;
    for(ent = ent_next(NULL); ent != NULL; ent = ent_next(ent)){
        if(ent->group==GROUP_PLAYER_UNIT && ent->HEALTH > 0){
            var isInside = 1;
            for(i = 0; i < 4 && isInside; i++){
                isInside = CheckIsLeftFrom(Posis[i],Posis[(i+1)%4],ent.x);
            }
            if(isInside || (key_shiftl && ent.SELCTED_SKILL)){
                ent.ambient = SEL_AMBIENT;
            }else{
               ent.ambient = 0;
            }
        }
    }
}


function SelectMarkedUnits()
{
    ENTITY * ent;
    for(ent = ent_next(NULL); ent != NULL; ent = ent_next(ent)){
        if(ent->group==GROUP_PLAYER_UNIT){
            SetUnitSelcted(ent, ent.ambient > 0);
        }
     }
}



function DrawQuadDemo(){
    draw_line(vector(maxv(ClickPoint2D_A[0],ClickPoint2D_B[0]),minv(ClickPoint2D_A[1],ClickPoint2D_B[1]),0),NULL,100);
    draw_line(vector(maxv(ClickPoint2D_A[0],ClickPoint2D_B[0]),maxv(ClickPoint2D_A[1],ClickPoint2D_B[1]),0),vector(255,255,255),80);
    draw_line(vector(minv(ClickPoint2D_A[0],ClickPoint2D_B[0]),maxv(ClickPoint2D_A[1],ClickPoint2D_B[1]),0),vector(255,255,255),80);
    draw_line(vector(minv(ClickPoint2D_A[0],ClickPoint2D_B[0]),minv(ClickPoint2D_A[1],ClickPoint2D_B[1]),0),vector(255,255,255),80);
    draw_line(vector(maxv(ClickPoint2D_A[0],ClickPoint2D_B[0]),minv(ClickPoint2D_A[1],ClickPoint2D_B[1]),0),vector(255,255,255),80);
}


var MouseLeftLast = 0;
var MouseRightLast = 0;

function SetDestForSelectd(VECTOR * Dest)
{
    int Count = 0;
    ENTITY * ent;
    for(ent = ent_next(NULL); ent != NULL; ent = ent_next(ent)){
        if(ent->group==GROUP_PLAYER_UNIT && ent.SELCTED_SKILL){
            unit_setTarget(ent, Dest);
            Count++;
        }
    }
    return Count;
}


function SetVictimForSelectd(ENTITY * Victim)
{
    int Count = 0;
    ENTITY * ent;
    for(ent = ent_next(NULL); ent != NULL; ent = ent_next(ent)){
        if(ent->group==GROUP_PLAYER_UNIT && ent.SELCTED_SKILL){
            unit_setVictim(ent, Victim);
            Count++;
        }
    }
    return Count;
}


function DebugDrawDests()
{
    ENTITY * ent;
    SUBSYSTEM_LOOP(ent, SUBSYSTEM_UNIT_MANAGEMENT){
        draw_line3d(ent.x, NULL, 100);
        draw_line3d(ent.x, vector(0,255,0), 50);
        draw_line3d(ent.UNIT_DEST_SKILL, vector(0,255,0), 50);
    }
}

function DeselectAllOfSubsystem(int Subsys)
{
    ENTITY * ent;
    SUBSYSTEM_LOOP(ent, Subsys){
        DeselectUnit(ent);
    }
}

function DeselectAllOfGroup(int Group)
{
    ENTITY * ent;
    for(ent = ent_next(NULL); ent != NULL; ent = ent_next(ent)){
        if(ent->group==Group){
            DeselectUnit(ent);
        }
     }
}

function NumberKeyPressed(int nr)
{
    var x = 0;
    var y = 0;
    var count = 0;

    ENTITY * ent;
    for(ent = ent_next(NULL); ent != NULL; ent = ent_next(ent)){
        if(ent->group==GROUP_PLAYER_UNIT){
            if(key_ctrl){
                if(ent.SELCTED_SKILL){
                    ent.UNIT_GROUP_SKILL = nr;
                }else{
                    if(ent.UNIT_GROUP_SKILL == nr){
                        ent.UNIT_GROUP_SKILL = 0;
                    }
                }
            }else{
                SetUnitSelcted(ent, ent.UNIT_GROUP_SKILL == nr);
                if(ent.UNIT_GROUP_SKILL == nr){
                    x+=ent.x;
                    y+=ent.y;
                    count++;
                }

            }
        }
    }
    if(!key_ctrl && key_alt && count){//Gruppe mit Alt-Ausgew?hlt die auch Units enth?lt
        x /= count;
        y /= count;
        topdown_camera_set_pos(vector(x,y,0));
    }
}

var NuberKeyPressedLast = 0;

function UnitControl()
{
    VECTOR temp;
    mouse_mode = 4;
    if(mouse_left){
        if(MouseLeftLast == 0 ){

        	if(mouse_panel == 0){
	            if(!key_shiftl){
	                DeselectAllOfGroup(GROUP_PLAYER_UNIT);
	               // DeselectAllOfGroup(GROUP_PLAYER_SPAWNER);

	                //DeselectAllOfGroup(GROUP_ENEMY_SPAWNER);
	            }
	            DeselectAllOfGroup(GROUP_ENEMY_UNIT);
	            DeselectAllOfSubsystem(SUBSYSTEM_SPAWNER);

                vec_set(temp, vector(mouse_pos.x,mouse_pos.y, camera.clip_far));
                vec_for_screen(temp,camera);
                c_trace(camera.x, temp,USE_POLYGON | IGNORE_PASSENTS | IGNORE_SPRITES);
                if(you != 0){
                         if(you->group==GROUP_PLAYER_UNIT || you->group==GROUP_PLAYER_SPAWNER || you->group==GROUP_ENEMY_UNIT || you->group==GROUP_ENEMY_SPAWNER){

                             // only select units that are alive. everything
                             // else doesn't make sense
                            if(you->HEALTH > 0)
                            {
                                if(fow_isPosVisible(target.x))
                                {
                                    SelectUnit(you);   
                                }
                            }
                        }
                 }
            }

            ClickPoint2D_A[0]= ClickPoint2D_B[0] = mouse_pos.x;
            ClickPoint2D_A[1]= ClickPoint2D_B[1] = mouse_pos.y;

        }else{
            ClickPoint2D_B[0]=mouse_pos.x;
            ClickPoint2D_B[1]=mouse_pos.y;
        }
        if(abs(ClickPoint2D_A[0]-ClickPoint2D_B[0]) > 2 && abs(ClickPoint2D_A[1]-ClickPoint2D_B[1])> 2){
            DrawQuadDemo();
            MarkUnits();
        }
    }else{
       if(MouseLeftLast){
            SelectMarkedUnits();
       }
    }
    if(mouse_right && !MouseRightLast){
        var CmdType = EFFECTS2D_TYPE_GOTO;

        if(mouse_panel == 0){
            vec_set(temp, vector(mouse_pos.x,mouse_pos.y, camera.clip_far));
            vec_for_screen(temp,camera);
            c_trace(camera.x, temp,USE_POLYGON | IGNORE_PASSENTS);
            if(you != 0){
                 if(you->group==GROUP_ENEMY_UNIT  || you->group==GROUP_NEUTRAL_UNIT  || you->group == GROUP_ENEMY_SPAWNER){
//                    SetVictimForSelectd(you); //will be overwritten here
                    CmdType = EFFECTS2D_TYPE_ATTACK;
                    if(unit_getType(you) == UNIT_Z){
                       CmdType = EFFECTS2D_TYPE_MINE;
                    }


                }
            }
        }
        VECTOR Dest;
        PosToMap(Dest,mouse_pos.x,mouse_pos.y);
        if(SetDestForSelectd(Dest) > 0){

            SOUND ** group;
            int count;

            switch(CmdType)
            {
            case EFFECTS2D_TYPE_GOTO:
                group = unit_management_taunt_move;
                count = UNIT_MANAGEMENT_TAINT_MOVE_COUNT;
                break;
            case EFFECTS2D_TYPE_ATTACK:
                group = unit_management_taunt_attack;
                count = UNIT_MANAGEMENT_TAINT_ATTACK_COUNT;
                SetVictimForSelectd(you); //victim must be set after dest, as setting dest resets victim by default
                break;
            case EFFECTS2D_TYPE_MINE:
                group = unit_management_taunt_mine;
                count = UNIT_MANAGEMENT_TAINT_MINE_COUNT;
                SetVictimForSelectd(you); //victim must be set after dest, as setting dest resets victim by default
                break;
            }

            int sndid = (int)random(count);

            if(unit_management_current_taunt_sound == 0)
                unit_management_current_taunt_sound = snd_play(group[sndid], 100, 0);

            effects2d_spawn(Dest, CmdType);
        }
    }

    int i;
    var NuberPressed = 0;
    for(i = 2; i < 10; i++){
        if(key_pressed(i) && !NuberKeyPressedLast){
            NumberKeyPressed(i-1);
            NuberPressed = i-1;
        }
    }
    NuberKeyPressedLast = NuberPressed;
    MouseLeftLast = mouse_left;
    MouseRightLast = mouse_right;
}



void UnitMangement_update()
{
    UnitControl();
   // DebugDrawDests();

    if(unit_management_current_taunt_sound != 0)
    {
        if(!snd_playing(unit_management_current_taunt_sound))
            unit_management_current_taunt_sound = 0;
    }
}

void UnitMangement_close(){
    if(unit_management_current_taunt_sound != 0)
        snd_stop(unit_management_current_taunt_sound);
}

void UnitMangement_unselect(ENTITY* ent)
{
	if (ent) ent->SELCTED_SKILL = 0;
}
