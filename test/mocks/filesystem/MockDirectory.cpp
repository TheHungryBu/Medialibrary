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

#if HAVE_CONFIG_H
# include "config.h"
#endif

#include <cassert>
#include <system_error>

#include "MockDirectory.h"
#include "MockFile.h"
#include "MockDevice.h"

#include "utils/Filename.h"

namespace mock
{

Directory::Directory( const std::string& mrl, std::shared_ptr<Device> device)
    : m_mrl( mrl )
    , m_device( device )
{
    if ( ( *m_mrl.crbegin() ) != '/' )
        m_mrl += '/';
}

const std::string& Directory::mrl() const
{
    return m_mrl;
}

const std::vector<std::shared_ptr<fs::IFile>>& Directory::files() const
{
    // Assume no device means a wrong path
    if ( m_device.lock() == nullptr )
        throw std::system_error( ENOENT, std::generic_category(), "Failed to open mock directory" );
    m_filePathes.clear();
    for ( auto& f : m_files )
        m_filePathes.push_back( f.second );
    return m_filePathes;
}

const std::vector<std::shared_ptr<fs::IDirectory>>& Directory::dirs() const
{
    if ( m_device.lock() == nullptr )
        throw std::system_error( ENOENT, std::generic_category(), "Failed to open mock directory" );
    m_dirPathes.clear();
    for ( const auto& d : m_dirs )
        m_dirPathes.push_back( d.second );
    return m_dirPathes;
}

std::shared_ptr<fs::IDevice> Directory::device() const
{
    return std::static_pointer_cast<fs::IDevice>( m_device.lock() );
}

void Directory::addFile(const std::string& filePath)
{
    auto subFolder = utils::file::firstFolder( filePath );
    if ( subFolder.empty() == true )
    {
        m_files[filePath] = std::make_shared<File>( m_mrl + filePath );
    }
    else
    {
        auto it = m_dirs.find( subFolder );
        assert( it != end( m_dirs ) );
        auto remainingPath = utils::file::removePath( filePath, subFolder );
        it->second->addFile( remainingPath );
    }
}

void Directory::addFolder(const std::string& folder)
{
    auto subFolder = utils::file::firstFolder( folder );
    auto remainingPath = utils::file::removePath( folder, subFolder );
    if ( remainingPath.empty() == true )
    {
        auto dir = std::make_shared<Directory>( m_mrl + subFolder, m_device.lock() );
        m_dirs[subFolder] = dir;
    }
    else
    {
        auto it = m_dirs.find( subFolder );
        assert( it != end( m_dirs ) );
        it->second->addFolder( remainingPath );
    }
}

void Directory::removeFile(const std::string& filePath)
{
    auto subFolder = utils::file::firstFolder( filePath );
    if ( subFolder.empty() == true )
    {
        auto it = m_files.find( filePath );
        assert( it != end( m_files ) );
        m_files.erase( it );
    }
    else
    {
        auto it = m_dirs.find( subFolder );
        assert( it != end( m_dirs ) );
        auto remainingPath = utils::file::removePath( filePath, subFolder );
        it->second->removeFile( remainingPath );
    }
}

std::shared_ptr<File> Directory::file(const std::string& filePath)
{
    auto subFolder = utils::file::firstFolder( filePath );
    if ( subFolder.empty() == true )
    {
        auto it = m_files.find( filePath );
        assert( it != end( m_files ) );
        return it->second;
    }
    else
    {
        auto it = m_dirs.find( subFolder );
        assert( it != end( m_dirs ) );
        auto remainingPath = utils::file::removePath( filePath, subFolder );
        return it->second->file( remainingPath );
    }
}

std::shared_ptr<Directory> Directory::directory(const std::string& path)
{
    auto subFolder = utils::file::firstFolder( path );
    auto remainingPath = utils::file::removePath( path, subFolder );
    if ( remainingPath.empty() == true )
    {
        auto it = m_dirs.find( subFolder );
        if ( it == end( m_dirs ) )
            return std::make_shared<Directory>( "", nullptr );
        return it->second;
    }
    else
    {
        auto it = m_dirs.find( subFolder );
        assert( it != end( m_dirs ) );
        return it->second->directory( remainingPath );
    }
}

void Directory::removeFolder(const std::string& path)
{
    auto subFolder = utils::file::firstFolder( path );
    auto remainingPath = utils::file::removePath( path, subFolder );
    if ( remainingPath.empty() == true )
    {
        auto it = m_dirs.find( subFolder );
        assert( it != end( m_dirs ) );
        m_dirs.erase( it );
    }
    else
    {
        auto it = m_dirs.find( subFolder );
        assert( it != end( m_dirs ) );
        it->second->removeFolder( remainingPath );
    }
}

void Directory::setMountpointRoot(const std::string& path, std::shared_ptr<Directory> root)
{
    auto subFolder = utils::file::firstFolder( path );
    auto remainingPath = utils::file::removePath( path, subFolder );
    if ( remainingPath.empty() == true )
    {
        m_dirs[subFolder] = root;
    }
    else
    {
        auto it = m_dirs.find( subFolder );
        assert( it != end( m_dirs ) );
        it->second->setMountpointRoot( remainingPath, root );
    }
}

void Directory::invalidateMountpoint(const std::string& path)
{
    auto subFolder = utils::file::firstFolder( path );
    auto remainingPath = utils::file::removePath( path, subFolder );
    if ( remainingPath.empty() == true )
    {
        m_dirs[subFolder] = std::make_shared<Directory>( m_mrl + subFolder, m_device.lock() );
    }
    else
    {
        auto it = m_dirs.find( subFolder );
        assert( it != end( m_dirs ) );
        it->second->invalidateMountpoint( remainingPath );
    }
}

}
