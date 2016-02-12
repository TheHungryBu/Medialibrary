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

#include "Tests.h"

#include "Artist.h"
#include "Album.h"
#include "AlbumTrack.h"
#include "Media.h"

class Artists : public Tests
{
};

TEST_F( Artists, Create )
{
    auto a = ml->createArtist( "Flying Otters" );
    ASSERT_NE( a, nullptr );
    ASSERT_EQ( a->name(), "Flying Otters" );

    Reload();

    auto a2 = ml->artist( "Flying Otters" );
    ASSERT_NE( a2, nullptr );
    ASSERT_EQ( a2->name(), "Flying Otters" );
}

TEST_F( Artists, CreateDefaults )
{
    // Ensure this won't fail due to duplicate insertions
    // We just reload, which will call the initialization routine again.
    // This is implicitely tested by all other tests, though it seems better
    // to have an explicit one. We might also just run the request twice from here
    // sometime in the future.
    Reload();
}

TEST_F( Artists, ShortBio )
{
    auto a = ml->createArtist( "Raging Otters" );
    ASSERT_NE( a, nullptr );
    ASSERT_EQ( a->shortBio(), "" );

    std::string bio("An otter based post-rock band");
    a->setShortBio( bio );
    ASSERT_EQ( a->shortBio(), bio );

    Reload();

    auto a2 = ml->artist( "Raging Otters" );
    ASSERT_NE( a2, nullptr );
    ASSERT_EQ( a2->shortBio(), bio );
}

TEST_F( Artists, ArtworkMrl )
{
    auto a = ml->createArtist( "Dream seaotter" );
    ASSERT_NE( a, nullptr );
    ASSERT_EQ( a->artworkMrl(), "" );

    std::string artwork("/tmp/otter.png");
    a->setArtworkMrl( artwork );
    ASSERT_EQ( a->artworkMrl(), artwork );

    Reload();

    auto a2 = ml->artist( "Dream seaotter" );
    ASSERT_NE( a2, nullptr );
    ASSERT_EQ( a2->artworkMrl(), artwork );
}

TEST_F( Artists, Albums )
{
    auto artist = ml->createArtist( "Cannibal Otters" );
    auto album1 = ml->createAlbum( "album1" );
    auto album2 = ml->createAlbum( "album2" );

    ASSERT_NE( artist, nullptr );
    ASSERT_NE( album1, nullptr );
    ASSERT_NE( album2, nullptr );

    album1->setAlbumArtist( artist.get() );
    album2->setAlbumArtist( artist.get() );

    auto albums = artist->albums( medialibrary::SortingCriteria::Default, false );
    ASSERT_EQ( albums.size(), 2u );

    Reload();

    auto artist2 = ml->artist( "Cannibal Otters" );
    auto albums2 = artist2->albums( medialibrary::SortingCriteria::Default, false );
    ASSERT_EQ( albums.size(), 2u );
}

TEST_F( Artists, AllSongs )
{
    auto artist = ml->createArtist( "Cannibal Otters" );
    ASSERT_NE( artist, nullptr );

    for (auto i = 1; i <= 3; ++i)
    {
        auto f = ml->addFile( "song" + std::to_string(i) + ".mp3" );
        auto res = artist->addMedia( *f );
        ASSERT_TRUE( res );
    }

    auto songs = artist->media( medialibrary::SortingCriteria::Default, false );
    ASSERT_EQ( songs.size(), 3u );

    Reload();

    auto artist2 = ml->artist( "Cannibal Otters" );
    songs = artist2->media( medialibrary::SortingCriteria::Default, false );
    ASSERT_EQ( songs.size(), 3u );
}

TEST_F( Artists, GetAll )
{
    auto artists = ml->artists();
    // Ensure we don't include Unknown Artist // Various Artists
    ASSERT_EQ( artists.size(), 0u );

    for ( int i = 0; i < 5; i++ )
    {
        auto a = ml->createArtist( std::to_string( i ) );
        auto alb = ml->createAlbum( std::to_string( i ) );
        ASSERT_NE( nullptr, alb );
        alb->setAlbumArtist( a.get() );
        ASSERT_NE( a, nullptr );
    }
    artists = ml->artists();
    ASSERT_EQ( artists.size(), 5u );

    Reload();

    auto artists2 = ml->artists();
    ASSERT_EQ( artists2.size(), 5u );
}

TEST_F( Artists, UnknownAlbum )
{
    auto a = ml->createArtist( "Explotters in the sky" );
    auto album = a->unknownAlbum();
    auto album2 = a->unknownAlbum();

    ASSERT_NE( nullptr, album );
    ASSERT_NE( nullptr, album2 );
    ASSERT_EQ( album->id(), album2->id() );

    Reload();

    a = std::static_pointer_cast<Artist>( ml->artist( a->name() ) );
    album2 = a->unknownAlbum();
    ASSERT_NE( nullptr, album2 );
    ASSERT_EQ( album2->id(), album->id() );
}

TEST_F( Artists, MusicBrainzId )
{
    auto a = ml->createArtist( "Otters Never Say Die" );
    ASSERT_NE( a, nullptr );
    ASSERT_EQ( a->musicBrainzId(), "" );

    std::string mbId("{this-id-an-id}");
    a->setMusicBrainzId( mbId );
    ASSERT_EQ( a->musicBrainzId(), mbId );

    Reload();

    auto a2 = ml->artist( "Otters Never Say Die" );
    ASSERT_NE( a2, nullptr );
    ASSERT_EQ( a2->musicBrainzId(), mbId );
}

TEST_F( Artists, Search )
{
    ml->createArtist( "artist 1" );
    ml->createArtist( "artist 2" );
    ml->createArtist( "dream seaotter" );

    auto artists = ml->searchArtists( "artist" );
    ASSERT_EQ( 2u, artists.size() );
}

TEST_F( Artists, SearchAfterDelete )
{
    auto a = ml->createArtist( "artist 1" );
    ml->createArtist( "artist 2" );
    ml->createArtist( "dream seaotter" );

    auto artists = ml->searchArtists( "artist" );
    ASSERT_EQ( 2u, artists.size() );

    ml->deleteArtist( a->id() );

    artists = ml->searchArtists( "artist" );
    ASSERT_EQ( 1u, artists.size() );
}

TEST_F( Artists, SortMedia )
{
    auto artist = ml->createArtist( "Russian Otters" );

    for (auto i = 1; i <= 3; ++i)
    {
        auto f = ml->addFile( "song" + std::to_string(i) + ".mp3" );
        f->setDuration( 10 - i );
        f->save();
        artist->addMedia( *f );
    }

    auto tracks = artist->media( medialibrary::SortingCriteria::Duration, false );
    ASSERT_EQ( 3u, tracks.size() );
    ASSERT_EQ( "song3.mp3", tracks[0]->title() ); // Duration: 8
    ASSERT_EQ( "song2.mp3", tracks[1]->title() ); // Duration: 9
    ASSERT_EQ( "song1.mp3", tracks[2]->title() ); // Duration: 10

    tracks = artist->media( medialibrary::SortingCriteria::Duration, true );
    ASSERT_EQ( 3u, tracks.size() );
    ASSERT_EQ( "song1.mp3", tracks[0]->title() );
    ASSERT_EQ( "song2.mp3", tracks[1]->title() );
    ASSERT_EQ( "song3.mp3", tracks[2]->title() );
}

TEST_F( Artists, SortAlbum )
{
    auto artist = ml->createArtist( "Dream Seaotter" );
    auto album1 = ml->createAlbum( "album1" );
    album1->setReleaseYear( 2000, false );
    auto album2 = ml->createAlbum( "album2" );
    album2->setReleaseYear( 1000, false );
    auto album3 = ml->createAlbum( "album3" );
    album3->setReleaseYear( 2000, false );

    album1->setAlbumArtist( artist.get() );
    album2->setAlbumArtist( artist.get() );
    album3->setAlbumArtist( artist.get() );

    // Default order is by descending year, discriminated by lexical order
    auto albums = artist->albums( medialibrary::SortingCriteria::Default, false );
    ASSERT_EQ( 3u, albums.size() );
    ASSERT_EQ( album1->id(), albums[0]->id() );
    ASSERT_EQ( album3->id(), albums[1]->id() );
    ASSERT_EQ( album2->id(), albums[2]->id() );

    albums = artist->albums( medialibrary::SortingCriteria::Default, true );
    ASSERT_EQ( 3u, albums.size() );
    ASSERT_EQ( album2->id(), albums[0]->id() );
    ASSERT_EQ( album1->id(), albums[1]->id() );
    ASSERT_EQ( album3->id(), albums[2]->id() );

    albums = artist->albums( medialibrary::SortingCriteria::Alpha, false );
    ASSERT_EQ( 3u, albums.size() );
    ASSERT_EQ( album1->id(), albums[0]->id() );
    ASSERT_EQ( album2->id(), albums[1]->id() );
    ASSERT_EQ( album3->id(), albums[2]->id() );

    albums = artist->albums( medialibrary::SortingCriteria::Alpha, true );
    ASSERT_EQ( 3u, albums.size() );
    ASSERT_EQ( album3->id(), albums[0]->id() );
    ASSERT_EQ( album2->id(), albums[1]->id() );
    ASSERT_EQ( album1->id(), albums[2]->id() );
}
