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

#ifndef FILESYSTEM_H
#define FILESYSTEM_H

#include <string>
#include <unordered_map>

#include "filesystem/IDirectory.h"
#include "filesystem/IFile.h"
#include "filesystem/IDevice.h"
#include "factory/IFileSystem.h"
#include "utils/Filename.h"

namespace mock
{

class File : public fs::IFile
{
public:
    File( const std::string& filePath )
        : m_name( utils::file::fileName( filePath ) )
        , m_path( utils::file::directory( filePath ) )
        , m_fullPath( filePath )
        , m_extension( utils::file::extension( filePath ) )
        , m_lastModification( 0 )
    {
    }

    File( const File& ) = default;

    virtual const std::string& name() const override
    {
        return m_name;
    }

    virtual const std::string& path() const override
    {
        return m_path;
    }

    virtual const std::string& fullPath() const override
    {
        return m_fullPath;
    }

    virtual const std::string& extension() const override
    {
        return m_extension;
    }

    virtual unsigned int lastModificationDate() const override
    {
        return m_lastModification;
    }

    void markAsModified()
    {
        m_lastModification++;
    }

    std::string m_name;
    std::string m_path;
    std::string m_fullPath;
    std::string m_extension;
    unsigned int m_lastModification;
};

class Device : public fs::IDevice
{
public:
    Device( const std::string& mountpoint, const std::string& uuid ) : m_uuid( uuid ),
        m_removable( false ), m_mountpoint( mountpoint ) {}
    virtual const std::string& uuid() const override { return m_uuid; }
    virtual bool isRemovable() const override { return m_removable; }
    virtual const std::string& mountpoint() const override { return m_mountpoint; }

    void setRemovable( bool value ) { m_removable = value; }

private:
    std::string m_uuid;
    bool m_removable;
    std::string m_mountpoint;
};

class Directory : public fs::IDirectory
{
public:
    Directory( std::shared_ptr<mock::Directory> parent, const std::string& path, unsigned int lastModif, std::shared_ptr<fs::IDevice> device  )
        : m_path( path )
        , m_parent( parent )
        , m_lastModificationDate( lastModif )
        , m_isRemovable( false )
        , m_device( device )
    {
    }

    virtual const std::string& path() const override
    {
        return m_path;
    }

    virtual const std::vector<std::string>& files() override
    {
        return m_files;
    }

    virtual const std::vector<std::string>& dirs() override
    {
        return m_dirs;
    }

    virtual unsigned int lastModificationDate() const override
    {
        return m_lastModificationDate;
    }

    virtual std::shared_ptr<fs::IDevice> device() const override
    {
        return m_device;
    }

    void removeDevice()
    {
        m_device = nullptr;
    }

    void addFile( const std::string& fileName )
    {
        m_files.emplace_back( m_path + fileName );
        markAsModified();
    }

    void addFolder( const std::string& folder )
    {
        m_dirs.emplace_back( m_path + folder );
        markAsModified();
    }

    void removeFile( const std::string& fileName )
    {
        auto it = std::find( begin( m_files ), end( m_files ), fileName );
        if ( it == end( m_files ) )
            throw std::runtime_error("Invalid filename");
        m_files.erase( it );
        markAsModified();
    }

    void remove()
    {
        if ( m_parent == nullptr )
            return;
        m_parent->removeFolder( m_path );
    }

    void removeFolder( const std::string& path )
    {
        auto it = std::find( begin( m_dirs ), end( m_dirs ), path );
        if ( it == end( m_dirs ) )
            throw std::runtime_error( "Invalid subfolder to remove" );
        m_dirs.erase( it );
        markAsModified();
    }

    void markAsModified()
    {
        if ( m_parent != nullptr )
            m_parent->markAsModified();
        m_lastModificationDate++;
    }

    virtual bool isRemovable() const override
    {
        return m_isRemovable;
    }

    void markRemovable()
    {
        m_isRemovable = true;
    }

private:
    std::string m_path;
    std::vector<std::string> m_files;
    std::vector<std::string> m_dirs;
    std::shared_ptr<mock::Directory> m_parent;
    unsigned int m_lastModificationDate;
    bool m_isRemovable;
    std::shared_ptr<fs::IDevice> m_device;
};


struct FileSystemFactory : public factory::IFileSystem
{
    static constexpr const char* Root = "/a/";
    static constexpr const char* SubFolder = "/a/folder/";

    FileSystemFactory()
    {
        rootDevice = std::make_shared<Device>( std::string{ Root }, "root" );
        removableDevice = std::make_shared<Device>( std::string{ SubFolder }, "removable" );
        removableDevice->setRemovable( true );
        dirs[Root] = std::unique_ptr<mock::Directory>( new Directory{ nullptr, Root, 123, rootDevice  } );
            addFile( Root, "video.avi" );
            addFile( Root, "audio.mp3" );
            addFile( Root, "not_a_media.something" );
            addFile( Root, "some_other_file.seaotter" );
            addFolder( Root, "folder/", 456, removableDevice  );
                addFile( SubFolder, "subfile.mp4" );
    }

    void addFile( const std::string& path, const std::string& fileName )
    {
        dirs[path]->addFile( fileName );
        files[path + fileName] = std::unique_ptr<mock::File>( new mock::File( path + fileName ) );
    }

    void addFolder( const std::string& parentPath, const std::string& path, unsigned int lastModif, std::shared_ptr<fs::IDevice> device )
    {
        auto parent = dirs[parentPath];
        parent->addFolder( path );
        dirs[parentPath + path] = std::unique_ptr<mock::Directory>( new Directory( parent, parentPath + path, lastModif, device ) );
    }

    void removeFile( const std::string& path, const std::string& fileName )
    {
        auto it = files.find( path + fileName );
        if ( it == end( files ) )
            throw std::runtime_error( "Invalid file to remove" );
        files.erase( it );
        dirs[path]->removeFile( path + fileName );
    }

    void removeFolder( const std::string& path )
    {
        auto it = dirs.find( path );
        if ( it == end( dirs ) )
            throw std::runtime_error( "Invalid directory to remove" );
        for (const auto& f : it->second->files() )
        {
            removeFile( path, utils::file::fileName( f ) );
        }
        it->second->remove();
        dirs.erase( it );
    }

    std::shared_ptr<Directory> directory( const std::string& path )
    {
        auto it = dirs.find( path );
        if ( it != end( dirs ) )
            return it->second;
        return nullptr;
    }

    virtual std::shared_ptr<fs::IDirectory> createDirectory(const std::string& path) override
    {
        mock::Directory* res = nullptr;
        if ( path == "." )
        {
            res = dirs[Root].get();
        }
        else
        {
            auto it = dirs.find( path );
            if ( it != end( dirs ) )
                res = it->second.get();
        }
        if ( res == nullptr )
            throw std::runtime_error("Invalid path");
        return std::shared_ptr<fs::IDirectory>( new Directory( *res ) );
    }

    virtual std::unique_ptr<fs::IFile> createFile( const std::string &filePath ) override
    {
        const auto it = files.find( filePath );
        if ( it == end( files ) )
            files[filePath].reset( new File( filePath ) );
        return std::unique_ptr<fs::IFile>( new File( static_cast<const mock::File&>( * it->second.get() ) ) );
    }

    virtual std::shared_ptr<fs::IDevice> createDevice( const std::string& uuid ) override
    {
        if ( uuid == "root" )
            return rootDevice;
        if ( uuid == "removable" )
            return removableDevice;
        return nullptr;
    }

    std::unordered_map<std::string, std::shared_ptr<mock::File>> files;
    std::unordered_map<std::string, std::shared_ptr<mock::Directory>> dirs;
    std::shared_ptr<Device> rootDevice;
    std::shared_ptr<Device> removableDevice;
};

// Noop FS (basically just returns file names, and don't try to access those.)
class NoopFile : public fs::IFile
{
    std::string m_path;
    std::string m_fileName;
    std::string m_extension;

public:
    NoopFile( const std::string& file )
        : m_path( file )
        , m_fileName( utils::file::fileName( file ) )
        , m_extension( utils::file::extension( file ) )
    {
    }

    virtual const std::string& name() const
    {
        return m_fileName;
    }

    virtual const std::string& path() const
    {
        return m_path;
    }

    virtual const std::string& fullPath() const
    {
        return m_path;
    }

    virtual const std::string& extension() const
    {
        return m_extension;
    }

    virtual unsigned int lastModificationDate() const
    {
        // Ensure a non-0 value so tests can easily verify that the value
        // is initialized
        return 123;
    }
};

class NoopFsFactory : public factory::IFileSystem
{
public:
    virtual std::shared_ptr<fs::IDirectory> createDirectory( const std::string& ) override
    {
        return nullptr;
    }

    virtual std::unique_ptr<fs::IFile> createFile( const std::string &fileName ) override
    {
        return std::unique_ptr<fs::IFile>( new NoopFile( fileName ) );
    }

    virtual std::shared_ptr<fs::IDevice> createDevice( const std::string& ) override
    {
        return nullptr;
    }
};

}


#endif // FILESYSTEM_H

