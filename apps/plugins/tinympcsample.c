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

/* .wav header struct
 *
 * Header is composed of 3 parts: 
 *      1. RIFF chunk descriptor
 *      2. fmt sup-chunk
 *      3. data sub-chunk
 *
 * Endianess matters...
 * How are bytes packed in the actual header? lets pack
 */
#pragma pack(1)
struct wav_header
{
    char chunk_id[4];
    unsigned long chunk_size;
    char format[4];
    char fmt_id[4];
    unsigned short fmt_size;
    unsigned short num_channels;
    unsigned long sample_rate;
    unsigned long byte_rate;
    unsigned short block_align;
    unsigned short bits_per_sample;
}

/* Constructs the first .wav filepath */
void construct_wav_path(char* filepath)
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

    /* Import .wav */
    char filepath[MAX_PATH];
    char header_buf[44];
    int path_fd;
    ssize_t read_valid;

    construct_wav_path(filepath);
    path_fd = rb->open(filepath);
    read_valid = rb->read(path_fd, &header_buf, 44);

    if (read_valid < 0) { 
        rb->splash("Read invalid, byte-reading error");
        return PLUGIN_ERROR;
    }

    /* TBD: check the files size and determine alloc size for the buffer */

    /* TBD URGNT: Check if the size of the header struct is < 44 or memcpy will
     * corrupt mem */


    /* TBD: recheck wav import flow: 
     *      path construction -> read header into buf -> cpy to struct */

    /* Read from the .wav header - We need to figure out how big the file is 
     * so we can alloc an appropriate amount */
    wav_header first_wav_header;
    if (filepath != NULL) {
        memcpy(first_wav_header, header_buf, sizeof(struct wav_header));
    }

    /* TBD: RAM alloc strat */
    static struct buflib_context tmpc_ctx;
    size_t buf_size;
    void *plugin_buf = rb->plugin_get_buffer(&buf_size);
    rb->buflib_init(&tmpc_ctx, plugin_buf, buf_size);

    /* TBD: Main loop */
    while (true) {}

    return PLUGIN_OK;
}
