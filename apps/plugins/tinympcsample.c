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

/* tinympcsample rockbox application */

#include "plugin.h"

/* this is the plugin entry point */
enum plugin_status plugin_start(const void* parameter)
{
    (void)parameter;
    char filepath[MAX_PATH];
    bool filefound = false;

    while (true)
    {
        int action = rb->get_action(CONTEXT_STD, TIMEOUT_BLOCK);

        if (action == ACTION_STD_OK) 
            break;
        else if (action == ACTION_STD_CANCEL) 
            return PLUGIN_OK;
    }

    DIR* dir = rb->opendir("../../build-dir/simdisk/sudofiles/");
    if (dir) {
        struct dirent* entry;
        while (entry == readdir(dir)) {
            if (entry->d_name[0] == ".") {
                continue;
        }

        rb->snprintf(filepath, sizeof(filepath), "../../build-dir/simdisk/sudofiles/", entry->d_name);
        filefound = true;
        break;
        }
    }

    if (filefound) {
        rb->playlist_create(NULL, NULL);
        rb->playlist_insert_track(NULL, filepath, 0, true, true);
        rb->playlist_start(0, 0, 0);
    }

    return PLUGIN_OK;
}
