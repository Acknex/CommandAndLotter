
#define DebugMode

var CamFPS;

#include <acknex.h>
#include "framework.h"
#include "map_loader.h"
#include "camera.h"
#include "global.h"

#ifdef DebugMode
    var test1; //für debugzwecke
    var test2;
    var test3;
    var test4;
    var test5;
    var test6;
    var test7;
    var test8;
    var test9;

    STRING* DebgStr1 = "";
    STRING* DebgStr2 = "";




PANEL* DbgDebugPan =
{
    pos_x = 4;
    pos_y = 12;
    layer = 998;

    size_x = 450;
    size_y = 120;
    red = 55;
    blue = 200;
    green = 113;
    alpha = 90;
    flags = LIGHT | TRANSLUCENT | OUTLINE;

    digits(0,2,"%4.0f x",*,1,screen_size.x);
    digits(30,2,"%.0f y",*,1,screen_size.y);
    digits(0,12,"fps%5.0f",*,16,def_dfps);
    digits(0,22,"pps%5.0fk",*,0.016,def_dtps);
    digits(0,32,"x%7.0f",*,1,camera.x);
    digits(0,42,"y%7.0f",*,1,camera.y);
    digits(0,52,"z%7.0f",*,1,camera.z);
    digits(0,62,"pan%5.0f",*,1,camera.pan);
    digits(0,72,"til%5.0f",*,1,camera.tilt);
    digits(0,82,"rol%5.0f",*,1,camera.roll);
    digits(0,92,"CamFps%5.0f",*,1,CamFPS);

//    digits(0,92,"ml %5.f",*,1,mouse_right);

    digits(95,2,"ms/frame",*,0,0);
    digits(95,22,"ent%5.1f",*,1,time_entities);
    digits(95,12,"geo%5.1f",*,1,def_dtlv);
    digits(95,32,"par%5.1f",*,1,time_effects);
    digits(95,42,"mir%5.1f",*,1,time_portals);
    digits(95,52,"pan%5.1f",*,1,time_panels);
    digits(95,62,"ref%5.1f",*,1,time_update);
    digits(95,72,"srv%5.1f",*,1,def_dtcs);
    digits(95,82,"fnc%5.1f",*,1,def_dtac);
    digits(95,92,"phy%5.1f",*,1,time_physics);

    digits(180,2,"count of",*,0,0;
    digits(180,12,"ent%5.0f",*,1,num_entities);
    digits(180,22,"vis%5.0f",*,1,num_visents);
    digits(180,32,"tri%5.0fk",*,0.001,num_vistriangles);
    digits(180,42,"par%5.0f",*,1,num_particles);
    digits(180,52,"lit%5.0f",*,1,num_lights);
    digits(180,62,"med%5.0f",*,1,num_mediastreams);
    digits(180,72,"fnc%5.0f",*,1,num_actions);
    digits(180,82,"bdy%5.0f",*,1,ph_num_bodies);
    digits(180,92,"con%5.0f",*,1,ph_num_contacts);

    digits(250,2,"memory kb",*,0,0);
    digits(250,12,"nex%6.0f",*,1,nexus);
    digits(250,22,"geo%6.0f",*,1,d3d_texsurfs);
    digits(250,32,"sha%6.0f",*,1,d3d_texsmaps);
    digits(250,42,"ent%6.0f",*,1,d3d_texskins);
    digits(250,52,"bmp%6.0f",*,1,d3d_texbmaps);
    digits(250,62,"fr%7.0f",*,1,d3d_texfree);

//	digits(270,2,"network",*,0,0);
//	digits(270,12,"lat%4.0f",*,1,dplay_latency);
//	digits(270,22,"bps%4.0f",*,1,dplay_bps);
//	digits(270,32,"bpk%4.0f",*,1,dplay_bpspeak);
//	digits(270,42,"rel%4.0f",*,1,dplay_bpsreliable);
//	digits(270,52,"unr%4.0f",*,1,dplay_bpsunreliable);
//	digits(270,62,"drp%4.0f",*,1,dplay_dropped);

        //Testvariablen
    digits(320,2,"Test1: %-9.3f",*,1,test1);
    digits(320,12,"Test2: %-9.3f",*,1,test2);
    digits(320,22,"Test3: %-9.3f",*,1,test3);

    digits(320,32,"Test4: %-9.3f",*,1,test4);
    digits(320,42,"Test5: %-9.3f",*,1,test5);
    digits(320,52,"Test6: %-9.3f",*,1,test6);

    digits(320,62,"Test7: %-9.3f",*,1,test7);
    digits(320,72,"Test8: %-9.3f",*,1,test8);
    digits(320,82,"Test9: %-9.3f",*,1,test9);


    digits(0,102,"Str1: %s",*,1,DebgStr1);
    digits(0,112,"Str2: %s",*,1,DebgStr2);

}

function DbgToggleDebugpanel()
{
    toggle(DbgDebugPan,SHOW);
}

#endif

void UnitMangement_init(){
    #ifdef DebugMode
        on_f12 = DbgToggleDebugpanel;
    #endif
}





void UnitMangement_open(){

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

#define SEL_AMBIENT 255

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
        if(ent.group==GROUP_PLAYER_UNIT){
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
    ENTITY * ent;
    for(ent = ent_next(NULL); ent != NULL; ent = ent_next(ent)){
        if(ent->group==GROUP_PLAYER_UNIT && ent.SELCTED_SKILL){
            unit_setTarget(ent, Dest);
        }
    }
}

VECTOR UnitTempVec;



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
    if(!key_ctrl && key_alt && count){//Gruppe mit Alt-Ausgewählt die auch Units enthält
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
        if(MouseLeftLast == 0){
            if(!key_shiftl){
                DeselectAllOfGroup(GROUP_PLAYER_UNIT);
            }
            DeselectAllOfSubsystem(SUBSYSTEM_SPAWNER);

            if(mouse_panel == 0){
                vec_set(temp, vector(mouse_pos.x,mouse_pos.y, camera.clip_far));
                vec_for_screen(temp,camera);
                c_trace(camera.x, temp,USE_POLYGON | IGNORE_PASSENTS);
                if(you != 0){
                     if(you->group==GROUP_PLAYER_UNIT || you->group==GROUP_PLAYER_SPAWNER){
                        SelectUnit(you);
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

        VECTOR Dest;
        PosToMap(Dest,mouse_pos.x,mouse_pos.y);
        SetDestForSelectd(Dest);
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
}

bool UnitMangement_is_done(){

}

void UnitMangement_close(){

}
