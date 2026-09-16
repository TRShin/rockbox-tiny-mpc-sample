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

/* Exclusively testing Ipod nano 2G */
#define TMPCS_SELECT    BUTTON_SELECT
#define TMPCS_UP        BUTTON_MENU
#define TMPCS_DOWN      BUTTON_PLAY
#define TMPCS_LEFT      BUTTON_LEFT
#define TMPCS_RIGHT     BUTTON_RIGHT

/* Plugin page control */
enum pages
{
    TIMELINE,
    SAMPLE,
    CHOP,
    RECORD
};

/* Pad meta data 
 * HOLD on this and experiment with pcm 
 */

/* 
struct pad_info {
    int key;
    type begin;
    type end;
};
*/

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

/* Dont even test state management, figure out how to use the pcm buffer */
#if 0
void page_enter(int page, int action)
{
    switch (page) {
        case TIMELINE:
        {}
        case SAMPLE:
        {}
        case CHOP:
        {
            /* FIRST: test the pcm how to do playback we can find how to save state possibly */ 


            /* how to know what pad was selected? - Pass in the action? */
            /* Create a struct with the data that needs to be saved to memory 
             *
             * Key, Markers, ...
             *
             */

            break;
        }
        case RECORD:
        {}

        /* In the future probably want to return a return code */
        default: return;
    }

    /* In the future probably want to return a return code */
    return;
}

void page_exit(int page)
{}

int page_update(int page, int action)
{
    switch (page) {
        case TIMELINE:
        {
            switch (action) {
                case TMPCS_SELECT: return SAMPLE;
                default: break;
            }
            break;
        }

        case SAMPLE:
        {
            switch (action) {
                case TPMS_LEFT: return CHOP;
            }
            break;
        }

        case CHOP:
        {
            switch (action) {

            }
        }

        case RECORD:
        {
            break;
        }

        default: return page;
    }
}
#endif

/* This is the plugin entry point */
enum plugin_status plugin_start(const void* parameter)
{
    (void)parameter;

    /* TBD: Default to Timeline page on entry 
     * int current_page = TIMELINE, previous_page = TIMELINE;
     */

    /* Find .wav on disk -> allocate buffer in mem -> copy .wav data to memory -> call playback function */
    char filepath[MAX_PATH], header_buf[44];

    construct_wav_path(filepath, MAX_PATH);
    int path_fd = rb->open(filepath, O_RDONLY);
    if (path_fd < 0) {
        rb->splashf(HZ*2, "Open file error");
        return PLUGIN_ERROR;
    }

    ssize_t read_valid = rb->read(path_fd, header_buf, 44);
    if (read_valid != 44) { 
        rb->splashf(HZ*2, "Read invalid, byte-reading error or file open error");
        return PLUGIN_ERROR;
    }

    /* Read from the .wav header - We need to figure out how big the file is 
     * so we can alloc an appropriate amount */
    struct wav_header first_wav_header;
    if (sizeof(first_wav_header) != 44) {
        rb->splash(HZ*2, ".wav header size issue");
        return PLUGIN_ERROR;
    }

    /* Copy exactly 44 bytes of memory from the header buffer and into the struct
     * Since our header struct is not padded we can just copy the raw bytes directly */ 
    memcpy(&first_wav_header, header_buf, sizeof(struct wav_header));

    /* Init the rb buffer in memory */

    /* Really 31 MB? Seems so... */
    static struct buflib_context tmpc_ctx;
    size_t rb_audiobuffer;

    void *audio_buf_ptr = rb->plugin_get_audio_buffer(&rb_audiobuffer);

    rb->buflib_init(&tmpc_ctx, audio_buf_ptr, rb_audiobuffer);

    int test_handle = rb->buflib_alloc(&tmpc_ctx, first_wav_header.data_size);
    if (test_handle <= 0 ) {
        rb->splash(HZ*2, "Alloc memory error");
        return PLUGIN_ERROR;
    }

    /* Access to the rbbuffer */

    /* Issue with using a pointer to the data is that the memory management system moves blocks around to defrag,
     * thus it can be the case that the pointer wont point to our buffer after defrag / move so lets pin the 
     * buffer while we write and read from it.
     */
    void *my_data = buflib_get_data_pinned(&tmpc_ctx, test_handle);
    
    /* Copy actual pcm data from the track to a buffer: */

    /* Move the fd 44 bytes over to skip the header and access the raw PCM */
    off_t data_start = rb->lseek(path_fd, 44, SEEK_SET);
    if (data_start != 44) {
        rb->splash(HZ*2, "Wrong offset, cannot copy memory");
        return PLUGIN_ERROR;
    }

    /* Read PCM into the rbbuffer */
    ssize_t pcmdata_read_valid = rb->read(path_fd, my_data, first_wav_header.data_size);
    if (pcmdata_read_valid < 0 || path_fd < 0) { 
        rb->splashf(HZ*2, "Read invalid, byte-reading error or file open error");
        return PLUGIN_ERROR;
    }
    //rb->splashf(HZ*2, "Bytes read: %zd should equal data size from the header: %u", pcmdata_read_valid, first_wav_header.data_size);

    /* Close the file descriptor after read */
    rb->close(path_fd);
    rb->splash(HZ*2, "closed fd");

    /* NO AUDIO Resolved! - Raw PCM defaults to 0 gain potentially? So manually set the amp yay */ 
    rb->mixer_channel_set_amplitude(PCM_MIXER_CHAN_PLAYBACK, MIX_AMP_UNITY);

    /* Data hopefully in rbbuffer, playback track */
    rb->mixer_channel_play_data(
                                PCM_MIXER_CHAN_PLAYBACK,
                                NULL,
                                my_data,
                                first_wav_header.data_size
                               );

    /* Busy wait until playback finishes */
    while (rb->mixer_channel_status(PCM_MIXER_CHAN_PLAYBACK) == CHANNEL_PLAYING) {
        rb->sleep(1);
    }

    /* Main loop */
    while (true) {
        /* int action = rb->get_action(CONTEXT_STD, TIMEOUT_BLOCK, NULL); */

        /*
        if (current_page != previous_page) {
            page_exit(previous_page);
            page_enter(current_page, action);
            previous_page = current_page;
        }

        int new_page = page_update(current_page, action);
        previous_page = current_page;
        current_page = new_page;
        */

        rb->splashf(HZ*2, "IN WHILE");
        rb->sleep(44);
        break;
    }

    /* Unpin and free memory before returning */ 
    rb->buflib_put_data_pinned(&tmpc_ctx, my_data);
    test_handle = rb->buflib_free(&tmpc_ctx, test_handle);

    return PLUGIN_OK;
}
