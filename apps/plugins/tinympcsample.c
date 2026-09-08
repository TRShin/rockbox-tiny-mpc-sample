/***************************************************************************
 *             __________               __   ___.
 *   Open      \______   \ ____   ____ |  | _\_ |__   _______  ___
 *   Source     |       _//  _ \_/ ___\|  |/ /| __ \ /  _ \  \/  /
 *   Jukebox    |    |   (  <_> )  \___|    < | \_\ (  <_> > <  <
 *   Firmware   |____|_  /\____/ \___  >__|_ \|___  /\____/__/\_ \
 *                     \/            \/     \/    \/            \/
 *
 * Copyright (C) 2002 Björn Stenberg
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * This software is distributed on an "AS IS" basis, WITHOUT WARRANTY OF ANY
 * KIND, either express or implied.
 *
 ****************************************************************************/

/* tinympcsample rockbox application (super bare bones) */

#include "plugin.h"

/* Checks a specific wav containing dir if a wav file exists, on success
 * modifies the caller's filepath to the wav file */
void import_first_wav(char* filepath)
{
    /* Test opening the dir which will contain the wav file(s) */
    DIR* wav_dir = rb->opendir('../../build-dir/simdisk/simtracks/');
    if (wav_dir) {
        struct dirent* wav_dir_ent;

        /* readdir() advances the file stream each call */
        /* Function expects correct file type and organization
         * Error check if no dir and skip . .. entries */
        while ((wav_dir_end = rb->readdir(wav_dir)) != NULL) {
            if (wav_dir_ent->d_name[0] != '.') {
                break;
            }
        }

        if (wav_dir_end == NULL) return;

        /* On the first wav file found, construct and assign the path */
        rb->snprintf(filepath, sizeof(filepath), '../../build-dir/simdisk/simtracks/%s', entry->d_name);

    }
    return;
}

/* This is the plugin entry point */
enum plugin_status plugin_start(const void* parameter)
{
    (void)parameter;

    /* For testing, lets import the first file from the specified 
     * dir which should be a .wav */
    char filepath[MAX_PATH];
    import_first_wav(filepath);

    /* TBD: check the files size and determine alloc size for the buffer */
    if (filepath) {}

    /* TBD: RAM alloc strat */
    static struct buflib_context tmpc_ctx;
    size_t buf_size;
    void *plugin_buf = rb->plugin_get_buffer(&buf_size);
    rb->buflib_init(&tmpc_ctx, plugin_buf, buf_size);

    /* TBD: Main loop */
    while (true) {}

    return PLUGIN_OK;
}
