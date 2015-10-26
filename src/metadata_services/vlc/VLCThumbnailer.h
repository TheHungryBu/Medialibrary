/*****************************************************************************
 * Media Library
 *****************************************************************************
 * Copyright (C) 2015 Hugo Beauzée-Luyssen, Videolabs
 *
 * Authors: Hugo Beauzée-Luyssen<hugo@beauzee.fr>
 *
 * This program is free software; you can redistribute it and/or modify it
 * under the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation; either version 2.1 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this program; if not, write to the Free Software Foundation,
 * Inc., 51 Franklin Street, Fifth Floor, Boston MA 02110-1301, USA.
 *****************************************************************************/

#pragma once

#include <condition_variable>

#include <vlcpp/vlc.hpp>

#if defined(WITH_EVAS)
#include <Evas.h>
#endif

#include "IMetadataService.h"

class VLCThumbnailer : public IMetadataService
{
public:
    VLCThumbnailer( const VLC::Instance& vlc );
    ~VLCThumbnailer();
    virtual bool initialize(IMetadataServiceCb *callback, MediaLibrary *ml) override;
    virtual unsigned int priority() const override;
    virtual void run(std::shared_ptr<Media> file, void *data) override;

private:
    bool startPlayback( std::shared_ptr<Media> file, VLC::MediaPlayer& mp, void *data);
    bool seekAhead(std::shared_ptr<Media> file, VLC::MediaPlayer &mp, void *data);
    void setupVout(VLC::MediaPlayer &mp);
    bool takeSnapshot(std::shared_ptr<Media> file, VLC::MediaPlayer &mp, void* data);
    bool compress(std::shared_ptr<Media> file, void* data );

private:
    // Force a base width, let height be computed depending on A/R
    static const uint32_t Width = 320;
    static const uint8_t Bpp = 4;

private:
    VLC::Instance m_instance;
    IMetadataServiceCb* m_cb;
    MediaLibrary* m_ml;
    std::mutex m_mutex;
    std::condition_variable m_cond;
    // Per snapshot variables
#ifdef WITH_EVAS
    std::unique_ptr<Evas, void(*)(Evas*)> m_canvas;
#endif
    std::unique_ptr<uint8_t[]> m_buff;
    std::atomic_bool m_snapshotRequired;
    uint32_t m_height;
    uint32_t m_prevSize;
};
