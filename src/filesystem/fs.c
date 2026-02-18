

#include <string.h>
#include <stdlib.h>
#include <stdio.h>


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
#include <stdint.h>



// drivers irx 
#include <filesystem/usbd.h>
#include <filesystem/usbhdfsd.h>


#elif defined(PSP_BUILD)
#include <pspiofilemgr.h>

#elif defined(GC_BUILD) || defined(PC_BUILD) || defined(ANDROID_BUILD)
#include <stdio.h>
#include <dirent.h>
#include <unistd.h>
#include <sys/stat.h>
#endif

/* ---------------------------------------------------- */
/* inicialización del filesystem */
/* ---------------------------------------------------- */
int fs_init(void)
{
#if defined(PS2_BUILD)

	int ret;

#ifdef PS2_DEBUG
	/* ================================ DEBUG (ps2link) NO resetear IOP
	   ================================ */
	SifInitRpc(0);

#else
	/* ================================ RELEASE (app final) Reset limpio del
	   IOP ================================ */
	SifInitRpc(0);

	if (SifIopReset(NULL, 0) < 0)
	{
		printf("FS: SifIopReset failed\n");
		return -1;
	}

	while (!SifIopSync());

	/* RPC debe reiniciarse después del reset */
	SifInitRpc(0);
#endif

	/* ================================ Inicializar File I/O
	   ================================ */
	ret = fioInit();
	if (ret < 0)
	{
		printf("FS: fioInit failed (%d)\n", ret);
		return -1;
	}

	/* ================================ Cargar módulos USB desde memoria
	   ================================ */
	SifExecModuleBuffer((void *)usbd_irx, usbd_irx_size, 0, NULL, &ret);
	if (ret < 0)
	{
		printf("FS: failed to load usbd.irx (%d)\n", ret);
		return -1;
	}

	SifExecModuleBuffer((void *)usbhdfsd_irx, usbhdfsd_irx_size, 0, NULL, &ret);
	if (ret < 0)
	{
		printf("FS: failed to load usbhdfsd.irx (%d)\n", ret);
		return -2;
	}

#if !defined(PS2_DEBUG)
	DelayThread(100 * 1000);
#endif

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
        return NULL;

    snprintf(full_path, sizeof(full_path),
             "%s%s", PREFIX, relative);

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

    FILE *fp = fopen(full_path, mode);
    if (!fp)
    {
        free(f);
        return NULL;
    }

    f->handle = (void*)fp;
    f->pos = 0;

    /* Obtener tamaño del archivo */
    fseek(fp, 0, SEEK_END);
    f->size = (u32)ftell(fp);

    if (mode[0] == 'a')
    {
        f->pos = f->size;
    }
    else
    {
        fseek(fp, 0, SEEK_SET);
    }

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

int fs_read(void *ptr, u32 size, u32 count, FS_FILE * stream)
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

    FILE *fp = (FILE *)stream->handle;

    size_t r = fread(ptr, size, count, fp);
    if (r == 0)
        return 0;

    stream->pos = (u32)ftell(fp);
    return (int)r;

#else /* PC / ANDROID */

	FILE *fp = (FILE *) stream->handle;
	size_t r = fread(ptr, size, count, fp);

	stream->pos = (u32) ftell(fp);
	return (int)r;

#endif
}

/* ---------------------------------------------------- */
/* wsceitura estilo write */
/* ---------------------------------------------------- */

int fs_write(const void *ptr, u32 size, u32 count, FS_FILE *stream)
{
	if (!stream || !stream->handle || !ptr || size == 0 || count == 0)
		return 0;

	u32 bytes = size * count;

#if defined(PS2_BUILD)

	int fd = *(int *)stream->handle;
	int r = fioWrite(fd, ptr, bytes);
	if (r <= 0)
		return 0;

	stream->pos += r;

	/* actualizar tamaño si crece */
	if (stream->pos > stream->size)
		stream->size = stream->pos;

	return r / size;

#elif defined(PSP_BUILD)

	int fd = *(int *)stream->handle;
	int r = sceIoWrite(fd, ptr, bytes);
	if (r <= 0)
		return 0;

	stream->pos += r;

	if (stream->pos > stream->size)
		stream->size = stream->pos;

	return r / size;

#elif defined(GC_BUILD)

    FILE *fp = (FILE *)stream->handle;

    size_t r = fwrite(ptr, size, count, fp);
    if (r == 0)
        return 0;

    stream->pos = (u32)ftell(fp);

    if (stream->pos > stream->size)
        stream->size = stream->pos;

    return (int)r;

#else /* PC / ANDROID */

	FILE *fp = (FILE *)stream->handle;
	size_t r = fwrite(ptr, size, count, fp);

	stream->pos = (u32)ftell(fp);

	if (stream->pos > stream->size)
		stream->size = stream->pos;

	return (int)r;

#endif
}

/* ---------------------------------------------------- */
/* mover cursor estilo fseek */
/* ---------------------------------------------------- */

int fs_seek(FS_FILE * stream, s32 offset, int origin)
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

    FILE *fp = (FILE *)stream->handle;

    int r = fseek(fp, offset, origin);
    if (r == 0)
        stream->pos = (u32)ftell(fp);

    return r;

#else /* ANDROID + PC */

	FILE *fp = (FILE *) stream->handle;
	int r = fseek(fp, offset, origin);
	if (r == 0)
		stream->pos = (u32) ftell(fp);
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

void fs_close(FS_FILE * stream)
{
	if (!stream)
		return;

#if defined(PS2_BUILD)

	if (stream->handle)
	{
		int fd = *(int *)stream->handle;
		fioClose(fd);
		free(stream->handle);
	}

#elif defined(PSP_BUILD)

	if (stream->handle)
	{
		int fd = *(int *)stream->handle;
		sceIoClose(fd);
		free(stream->handle);
	}

#elif defined(GC_BUILD)

    if (stream->handle)
    {
        fclose((FILE *)stream->handle);
    }
#else /* PC / ANDROID */

	if (stream->handle)
		fclose((FILE *) stream->handle);

#endif

	free(stream);
}

/********************/

int fs_mkdir(const char *path)
{
    const char *real = fs_make_path(path);

    if (!real)
        return -1;

#if defined(PS2_BUILD)

    int r = fioMkdir(real);
    return (r >= 0) ? 0 : -1;

#elif defined(PSP_BUILD)

    int r = sceIoMkdir(real, 0777);
    return (r >= 0) ? 0 : -1;

#elif defined(GC_BUILD)

    /* GameCube usa libc tipo POSIX */
    return mkdir(real, 0755);

#elif defined(ANDROID_BUILD)

    return mkdir(real, 0755);

#elif defined(PC_BUILD)

    #if defined(_WIN32)
        return _mkdir(real);
    #else
        return mkdir(real, 0755);
    #endif

#else

    return -1;

#endif
}


int fs_rmdir(const char *path)
{
    const char *real = fs_make_path(path);

    if (!real)
        return -1;

#if defined(PS2_BUILD)

    return (fioRmdir(real) >= 0) ? 0 : -1;

#elif defined(PSP_BUILD)

    return (sceIoRmdir(real) >= 0) ? 0 : -1;

#elif defined(GC_BUILD)

    return (rmdir(real) == 0) ? 0 : -1;

#elif defined(ANDROID_BUILD)

    return (rmdir(real) == 0) ? 0 : -1;

#elif defined(PC_BUILD)

    #if defined(_WIN32)
        return (_rmdir(real) == 0) ? 0 : -1;
    #else
        return (rmdir(real) == 0) ? 0 : -1;
    #endif

#else

    return -1;

#endif
}

int fs_exists(const char *path)
{
    const char *real = fs_make_path(path);

    if (!real)
        return 0;

#if defined(PS2_BUILD)

    int fd = fioOpen(real, O_RDONLY);
    if (fd >= 0)
    {
        fioClose(fd);
        return 1;
    }
    return 0;

#elif defined(PSP_BUILD)

    SceUID fd = sceIoOpen(real, PSP_O_RDONLY, 0);
    if (fd >= 0)
    {
        sceIoClose(fd);
        return 1;
    }
    return 0;

#elif defined(GC_BUILD)

    struct stat st;
    return (stat(real, &st) == 0);

#elif defined(ANDROID_BUILD)

    return (access(real, F_OK) == 0);

#elif defined(PC_BUILD)

    return (access(real, F_OK) == 0);

#else

    return 0;

#endif
}

int fs_isdir(const char *path)
{
    const char *real = fs_make_path(path);

    if (!real)
        return 0;

#if defined(PS2_BUILD)

   fio_stat_t st;

    if (fioGetstat(real, &st) < 0)
        return 0;

    /* En PS2 los directorios tienen el bit 0x2000 */
    if (st.mode & 0x2000)
        return 1;

    return 0;

#elif defined(GC_BUILD) || defined(ANDROID_BUILD) || defined(PC_BUILD)

    struct stat st;
    if (stat(real, &st) == 0)
        return S_ISDIR(st.st_mode);

    return 0;

#else

    return 0;

#endif
}

int fs_opendir(FS_DIR *dir, const char *path)
{
    const char *real = fs_make_path(path);

    if (!dir || !real)
        return -1;

#if defined(PS2_BUILD)

    int fd = fioDopen(real);
    if (fd < 0)
        return -1;

    dir->handle = (void*)(intptr_t)fd;

    /* guardar ruta */
    strncpy(dir->path, real, sizeof(dir->path)-1);
    dir->path[sizeof(dir->path)-1] = '\0';

    return 0;

#elif defined(PSP_BUILD)

    SceUID fd = sceIoDopen(real);
    if (fd < 0)
        return -1;

    dir->handle = (void*)(intptr_t)fd;

    /* guardar ruta */
    strncpy(dir->path, real, sizeof(dir->path)-1);
    dir->path[sizeof(dir->path)-1] = '\0';

    return 0;

#elif defined(GC_BUILD) || defined(ANDROID_BUILD) || defined(PC_BUILD)

    DIR *d = opendir(real);
    if (!d)
        return -1;

    dir->handle = d;

    /* guardar ruta para fs_readdir */
    strncpy(dir->path, real, sizeof(dir->path)-1);
    dir->path[sizeof(dir->path)-1] = '\0';

    return 0;

#else

    return -1;

#endif
}

int fs_readdir(FS_DIR *dir, FS_DIRENT *ent)
{
    if (!dir || !ent || !dir->handle)
        return -1;

#if defined(PS2_BUILD)

    fio_dirent_t info;
    int fd = (unsigned int)dir->handle;

    if (fioDread(fd, &info) <= 0)
        return -1;

    strncpy(ent->name, info.name, sizeof(ent->name) - 1);
    ent->name[sizeof(ent->name) - 1] = '\0';

    /* En PS2 el bit 0x2000 indica directorio */
    ent->is_dir = (info.stat.mode & 0x2000) ? 1 : 0;

    return 0;

#elif defined(PSP_BUILD)

    SceUID fd = (SceUID)(intptr_t)dir->handle;

    SceIoDirent info;
    memset(&info, 0, sizeof(SceIoDirent));   // ⚠ IMPORTANTE

    int res = sceIoDread(fd, &info);

    if (res <= 0)
        return -1;

    strncpy(ent->name, info.d_name, sizeof(ent->name)-1);
    ent->name[sizeof(ent->name)-1] = '\0';

    ent->is_dir = (info.d_stat.st_attr & FIO_SO_IFDIR) ? 1 : 0;

    return 0;

#elif defined(GC_BUILD) || defined(ANDROID_BUILD) || defined(PC_BUILD)

    DIR *d = (DIR*)dir->handle;
    struct dirent *entry = readdir(d);

    if (!entry)
        return -1;

    /* copiar nombre seguro */
    strncpy(ent->name, entry->d_name, sizeof(ent->name)-1);
    ent->name[sizeof(ent->name)-1] = '\0';

    /* construir ruta completa */
    char fullpath[512];
    snprintf(fullpath, sizeof(fullpath), "%s/%s", dir->path, entry->d_name);

    ent->is_dir = fs_isdir(fullpath);

    return 0;

#else

    return -1;

#endif
}


int fs_closedir(FS_DIR *dir)
{
    if (!dir)
        return -1;

#if defined(PS2_BUILD)

    int fd = (int)(intptr_t)dir->handle;
    return fioDclose(fd);

#elif defined(PSP_BUILD)

    SceUID fd = (SceUID)(intptr_t)dir->handle;
    return sceIoDclose(fd);

#elif defined(GC_BUILD) || defined(ANDROID_BUILD) || defined(PC_BUILD)

    DIR *d = (DIR*)dir->handle;
    return closedir(d);

#else

    return -1;

#endif
}

int fs_remove(const char *path)
{
    const char *real = fs_make_path(path);

    if (!real)
        return -1;

#if defined(PS2_BUILD)

    return fioRemove(real);

#elif defined(PSP_BUILD)

    return sceIoRemove(real);

#elif defined(GC_BUILD) || defined(ANDROID_BUILD) || defined(PC_BUILD)

    return remove(real);

#else

    return -1;

#endif
}
