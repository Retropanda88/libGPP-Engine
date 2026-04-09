#include <engine/Engine.h>

#define COLOR_WHITE  0xFFFFFFFF
#define COLOR_GREEN  0xFF00FF00
#define COLOR_RED    0xFFFF0000
#define COLOR_YELLOW 0xFFFFFF00

#define TEST_FILE_TXT "test_text.bin"
#define TEST_FILE_BIG "test_big.bin"

int main(int argc, char **argv)
{
    if (Init_Sistem("Filesystem Full Test") < 0)
        return 1;

    if (Set_Video() < 0)
        return 1;
    
    int result_ok = 1;
    int seek_ok = 0;
    int big_ok = 0;

    /* ========================= */
    /* 1️⃣ Escritura en bloques  */
    /* ========================= */

    const char data1[] = "BloqueA";
    const char data2[] = "BloqueB";
    const char data3[] = "BloqueC";

    FS_FILE *f = fs_open(TEST_FILE_TXT, "ab");
    if (!f) result_ok = 0;

    u32 w1 = fs_write(data1, 1, sizeof(data1), f);
    u32 w2 = fs_write(data2, 1, sizeof(data2), f);
    u32 w3 = fs_write(data3, 1, sizeof(data3), f);
    fs_close(f);

    /* ========================= */
    /* 2️⃣ Probar SEEK           */
    /* ========================= */

    f = fs_open(TEST_FILE_TXT, "rb");
    if (!f) result_ok = 0;

    fs_seek(f, sizeof(data1), SEEK_SET);

    char buffer[32] = {0};
    fs_read(buffer, 1, sizeof(data2), f);
    fs_close(f);

    if (memcmp(buffer, data2, sizeof(data2)) == 0)
        seek_ok = 1;

    /* ========================= */
    /* 3️⃣ Archivo grande 1MB    */
    /* ========================= */

    const int big_size = 1024 * 1024;
    static unsigned char big_buffer[4096];

    for (int i = 0; i < 4096; i++)
        big_buffer[i] = i & 0xFF;

    f = fs_open(TEST_FILE_BIG, "wb");
    if (!f) result_ok = 0;

    int written = 0;
    while (written < big_size)
    {
        fs_write(big_buffer, 1, sizeof(big_buffer), f);
        written += sizeof(big_buffer);
    }

    fs_close(f);

    /* Verificar tamaño */
    f = fs_open(TEST_FILE_BIG, "rb");
    fs_seek(f, 0, SEEK_END);
    u32 size = fs_tell(f);
    fs_close(f);

    if (size == big_size)
        big_ok = 1;

    /* ========================= */
    /* LOOP VISUAL               */
    /* ========================= */

    while (1)
    {
        cls();

        int y = 40;

        print(40, y, "Filesystem Full Test", COLOR_WHITE);
        y += 40;

        print_f(40, y, COLOR_YELLOW,
                "Escritura bloques: %d bytes",
                w1 + w2 + w3);
        y += 30;

        print_f(40, y,
                seek_ok ? COLOR_GREEN : COLOR_RED,
                "Seek test: %s",
                seek_ok ? "OK" : "FAIL");
        y += 30;

        print_f(40, y,
                big_ok ? COLOR_GREEN : COLOR_RED,
                "Archivo 5MB: %s",
                big_ok ? "OK" : "FAIL");
        y += 30;

        print_f(40, y,
                (result_ok && seek_ok && big_ok) ? COLOR_GREEN : COLOR_RED,
                "RESULTADO FINAL: %s",
                (result_ok && seek_ok && big_ok) ? "TEST OK" : "ERROR");
        y += 40;

        Render();
    }

    return 0;
}