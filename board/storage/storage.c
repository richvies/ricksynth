/****************************************************************************

RickSynth
----------

MIT License

Copyright (c) [2021] [Richard Davies]

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
****************************************************************************/


#include "storage.h"

#include "lfs.h"
#include "chips.h"

#include "flash.h"
#include "merror.h"


typedef struct
{
  lfs_t lfs;
  struct lfs_config cfg;
  lfs_file_t file;
} stg_t;


static int external_read(const struct lfs_config *c,
                         lfs_block_t block,
                         lfs_off_t off,
                         void *buffer,
                         lfs_size_t size);
static int external_prog(const struct lfs_config *c,
                         lfs_block_t block,
                         lfs_off_t off,
                         const void *buffer,
                         lfs_size_t size);
static int external_erase(const struct lfs_config *c, lfs_block_t block);
static int external_sync(const struct lfs_config *c);
static void logError(stg_e idx, merror_e type, int err);


static stg_t stgs[STG_NUM_OF] =
{
  {
    .cfg =
    {
      // block device operations
      .read  = external_read,
      .prog  = external_prog,
      .erase = external_erase,
      .sync  = external_sync,

      // block device configuration
      .read_size = STG_0_READ_SIZE,
      .prog_size = STG_0_PROG_SIZE,
      .block_size = STG_0_SECTOR_SIZE,
      .block_count = STG_0_SECTOR_COUNT,
      .cache_size = 128,
      .lookahead_size = 16,
      .block_cycles = 500,
    }
  }
};


bool     STG_init(bool first_boot)
{
  bool ret = true;

  stg_e idx;
  stg_t *stg;
  int err;

  ret &= FLASH_init();
  ret &= W25Q_init();

  for (idx = STG_FIRST; idx < STG_NUM_OF; idx++)
  {
    stg = &stgs[idx];

    err = lfs_mount(&stgs[idx].lfs, &stgs[idx].cfg);

    if (err)
    {
      if (!first_boot) {
      MERR_error(MERROR_STG_MOUNT_FAIL, idx); }

      err = lfs_format(&stg->lfs, &stg->cfg);

      if (err)
      {
        ret = false;
        MERR_error(MERROR_STG_FORMAT_FAIL, idx);
      }
    }
  }

  return ret;
}

bool     STG_deinit(stg_e idx)
{
  bool ret = false;
  stg_t *stg = &stgs[idx];

  if (lfs_unmount(&stg->lfs))
  {
    MERR_error(MERROR_STG_UNMOUNT_FAIL, idx);
    ret = false;
  }

  return ret;
}

bool     STG_newFile(stg_e idx, char *fname)
{
  bool ret = false;
  stg_t *stg = &stgs[idx];
  int err;

  err = lfs_file_open(&stg->lfs,
                      &stg->file,
                      fname,
                      LFS_O_CREAT | LFS_O_RDWR | LFS_O_EXCL);

  if (err < 0) {
    logError(idx, MERROR_STG_NEW_FILE, err); }
  else {
    ret = STG_closeFile(idx); }

  return ret;
}

bool     STG_openFile(stg_e idx, char *fname)
{
  bool ret = false;
  stg_t *stg = &stgs[idx];
  int err;

  err = lfs_file_open(&stg->lfs,
                      &stg->file,
                      fname,
                      LFS_O_CREAT | LFS_O_RDWR);

  if (err < 0) {
    logError(idx, MERROR_STG_OPEN_FILE, err); }
  else {
    ret = true; }

  return ret;
}

bool     STG_writeFile(stg_e idx,
                       void *buf,
                       uint16_t len,
                       uint16_t offset)
{
  bool ret = false;
  stg_t *stg = &stgs[idx];
  int err;

  err = lfs_file_seek(&stg->lfs,
                      &stg->file,
                      offset,
                      LFS_SEEK_SET);

  if (err < 0) {
    goto end; }

  err = lfs_file_write(&stg->lfs,
                       &stg->file,
                       buf,
                       len);

end:
  if (err < 0) {
    logError(idx, MERROR_STG_WRITE_FILE, err); }
  else {
    ret = true; }

  return ret;
}

bool     STG_appendFile(stg_e idx, void *buf, uint16_t len)
{
  bool ret = false;
  stg_t *stg = &stgs[idx];
  int err;

  err = lfs_file_seek(&stg->lfs,
                      &stg->file,
                      0,
                      LFS_SEEK_END);

  if (err < 0) {
    goto end; }

  err = lfs_file_write(&stg->lfs,
                       &stg->file,
                       buf,
                       len);

end:
  if (err < 0) {
    logError(idx, MERROR_STG_APPEND_FILE, err); }
  else {
    ret = true; }

  return ret;
}

uint16_t STG_readFile(stg_e idx, void *buf, uint16_t len)
{
  bool ret = false;
  stg_t *stg = &stgs[idx];
  int err;

  err = lfs_file_read(&stg->lfs,
                      &stg->file,
                      buf,
                      len);

  if (err < 0) {
    logError(idx, MERROR_STG_READ_FILE, err); }
  else {
    ret = true; }

  return ret;
}

bool     STG_closeFile(stg_e idx)
{
  bool ret = false;
  stg_t *stg = &stgs[idx];
  int err;

  err = lfs_file_close(&stg->lfs, &stg->file);

  if (err < 0) {
    logError(idx, MERROR_STG_CLOSE_FILE, err); }
  else {
    ret = true; }

  return ret;
}


static int external_read(const struct lfs_config *c,
                         lfs_block_t block,
                         lfs_off_t off,
                         void *buffer,
                         lfs_size_t size)
{
  (void)c;

  int ret = LFS_ERR_OK;

  if (false == W25Q_readSector(block, off, buffer, size))
  {
    ret = LFS_ERR_IO;
  }

  return ret;
}

static int external_prog(const struct lfs_config *c,
                         lfs_block_t block,
                         lfs_off_t off,
                         const void *buffer,
                         lfs_size_t size)
{
  (void)c;

  int ret = LFS_ERR_OK;

  if (false == W25Q_programSector(block, off, (void*)buffer, size))
  {
    ret = LFS_ERR_IO;
  }

  return ret;
}

static int external_erase(const struct lfs_config *c, lfs_block_t block)
{
  (void)c;

  int ret = LFS_ERR_OK;

  if (false == W25Q_eraseSector(block))
  {
    ret = LFS_ERR_IO;
  }

  return ret;
}

static int external_sync(const struct lfs_config *c)
{
  (void)c;

  return LFS_ERR_OK;
}

static void logError(stg_e idx, merror_e type, int err)
{
  uint32_t arg[] = {idx, err};
  MERR_errorExt(type, arg, SIZEOF(arg));
}
