#include <engine/Engine.h>
#include <stdio.h>

int main(int argc, char **argv)
{
    if (Init_Sistem(NULL) < 0) {
        printf("error: system not found\n");
        return 1;
    }

    if (Set_Video() < 0) {
        printf("error: video system not found\n");
        return 1;
    }

    int ret = fs_init();

    if (ret < 0) {
        while (1){
            u32 color = color_rgb(255,0,0);
            print_f(10,100,color,"irx not load error %d....",ret);
            Render();
        }
        return 1;
    }

    FS_FILE *file;
    char buffer[128];
    int bytes;

    file = fs_open("test.txt", "r");
    if (!file) {
        printf("FS error: cannot open file\n");
        while (1){
            u32 color = color_rgb(255,0,0);
            print(10,80,"FS error: cannot open file",color);
            Render();
        }
    }

   /* printf("=== File content ===\n");

    while ((bytes = fs_read(buffer, 1, sizeof(buffer) - 1, file)) > 0) {
        buffer[bytes] = '\0';
        printf("%s", buffer);
    }

    printf("\n=== End of file ===\n");

    fs_close(file);*/



    off_video();
    shoutdown_sistem();
    return 0;
}
