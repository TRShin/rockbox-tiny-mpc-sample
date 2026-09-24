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
#include "action.h"
#include "button.h"

/* Exclusively testing Ipod nano 2G, create custom mapping */
#define CONTEXT_TMPCS (CONTEXT_CUSTOM | 1)

enum {
    ACTION_TMPCS_LEFT = LAST_ACTION_PLACEHOLDER + 1,
    ACTION_TMPCS_RIGHT,
    ACTION_TMPCS_SELECT,
    ACTION_TMPCS_DOWN,
    ACTION_TMPCS_UP,
};

static const struct button_mapping button_context_tmpcs[] = {
    { ACTION_TMPCS_LEFT,         BUTTON_LEFT,                   BUTTON_NONE },
    { ACTION_TMPCS_RIGHT,        BUTTON_RIGHT,                  BUTTON_NONE },
    { ACTION_TMPCS_SELECT,       BUTTON_SELECT,                 BUTTON_NONE },
    { ACTION_TMPCS_DOWN,         BUTTON_PLAY,                   BUTTON_NONE },
    { ACTION_TMPCS_UP,        BUTTON_MENU|BUTTON_REL,        BUTTON_MENU },

    /* TBD 
    *{ ACTION_KBD_UP,           BUTTON_SCROLL_BACK,                    BUTTON_NONE },
    *{ ACTION_KBD_UP,           BUTTON_SCROLL_BACK|BUTTON_REPEAT,      BUTTON_NONE },
    *{ ACTION_KBD_DOWN,         BUTTON_SCROLL_FWD,                     BUTTON_NONE },
    *{ ACTION_KBD_DOWN,         BUTTON_SCROLL_FWD|BUTTON_REPEAT,       BUTTON_NONE },
    */

    LAST_ITEM_IN_LIST__NEXTLIST(CONTEXT_STD)
};

static const struct button_mapping *tmpcs_get_context_map(int context)
{
   (void)context;
    return button_context_tmpcs;
}

/* Page control */
enum pages
{
    TIMELINE,
    SAMPLE,
    CHOP,
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
#pragma pack(push, 1)
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
#pragma(pop)

/* Pad meta data */
enum pad_state {
    PAD_SET,
    PAD_NOT_SET
};

struct pad_info {
    enum pad_state state;
    struct wav_header header;
    char filepath[MAX_PATH];
    int action;
    int handle;
    size_t start_offset;
    size_t end_offset;
};

static struct pad_info pad_all[3];

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

/* Parses .wav header into the pad */
void parse_wav_header(struct pad_info *curr_pad_info) 
{
    char header_buf[44];

    /* Build path to .wav */
    construct_wav_path(curr_pad_info->filepath, MAX_PATH);
    int path_fd = rb->open(curr_pad_info->filepath, O_RDONLY);
    if (path_fd < 0) {
        rb->splashf(HZ*2, "Open file error");
        return;
    }

    /* Why am I not reading directly into the header struct ??? */
    ssize_t read_valid = rb->read(path_fd, header_buf, 44);
    if (read_valid != 44) { 
        rb->splashf(HZ*2, "Read invalid, byte-reading error or file open error");
        return;
    }

    rb->close(path_fd);

    /* Copy header struct into pad_info */
    if (sizeof(curr_pad_info->header) != 44) {
        rb->splash(HZ*2, ".wav header size issue");
        return;
    }

    /* Copy exactly 44 bytes of memory from the header buffer and into the struct
     * Since our header struct is not padded we can just copy the raw bytes directly */ 
    memcpy(&curr_pad_info->header, header_buf, 44);
}

void page_enter(int page, int action)
{
    switch (page) {
        case TIMELINE:
        {
            rb->splash(HZ*3, "You are entering the TIMELINE page");
            break;
        }
        case SAMPLE:
        {

            rb->splash(HZ*3, "You are entering the SAMPLE page");
            break;
        }
        case CHOP:
        {
            rb->splash(HZ*3, "You are entering the CHOP page");

#if 0
            if (pad_all[action].state == PAD_NOT_SET) {

                rb->splashf(HZ*2, "Modify pad %d", action);

                /* Move .wav header info into pad_info 
                 * Basically init the pad's sample */
                struct pad_info curr_pad_info;
                parse_wav_header(&curr_pad_info);

                curr_pad_info.action = action;
                curr_pad_info.state = PAD_SET;
            }
            else {
                break;
            }
#endif

            break;
        }
        case RECORD:
        {
            rb->splash(HZ*3, "You are entering the RECORD page");
            break;
        }

        /* In the future probably want to return a return code */
        default: return;
    }

    /* In the future probably want to return a return code */
    return;
}

void page_exit(int page)
{

    rb->splashf(HZ*3, "Exiting page: %d", page);
    return;
}

int page_update(int page, int action)
{
    switch (page) {
        case TIMELINE:
        {
            switch (action) {
                case ACTION_TMPCS_LEFT: return SAMPLE;
                case ACTION_TMPCS_RIGHT: return RECORD;
                default: break;
            }
            break;
        }

        case SAMPLE:
        {
            switch (action) {
                case ACTION_TMPCS_LEFT: return CHOP;
                case ACTION_TMPCS_UP: return TIMELINE;
                default: break;
            }
            break;
        }

        case CHOP:
        {
            switch (action) {
                case ACTION_TMPCS_UP: return TIMELINE;
                default: break;
            }
            break;
        }

        /* Temp Disable */
        case RECORD:
        {
            switch (action) {
                case ACTION_TMPCS_UP: return TIMELINE;
                default: break;
            }
            break;
        }

        default: return page;
    }
    return page;
}

/* This is the plugin entry point */
enum plugin_status plugin_start(const void* parameter)
{
    (void)parameter;

    int current_page = TIMELINE, previous_page = TIMELINE;

#if 0
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
#endif

    rb->splash(HZ*2, "Entering loop"); 
    /* Main loop */
    while (true) {

        /*
         * Want: Pages dictate actions
         *       Pages should only update after an action
         *
         *
         *
         * Defualt to TIMELINE
         * get_action blocks until action
         * say action is up -> page gets updated 
         * curr prev page diff -> next loop action is grabbed again, 
         * enter page diff if block, the prev page calls exit, curr page enters 
         * prev page updates to curr
         *
         *
         */
        if (current_page != previous_page) {
            page_exit(previous_page);
            page_enter(current_page, action);
            previous_page = current_page;
        }

        rb->splash(HZ*2, "In loop"); 
        int action = rb->get_custom_action(CONTEXT_TMPCS, TIMEOUT_BLOCK, tmpcs_get_context_map);

        /* Will there ever be a case when state needs updating, but is
         * not prompted by the action? */
        current_page = page_update(current_page, action);

        if (current_page == RECORD) break;
    }

#if 0
    /* Unpin and free memory before returning */ 
    rb->buflib_put_data_pinned(&tmpc_ctx, my_data);
    test_handle = rb->buflib_free(&tmpc_ctx, test_handle);
#endif 

    return PLUGIN_OK;
}
