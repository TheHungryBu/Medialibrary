#include "Tests.h"

#include "IMediaLibrary.h"
#include "IFile.h"

class Files : public Tests
{
};


TEST_F( Files, Init )
{
    // only test for correct test fixture behavior
}

TEST_F( Files, Create )
{
    auto f = ml->addFile( "/dev/null" );
    ASSERT_NE( f, nullptr );

    ASSERT_EQ( f->playCount(), 0 );
    ASSERT_EQ( f->albumTrack(), nullptr );
    ASSERT_EQ( f->showEpisode(), nullptr );
    ASSERT_TRUE( f->isStandAlone() );

    auto files = ml->files();
    ASSERT_EQ( files.size(), 1u );
    ASSERT_EQ( files[0]->mrl(), f->mrl() );
}

TEST_F( Files, Fetch )
{
    auto f = ml->addFile( "/dev/null" );
    auto f2 = ml->file( "/dev/null" );
    ASSERT_EQ( f->mrl(), f2->mrl() );
    ASSERT_EQ( f, f2 );

    // Flush cache and fetch from DB
    Reload();

    f2 = ml->file( "/dev/null" );
    ASSERT_EQ( f->mrl(), f2->mrl() );
    ASSERT_TRUE( f2->isStandAlone() );
}

TEST_F( Files, Delete )
{
    auto f = ml->addFile( "/dev/loutre" );
    auto f2 = ml->file( "/dev/loutre" );

    ASSERT_EQ( f, f2 );

    ml->deleteFile( f );
    f2 = ml->file( "/dev/loutre" );
    ASSERT_EQ( f2, nullptr );
}

TEST_F( Files, Duplicate )
{
    auto f = ml->addFile( "/dev/moulaf" );
    auto f2 = ml->addFile( "/dev/moulaf" );

    ASSERT_NE( f, nullptr );
    ASSERT_EQ( f2, nullptr );

    f2 = ml->file( "/dev/moulaf" );
    ASSERT_EQ( f, f2 );
}

TEST_F( Files, LastModificationDate )
{
    auto f = ml->addFile( "/dev/seaotter" );
    ASSERT_NE( 0u, f->lastModificationDate() );

    Reload();
    auto f2 = ml->file( "/dev/seaotter" );
    ASSERT_EQ( f->lastModificationDate(), f2->lastModificationDate() );
}

TEST_F( Files, Duration )
{
    auto f = ml->addFile( "/some/file" );
    ASSERT_EQ( f->duration(), 0u );

    f->setDuration( 123u );
    ASSERT_EQ( f->duration(), 123u );

    Reload();

    f = ml->file( "/some/file" );
    ASSERT_EQ( f->duration(), 123u );
}

TEST_F( Files, Snapshot )
{
    auto f = ml->addFile( "/some/file" );
    ASSERT_EQ( f->snapshot(), "" );

    std::string newSnapshot( "/path/to/snapshot" );

    f->setSnapshot( newSnapshot );
    ASSERT_EQ( f->snapshot(), newSnapshot );

    Reload();

    f = ml->file( "/some/file" );
    ASSERT_EQ( f->snapshot(), newSnapshot );
}
