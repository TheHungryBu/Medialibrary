#ifndef AUDIOTRACK_H
#define AUDIOTRACK_H

#include "IAudioTrack.h"
#include "IMediaLibrary.h"
#include "Cache.h"

class AudioTrack;

namespace policy
{
struct AudioTrackTable
{
    static const std::string Name;
    static const std::string CacheColumn;
    static unsigned int AudioTrack::* const PrimaryKey;
};
}

class AudioTrack : public IAudioTrack, public Cache<AudioTrack, IAudioTrack, policy::AudioTrackTable>
{
    public:
        AudioTrack( DBConnection dbConnection, sqlite3_stmt* stmt );
        AudioTrack( const std::string& codec, unsigned int bitrate, unsigned int sampleRate, unsigned int nbChannels );

        virtual unsigned int id() const;
        virtual const std::string&codec() const;
        virtual unsigned int bitrate() const;
        virtual unsigned int sampleRate() const;
        virtual unsigned int nbChannels() const;

        static bool createTable( DBConnection dbConnection );
        static AudioTrackPtr fetch( DBConnection dbConnection, const std::string& codec,
                                    unsigned int bitrate, unsigned int sampleRate, unsigned int nbChannels );
        static AudioTrackPtr create(DBConnection dbConnection, const std::string& codec, unsigned int bitrate , unsigned int sampleRate, unsigned int nbChannels);

    private:
        DBConnection m_dbConnection;
        unsigned int m_id;
        const std::string m_codec;
        const unsigned int m_bitrate;
        const unsigned int m_sampleRate;
        const unsigned int m_nbChannels;


    private:
        typedef Cache<AudioTrack, IAudioTrack, policy::AudioTrackTable> _Cache;
        friend struct policy::AudioTrackTable;
};

#endif // AUDIOTRACK_H