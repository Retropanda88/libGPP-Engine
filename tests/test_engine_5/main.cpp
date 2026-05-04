#include <engine/engine.h>
#include <string.h>

static float x;
static float y;

int main(int argc, char **argv){
    
    if(Init_Sistem("hello libGPP-Engine")<0)
        return 1;

    if(Set_Video()<0)
        return 0;

    startup();

    x = 0;
    y = 0;

    float vel = 0.3f; 

    float dy = vel;
    float dx = vel;
    
    int size;
    const char *msg="hello libGPP-Engine"; 
    size = strlen(msg);

    u32 white = color_rgb(255,255,255);
    u32 black = color_rgb(0,0,0);

    while(1){
        cls_rgb(0,0,255);
        
        print(((int)x)-4, ((int)y)-4,msg,black);
        print((int)x, (int)y,msg,white);
        
        Render();
        Fps_sincronizar(10);

        //desplazamiento vertical
        y+=dy;
        if(y<0)dy = vel;
        if(y>(240-8))dy = -vel;

        x+=dx;
        if(x<0)dx = vel;
        if(x>(320-(size*8)))dx = -vel;
    }

    return 0;
}