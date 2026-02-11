#ifndef FS_H
#define FS_H

#include <engine/types.h>

#ifdef __cplusplus
extern "C"
{
#endif

	typedef struct
	{
		void *handle;			/* FILE*, int, SceUID */
		u32 pos;
		u32 size;
	} FS_FILE;

	/* origen del seek */
#define FS_SEEK_SET  0
#define FS_SEEK_CUR  1
#define FS_SEEK_END  2

	/* inicialización del filesystem (drivers por plataforma) */
	int fs_init(void);

	/* API estilo stdio */
	FS_FILE *fs_open(const char *path, const char *mode);
	int fs_read(void *ptr, u32 size, u32 count, FS_FILE * stream);
	int fs_write(const void *ptr, u32 size, u32 count, FS_FILE *stream);
	int fs_seek(FS_FILE * stream, s32 offset, int origin);
	u32 fs_tell(FS_FILE * stream);
	void fs_close(FS_FILE * stream);

#ifdef __cplusplus
}
#endif

#endif							/* FS_H */
