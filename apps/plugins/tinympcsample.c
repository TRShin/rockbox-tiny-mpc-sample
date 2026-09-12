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

/* Application description and Manual:
 *
 * Graph rep:
 * Timeline page <--> Pad assignment page
 * Timeline page <--> Recording page TBD
 * Pad assignment page <--> Pad assignment page
 *
 * Users will enter the default, "timeline" page upon opening the applicaiton. 
 * A Timeline is just a ...
 *
 * The timeline will be empty and cleared each boot of the app
 *
 * How is the timeline organized? "Record" playback to add "components" to the timeline
 *
 */

#include "plugin.h"

/* Exclusively testing Ipod nano 2G */
#define ROCKPAINT_QUIT      ( ~BUTTON_MAIN )
#define ROCKPAINT_DRAW      BUTTON_SELECT
#define ROCKPAINT_MENU      ( BUTTON_SELECT | BUTTON_MENU )
#define ROCKPAINT_TOOLBAR   ( BUTTON_MENU | BUTTON_LEFT )
#define ROCKPAINT_TOOLBAR2  ( BUTTON_MENU | BUTTON_RIGHT )
#define ROCKPAINT_UP        BUTTON_MENU
#define ROCKPAINT_DOWN      BUTTON_PLAY
#define ROCKPAINT_LEFT      BUTTON_LEFT
#define ROCKPAINT_RIGHT     BUTTON_RIGHT

/* Plugin page control */
enum current_page 
{
    TIMELINE,
    SAMPLE,
    RECORD
};

/* .wav header struct
 *
 * Header is composed of 3 parts: 
 *      1. RIFF chunk descriptor
 *      2. fmt sup-chunk
 *      3. data sub-chunk
 *
 * Endianess matters ...
 * Im pretty sure its little endian for everything aside from the char arrarys?
 */
#pragma pack(1)
struct wav_header
{
    char chunk_id[4];
    uint32_t chunk_size;
    char format[4];
    char fmt_id[4];
    uint32_t fmt_size;
    uint16_t audio_format;
    uint16_t num_channels;
    uint32_t sample_rate;
    uint32_t byte_rate;
    uint16_t block_align;
    uint16_t bits_per_sample;
    char data_id[4];
    uint32_t data_size;
};

/* Constructs the first .wav filepath */
void construct_wav_path(char* filepath, int path_size)
{
    /* Test opening the dir which will contain the .wav file(s) */
    DIR* wav_dir = rb->opendir("/simtracks/");
    if (wav_dir) {
        struct dirent* wav_dir_ent;

        /* readdir() advances the file stream each call */
        /* Function expects correct file type and organization
         * Error check if no dir and skip . .. entries */
        while ((wav_dir_ent = rb->readdir(wav_dir)) != NULL) {
            if (wav_dir_ent->d_name[0] != '.') {
                break;
            }
        }

        if (wav_dir_ent == NULL) return;

        /* On the first wav file found, construct and assign the path */
        rb->snprintf(filepath, path_size, "/simtracks/%s", wav_dir_ent->d_name);
    }

    rb->closedir(wav_dir);
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

    construct_wav_path(filepath, MAX_PATH);
    path_fd = rb->open(filepath, O_RDONLY);
    read_valid = rb->read(path_fd, header_buf, 44);

    if (read_valid < 0 || path_fd < 0) { 
        rb->splashf(HZ*2, "Read invalid, byte-reading error or file open error");
        return PLUGIN_ERROR;
    }

    /* Read from the .wav header - We need to figure out how big the file is 
     * so we can alloc an appropriate amount */
    struct wav_header first_wav_header;

    /* Header size test */
    if (sizeof(first_wav_header) != 44) {
        rb->splash(HZ*2, ".wav header size issue");
        return PLUGIN_ERROR;
    }

    /* Copy exactly 44 bytes of memory from the header buffer and into the struct
     * Since our header struct is not padded we can just copy the raw bytes directly */ 
    memcpy(&first_wav_header, header_buf, sizeof(struct wav_header));

    /* Close the file descriptor */
    rb->close(path_fd);

    /* Try print the size of the .wav */ 
    /*
    rb->splashf(HZ*2, "Should be data chunk?: %.4s", first_wav_header.data_id);
    rb->splashf(HZ*2, "Size of data chunk?: %" PRIu32 ": ", first_wav_header.data_size);
    rb->splashf(HZ*2, "Sample rate?: %" PRIu32 ": ", first_wav_header.sample_rate);
    sleep(50);
    */

    /* TBD: RAM alloc strat */
    /* Test how much size we have */
    size_t buf_size;
    void *plugin_buf = rb->plugin_get_buffer(&buf_size);
    static struct buflib_context tmpc_ctx;
    rb->buflib_init(&tmpc_ctx, plugin_buf, buf_size);

    size_t buflib_avail_size = rb->buflib_available(&tmpc_ctx);
    /* Determined around 524000 Bytes */
    rb->splashf(HZ*2, "Size available: %zu", buflib_avail_size);


    /* TBD: Also check the "actual" buffers size */



    /* -- Sudo code for Pad mode -- */

    /* In Pad mode, the user will choose a track to open, choose a pad to define, then
     * will define the pad by setting start and end points based on the timestamp (potentially) */

    while (true) {

        /* debug to be removed */
        rb->splash(HZ, "Entered the main while loop successfully");
        rb->sleep(10);

       /* Flow from plugin entry to "Pad mode", pressing x button returns back a "page"
        *       A. x button will be pressed to enter Sample mode
        *       B. x button will be pressed to enter Pad edit mode 
        *       C. Pad can be modified
        */ 
        
        if ()


        break;
    }

    return PLUGIN_OK;
}
