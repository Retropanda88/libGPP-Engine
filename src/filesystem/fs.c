#include <string.h>
#include <stdlib.h>

#include <engine/types.h>
#include <filesystem/fs.h>
#include <filesystem/path_prefix.h>

/* includes por plataforma */
#if defined(PS2_BUILD)
#include <kernel.h>
#include <sifrpc.h>
#include <loadfile.h>
#include <fileio.h>
#include <tamtypes.h>
#include <iopcontrol.h>
#include <io_common.h>
#include <stdio.h>

//drivers irx 
#include <filesystem/usbd.h>
#include <filesystem/usbhdfsd.h>


#elif defined(PSP_BUILD)
#include <pspiofilemgr.h>

#elif defined(GC_BUILD) || defined(PC_BUILD) || defined(ANDROID_BUILD)
#include <stdio.h>
#endif

/* ---------------------------------------------------- */
/* inicialización del filesystem */
/* ---------------------------------------------------- */
int fs_init(void)
{
#if defined(PS2_BUILD)

    int ret;

#ifdef PS2_DEBUG
    /* ================================
     * DEBUG (ps2link)
     * NO resetear IOP
     * ================================ */
    SifInitRpc(0);

#else
    /* ================================
     * RELEASE (app final)
     * Reset limpio del IOP
     * ================================ */
    SifInitRpc(0);

    if (SifIopReset(NULL, 0) < 0) {
        printf("FS: SifIopReset failed\n");
        return -1;
    }

    while (!SifIopSync());

    /* RPC debe reiniciarse después del reset */
    SifInitRpc(0);
#endif

    /* ================================
     * Inicializar File I/O
     * ================================ */
    ret = fioInit();
    if (ret < 0) {
        printf("FS: fioInit failed (%d)\n", ret);
        return -1;
    }

    /* ================================
     * Cargar módulos USB desde memoria
     * ================================ */
	SifExecModuleBuffer((void*)usbd_irx, usbd_irx_size, 0, NULL, &ret);
	if (ret < 0) {
	    printf("FS: failed to load usbd.irx (%d)\n", ret);
	    return -1;
	}

	SifExecModuleBuffer((void*)usbhdfsd_irx, usbhdfsd_irx_size, 0, NULL, &ret);
	if (ret < 0) {
	    printf("FS: failed to load usbhdfsd.irx (%d)\n", ret);
	    return -2;
	}

    printf("FS: initialized successfully\n");
    return 0;

#else
    return 0;
#endif
}


/* ---------------------------------------------------- */
/* construcción de ruta completa */
/* ---------------------------------------------------- */
const char *fs_make_path(const char *relative)
{
	static char full_path[256];

	if (!relative)
		return 0;

	strcpy(full_path, PREFIX);
	strcat(full_path, relative);

	return full_path;
}

/* ---------------------------------------------------- */
/* abrir archivo */
/* ---------------------------------------------------- */

FS_FILE *fs_open(const char *path, const char *mode)
{
	FS_FILE *f = (FS_FILE *) malloc(sizeof(FS_FILE));
	if (!f)
		return NULL;

	f->handle = NULL;
	f->pos = 0;
	f->size = 0;

	const char *full_path = fs_make_path(path);
	if (!full_path)
	{
		free(f);
		return NULL;
	}

#if defined(PS2_BUILD)

	int flags = 0;

	if (!strcmp(mode, "r") || !strcmp(mode, "rb"))
		flags = O_RDONLY;
	else if (!strcmp(mode, "w") || !strcmp(mode, "wb"))
		flags = O_WRONLY | O_CREAT | O_TRUNC;
	else if (!strcmp(mode, "a") || !strcmp(mode, "ab"))
		flags = O_WRONLY | O_CREAT | O_APPEND;
	else
	{
		free(f);
		return NULL;
	}

	int *fd = (int *)malloc(sizeof(int));
	if (!fd)
	{
		free(f);
		return NULL;
	}

	*fd = fioOpen(full_path, flags);
	if (*fd < 0)
	{
		free(fd);
		free(f);
		return NULL;
	}

	if (flags & O_APPEND)
		f->pos = fioLseek(*fd, 0, SEEK_END);
	else
	{
		int end = fioLseek(*fd, 0, SEEK_END);
		fioLseek(*fd, 0, SEEK_SET);
		f->size = (u32) end;
	}

	f->handle = fd;

#elif defined(PSP_BUILD)

	int flags = 0;

	if (!strcmp(mode, "r") || !strcmp(mode, "rb"))
		flags = PSP_O_RDONLY;
	else if (!strcmp(mode, "w") || !strcmp(mode, "wb"))
		flags = PSP_O_WRONLY | PSP_O_CREAT | PSP_O_TRUNC;
	else if (!strcmp(mode, "a") || !strcmp(mode, "ab"))
		flags = PSP_O_WRONLY | PSP_O_CREAT | PSP_O_APPEND;
	else
	{
		free(f);
		return NULL;
	}

	int *fd = (int *)malloc(sizeof(int));
	if (!fd)
	{
		free(f);
		return NULL;
	}

	*fd = sceIoOpen(full_path, flags, 0777);
	if (*fd < 0)
	{
		free(fd);
		free(f);
		return NULL;
	}

	if (flags & PSP_O_APPEND)
		f->pos = sceIoLseek(*fd, 0, PSP_SEEK_END);
	else
	{
		int end = sceIoLseek(*fd, 0, PSP_SEEK_END);
		sceIoLseek(*fd, 0, PSP_SEEK_SET);
		f->size = (u32) end;
	}

	f->handle = fd;

#elif defined(GC_BUILD)

	int flags = 0;

	if (!strcmp(mode, "r") || !strcmp(mode, "rb"))
		flags = O_RDONLY;
	else if (!strcmp(mode, "w") || !strcmp(mode, "wb"))
		flags = O_WRONLY | O_CREAT | O_TRUNC;
	else if (!strcmp(mode, "a") || !strcmp(mode, "ab"))
		flags = O_WRONLY | O_CREAT | O_APPEND;
	else
	{
		free(f);
		return NULL;
	}

	int *fd = (int *)malloc(sizeof(int));
	if (!fd)
	{
		free(f);
		return NULL;
	}

	*fd = open(full_path, flags, 0666);
	if (*fd < 0)
	{
		free(fd);
		free(f);
		return NULL;
	}

	if (flags & O_APPEND)
		f->pos = lseek(*fd, 0, SEEK_END);
	else
	{
		int end = lseek(*fd, 0, SEEK_END);
		lseek(*fd, 0, SEEK_SET);
		f->size = (u32) end;
	}

	f->handle = fd;

#else /* PC / ANDROID */

	FILE *fp = fopen(full_path, mode);
	if (!fp)
	{
		free(f);
		return NULL;
	}

	fseek(fp, 0, SEEK_END);
	f->size = (u32) ftell(fp);

	if (mode[0] == 'a')
		f->pos = f->size;
	else
		fseek(fp, 0, SEEK_SET);

	f->handle = fp;

#endif

	return f;
}

/* ---------------------------------------------------- */
/* lectura estilo fread */
/* ---------------------------------------------------- */

int fs_read(void *ptr, u32 size, u32 count, FS_FILE *stream)
{
	if (!stream || !ptr || size == 0 || count == 0)
		return 0;

	u32 bytes = size * count;

#if defined(PS2_BUILD)

	int fd = *(int *)stream->handle;
	int r = fioRead(fd, ptr, bytes);
	if (r <= 0)
		return 0;

	stream->pos += r;
	return r / size;

#elif defined(PSP_BUILD)

	int fd = *(int *)stream->handle;
	int r = sceIoRead(fd, ptr, bytes);
	if (r <= 0)
		return 0;

	stream->pos += r;
	return r / size;

#elif defined(GC_BUILD)

	int fd = *(int *)stream->handle;
	int r = read(fd, ptr, bytes);
	if (r <= 0)
		return 0;

	stream->pos += r;
	return r / size;

#else /* PC / ANDROID */

	FILE *fp = (FILE *)stream->handle;
	size_t r = fread(ptr, size, count, fp);

	stream->pos = (u32)ftell(fp);
	return (int)r;

#endif
}
/* ---------------------------------------------------- */
/* mover cursor estilo fseek */
/* ---------------------------------------------------- */

int fs_seek(FS_FILE *stream, s32 offset, int origin)
{
	if (!stream || !stream->handle)
		return -1;

#if defined(PS2_BUILD)

	int fd = *(int *)stream->handle;
	int r = fioLseek(fd, offset, origin);
	if (r >= 0)
		stream->pos = r;
	return r;

#elif defined(PSP_BUILD)

	int fd = *(int *)stream->handle;
	int r = sceIoLseek(fd, offset, origin);
	if (r >= 0)
		stream->pos = r;
	return r;

#elif defined(GC_BUILD)

	int fd = *(int *)stream->handle;
	int r = lseek(fd, offset, origin);
	if (r >= 0)
		stream->pos = r;
	return r;

#else /* ANDROID + PC */

	FILE *fp = (FILE *)stream->handle;
	int r = fseek(fp, offset, origin);
	if (r == 0)
		stream->pos = (u32)ftell(fp);
	return r;

#endif
}

/* ---------------------------------------------------- */
/* posición actual */
/* ---------------------------------------------------- */
u32 fs_tell(FS_FILE * stream)
{
	if (!stream)
		return 0;

	return stream->pos;
}

/* ---------------------------------------------------- */
/* cerrar archivo */
/* ---------------------------------------------------- */

void fs_close(FS_FILE *stream)
{
	if (!stream)
		return;

#if defined(PS2_BUILD)

	if (stream->handle) {
		int fd = *(int *)stream->handle;
		fioClose(fd);
		free(stream->handle);
	}

#elif defined(PSP_BUILD)

	if (stream->handle) {
		int fd = *(int *)stream->handle;
		sceIoClose(fd);
		free(stream->handle);
	}

#elif defined(GC_BUILD)

	if (stream->handle) {
		int fd = *(int *)stream->handle;
		close(fd);
		free(stream->handle);
	}

#else /* PC / ANDROID */

	if (stream->handle)
		fclose((FILE *)stream->handle);

#endif

	free(stream);
}