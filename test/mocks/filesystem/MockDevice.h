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

#include <string>
#include <memory>

#include "filesystem/IDevice.h"

using namespace medialibrary;

namespace mock
{

class Directory;
class File;

class Device : public fs::IDevice, public std::enable_shared_from_this<Device>
{
public:
    Device( const std::string& mountpoint, const std::string& uuid );

    // We need at least one existing shared ptr before calling shared_from_this.
    // Let the device be initialized and stored in a shared_ptr by the FileSystemFactory, and then
    // initialize our fake root folder
    void setupRoot();

    std::shared_ptr<Directory> root();

    virtual const std::string& uuid() const override;
    virtual bool isRemovable() const override;
    virtual bool isPresent() const override;
    virtual const std::string& mountpoint() const override;

    void setRemovable( bool value );
    void setPresent( bool value );

    std::string relativePath( const std::string& path );
    void addFile( const std::string& filePath );
    void addFolder( const std::string& path );
    void removeFile( const std::string& filePath );
    void removeFolder( const std::string& filePath );
    std::shared_ptr<File> file( const std::string& filePath );
    std::shared_ptr<Directory> directory( const std::string& path );
    void setMountpointRoot( const std::string& path, std::shared_ptr<Directory> root );
    void invalidateMountpoint( const std::string& path );

private:
    std::string m_uuid;
    bool m_removable;
    bool m_present;
    std::string m_mountpoint;
    std::shared_ptr<Directory> m_root;
};

}
