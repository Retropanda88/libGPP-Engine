#include <engine/engine.h>
#include <string.h>

#define MAX_NAME_ITEMS 10

#define FPS 10

//dedinimos dos jugadores
#define player1 0
#define player2 1

static int done = 0;

//pst arrehlo de variables de estado.
/*[0] u, [1] d, [2] l, [3] r, [4] a, [5] b*/
bool pst[6]={false,false,false,false,false,false};
gfxFont font;;
Cmixer mixer;

enum
{ Menu_video,
  Menu_audio, 
  Menu_input, 
  Menu_system, 
  Menu_info, 
  MAX_ITEMS};

// identificadores 
const char itemsName[MAX_NAME_ITEMS][50] = {
	{"Video"},
	{"Audio"},
	{"System"},
	{"Input"},
	{"Info"}
};

struct Item
{
	char name[50];
	SDL_Surface *icon;
};

Item it[MAX_ITEMS];

//funcion para conectar los nombres de los items
void LoadSuiteData(){
	for(int i=0;i<MAX_ITEMS;i++){
		//copiamos los nombres 
		strcpy(it[i].name,itemsName[i]);
		//puntero a null
		it[i].icon = NULL;
	}
}

void initSuit()
{
	if (Init_Sistem("test suit") != 0)
	{
		printf("error al iniciar sistema\n");
	}

	if (Set_Video())
	{
		printf("error al iniciar el sistemabde video\n");
	}
	
	//inicializamos sistema de inputs
	Input::init();
	LoadSuiteData();
	
}

void playerUpdate(){
		pst[0] = Input::isPressed(player1,BUTTON_UP)  || Input::isPressed(player2,BUTTON_UP);
		pst[1] = Input::isPressed(player1,BUTTON_DOWN) || Input::isPressed(player2,BUTTON_DOWN);
		pst[2] = Input::isPressed(player1,BUTTON_A) || Input::isPressed(player2,BUTTON_A);
}

void loopMain()
{
	while (!done)
	{
		Input::update();
        playerUpdate();
		
		Render();
		cls();
		Fps_sincronizar(FPS);
	}
}

void closeSuite()
{
	off_video();
	shoutdown_sistem();
}

int main(int argc, char **argv)
{

	initSuit();
	loopMain();
	closeSuite();

	return 0;
}
