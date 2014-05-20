#include <cassert>
#include <cstdlib>
#include <cstring>

#include "Label.h"
#include "File.h"
#include "SqliteTools.h"

const std::string policy::LabelTable::Name = "Label";
const std::string policy::LabelTable::CacheColumn = "name";

Label::Label( sqlite3* dbConnection, sqlite3_stmt* stmt )
    : m_dbConnection( dbConnection )
    , m_files( NULL )
{
    m_id = sqlite3_column_int( stmt, 0 );
    m_name = (const char*)sqlite3_column_text( stmt, 1 );
}

Label::Label( const std::string& name )
    : m_dbConnection( NULL )
    , m_id( 0 )
    , m_name( name )
    , m_files( NULL )
{
}

unsigned int Label::id() const
{
    return m_id;
}


const std::string& Label::name()
{
    return m_name;
}

std::vector<FilePtr>& Label::files()
{
    if ( m_files == nullptr )
    {
        m_files = new std::vector<std::shared_ptr<IFile>>;
        static const std::string req = "SELECT f.* FROM " + policy::FileTable::Name + " f "
                "LEFT JOIN LabelFileRelation lfr ON lfr.id_file = f.id_file "
                "WHERE lfr.id_label = ?";
        SqliteTools::fetchAll<File>( m_dbConnection, req.c_str(), *m_files, m_id );
    }
    return *m_files;
}

bool Label::insert( sqlite3* dbConnection )
{
    assert( m_dbConnection == nullptr );
    assert( m_id == 0 );
    const char* req = "INSERT INTO Label VALUES(NULL, ?)";
    if ( SqliteTools::executeRequest( dbConnection, req, m_name ) == false )
        return false;
    m_dbConnection = dbConnection;
    m_id = sqlite3_last_insert_rowid( dbConnection );
    return true;
}

bool Label::createTable(sqlite3* dbConnection)
{
    std::string req = "CREATE TABLE IF NOT EXISTS " + policy::LabelTable::Name + "("
                "id_label INTEGER PRIMARY KEY AUTOINCREMENT, "
                "name TEXT UNIQUE ON CONFLICT FAIL"
            ")";
    if ( SqliteTools::executeRequest( dbConnection, req.c_str() ) == false )
        return false;
    req = "CREATE TABLE IF NOT EXISTS LabelFileRelation("
                "id_label INTEGER,"
                "id_file INTEGER,"
            "PRIMARY KEY (id_label, id_file)"
            "FOREIGN KEY(id_label) REFERENCES Label(id_label) ON DELETE CASCADE,"
            "FOREIGN KEY(id_file) REFERENCES File(id_file) ON DELETE CASCADE);";
    return SqliteTools::executeRequest( dbConnection, req.c_str() );
}



const std::string&policy::LabelCachePolicy::key( const std::shared_ptr<ILabel> self )
{
    return self->name();
}

std::string policy::LabelCachePolicy::key(sqlite3_stmt* stmt)
{
    return Traits<KeyType>::Load( stmt, 1 );
}