#include <engine/Engine.h>

#define MAX_FS_TESTS 32

typedef struct
{
	const char *name;
	int passed;
} FsTestResult;

static FsTestResult gFsTests[MAX_FS_TESTS];
static int gFsTestCount = 0;
static int gFsErrors = 0;
static int gFsExecuted = 0;

void fs_test_assert(const char *name, int condition)
{
	if (gFsTestCount >= MAX_FS_TESTS)
		return;

	gFsTests[gFsTestCount].name = name;
	gFsTests[gFsTestCount].passed = condition ? 1 : 0;

	if (!condition)
		gFsErrors++;

	gFsTestCount++;
}

void run_fs_full_test(void)
{
	gFsTestCount = 0;
	gFsErrors = 0;

	FS_DIR dir;
	FS_DIRENT ent;
	int found = 0;

	// 1. mkdir
	fs_test_assert("mkdir testdir", fs_mkdir("testdir") == 0);

	// 2. mkdir sub
	fs_test_assert("mkdir sub", fs_mkdir("testdir/sub") == 0);

	// 3. create file
	FS_FILE *f = fs_open("testdir/sub/file.txt", "wb");
	int created = (f != NULL);
	fs_test_assert("create file", created);

	if (f)
	{
		const char *msg = "libGPP";
		fs_write(msg, 1, 6, f);
		fs_close(f);
	}

	// 4. exists file
	fs_test_assert("exists file", fs_exists("testdir/sub/file.txt"));

	// 5. read + validate
	char buffer[16] = { 0 };
	f = fs_open("testdir/sub/file.txt", "rb");
	int valid = 0;
	if (f)
	{
		fs_read(buffer, 1, 6, f);
		fs_close(f);

		if (strcmp(buffer, "libGPP") == 0)
			valid = 1;
	}

	fs_test_assert("read content", valid);

	// 6. readdir
	if (fs_opendir(&dir, "testdir/sub") == 0)
	{
		while (fs_readdir(&dir, &ent) == 0)
		{
			if (strcmp(ent.name, "file.txt") == 0)
				found = 1;
		}
		fs_closedir(&dir);
	}

	fs_test_assert("readdir found file", found);

	// 7. remove file
	fs_test_assert("remove file", fs_remove("testdir/sub/file.txt") == 0);

	// 8. rmdir sub
	fs_test_assert("rmdir sub", fs_rmdir("testdir/sub") == 0);

	// 9. rmdir main
	fs_test_assert("rmdir testdir", fs_rmdir("testdir") == 0);

	gFsExecuted = 1;
}

void draw_fs_results(void)
{
	u32 white = color_rgb(255, 255, 255);
	u32 green = color_rgb(0, 255, 0);
	u32 red = color_rgb(255, 0, 0);

	int y = 30;

	print(20, 10, "Filesystem Test Suite", white);

	for (int i = 0; i < gFsTestCount; i++)
	{
		u32 color = gFsTests[i].passed ? green : red;
		print(20, y, gFsTests[i].name, color);
		y += 15;
	}

	y += 10;

	if (gFsErrors == 0)
		print(20, y, "ALL TESTS PASSED", green);
	else
	{
		char buffer[64];
		snprintf(buffer, sizeof(buffer), "FAILED: %d errors", gFsErrors);
		print(20, y, buffer, red);
	}
}

int main(int argc, char **argv)
{
	if (Init_Sistem("libGPP-Engine FS Test") != 0)
		return 1;

	if (Set_Video() != 0)
		return 1;

	while (1)
	{
		fill_radial_gradient(logic, color_rgb(25, 77, 255), color_rgb(22, 25, 160));

		if (!gFsExecuted)
			run_fs_full_test();

		draw_fs_results();

		Render();

#if defined(ANDROID_BUILD) || defined(PC_BUILD)
		SDL_Delay(16);
#endif
	}

	off_video();
	shoutdown_sistem();
	return 0;
}
