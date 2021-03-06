CREATE TABLE Album(id_album INTEGER PRIMARY KEY AUTOINCREMENT,title TEXT COLLATE NOCASE,artist_id UNSIGNED INTEGER,release_year UNSIGNED INTEGER,short_summary TEXT,artwork_mrl TEXT,nb_tracks UNSIGNED INTEGER DEFAULT 0,duration UNSIGNED INTEGER NOT NULL DEFAULT 0,is_present BOOLEAN NOT NULL DEFAULT 1,FOREIGN KEY( artist_id ) REFERENCES Artist(id_artist) ON DELETE CASCADE);
CREATE INDEX album_artist_id_idx ON Album(artist_id);
CREATE TABLE AlbumArtistRelation(album_id INTEGER,artist_id INTEGER,PRIMARY KEY (album_id, artist_id),FOREIGN KEY(album_id) REFERENCES Album(id_album) ON DELETE CASCADE,FOREIGN KEY(artist_id) REFERENCES Artist(id_artist) ON DELETE CASCADE);
CREATE TABLE AlbumTrack(id_track INTEGER PRIMARY KEY AUTOINCREMENT,media_id INTEGER,duration INTEGER NOT NULL,artist_id UNSIGNED INTEGER,genre_id INTEGER,track_number UNSIGNED INTEGER,album_id UNSIGNED INTEGER NOT NULL,disc_number UNSIGNED INTEGER,is_present BOOLEAN NOT NULL DEFAULT 1,FOREIGN KEY (media_id) REFERENCES Media(id_media) ON DELETE CASCADE,FOREIGN KEY (artist_id) REFERENCES Artist(id_artist) ON DELETE CASCADE,FOREIGN KEY (genre_id) REFERENCES Genre(id_genre),FOREIGN KEY (album_id) REFERENCES Album(id_album)  ON DELETE CASCADE);
CREATE INDEX album_media_artist_genre_album_idx ON AlbumTrack(media_id, artist_id, genre_id, album_id);
CREATE TABLE Artist(id_artist INTEGER PRIMARY KEY AUTOINCREMENT,name TEXT COLLATE NOCASE UNIQUE ON CONFLICT FAIL,shortbio TEXT,artwork_mrl TEXT,nb_albums UNSIGNED INT DEFAULT 0,mb_id TEXT,is_present BOOLEAN NOT NULL DEFAULT 1);
INSERT INTO Artist(id_artist,name,shortbio,artwork_mrl,nb_albums,mb_id,is_present) VALUES(1,NULL,NULL,NULL,0,NULL,1);
INSERT INTO Artist(id_artist,name,shortbio,artwork_mrl,nb_albums,mb_id,is_present) VALUES(2,NULL,NULL,NULL,0,NULL,1);
CREATE TABLE AudioTrack(id_track INTEGER PRIMARY KEY AUTOINCREMENT,codec TEXT,bitrate UNSIGNED INTEGER,samplerate UNSIGNED INTEGER,nb_channels UNSIGNED INTEGER,language TEXT,description TEXT,media_id UNSIGNED INT,FOREIGN KEY ( media_id ) REFERENCES Media( id_media ) ON DELETE CASCADE);
CREATE INDEX audio_track_media_idx ON AudioTrack(media_id);
CREATE TABLE Device(id_device INTEGER PRIMARY KEY AUTOINCREMENT,uuid TEXT UNIQUE ON CONFLICT FAIL,scheme TEXT,is_removable BOOLEAN,is_present BOOLEAN);
INSERT INTO Device(id_device,uuid,scheme,is_removable,is_present) VALUES(1,'wwwwwwww-xxxx-cccc-vvvv-bbbbbbbbbbbb','file://',0,1);
CREATE TABLE File(id_file INTEGER PRIMARY KEY AUTOINCREMENT,media_id UNSIGNED INT DEFAULT NULL,playlist_id UNSIGNED INT DEFAULT NULL,mrl TEXT,type UNSIGNED INTEGER,last_modification_date UNSIGNED INT,size UNSIGNED INT,parser_step INTEGER NOT NULL DEFAULT 0,parser_retries INTEGER NOT NULL DEFAULT 0,folder_id UNSIGNED INTEGER,is_present BOOLEAN NOT NULL DEFAULT 1,is_removable BOOLEAN NOT NULL,is_external BOOLEAN NOT NULL,FOREIGN KEY (media_id) REFERENCES Media(id_media) ON DELETE CASCADE,FOREIGN KEY (playlist_id) REFERENCES Playlist(id_playlist) ON DELETE CASCADE,FOREIGN KEY (folder_id) REFERENCES Folder(id_folder) ON DELETE CASCADE,UNIQUE( mrl, folder_id ) ON CONFLICT FAIL);
INSERT INTO File(id_file,media_id,playlist_id,mrl,type,last_modification_date,size,parser_step,parser_retries,folder_id,is_present,is_removable,is_external) VALUES(1,1,NULL,'file:///empty/empty.m3u8',1,1512482997,20,0,1,1,1,0,0);
CREATE INDEX file_media_id_index ON File(media_id);
CREATE INDEX file_folder_id_index ON File(folder_id);
CREATE TABLE Folder(id_folder INTEGER PRIMARY KEY AUTOINCREMENT,path TEXT,parent_id UNSIGNED INTEGER,is_blacklisted BOOLEAN NOT NULL DEFAULT 0,device_id UNSIGNED INTEGER,is_present BOOLEAN NOT NULL DEFAULT 1,is_removable BOOLEAN NOT NULL,FOREIGN KEY (parent_id) REFERENCES Folder(id_folder) ON DELETE CASCADE,FOREIGN KEY (device_id) REFERENCES Device(id_device) ON DELETE CASCADE,UNIQUE(path, device_id) ON CONFLICT FAIL);
INSERT INTO Folder(id_folder,path,parent_id,is_blacklisted,device_id,is_present,is_removable) VALUES(1,'file:///empty/',NULL,0,1,1,0);
CREATE INDEX folder_device_id_idx ON Folder (device_id);
CREATE INDEX parent_folder_id_idx ON Folder (parent_id);
CREATE TABLE Genre(id_genre INTEGER PRIMARY KEY AUTOINCREMENT,name TEXT UNIQUE ON CONFLICT FAIL,nb_tracks INTEGER NOT NULL DEFAULT 0);
CREATE TABLE History(id_media INTEGER PRIMARY KEY,insertion_date UNSIGNED INT NOT NULL,FOREIGN KEY (id_media) REFERENCES Media(id_media) ON DELETE CASCADE);
CREATE TABLE Label(id_label INTEGER PRIMARY KEY AUTOINCREMENT, name TEXT UNIQUE ON CONFLICT FAIL);
CREATE TABLE LabelFileRelation(label_id INTEGER,media_id INTEGER,PRIMARY KEY (label_id, media_id),FOREIGN KEY(label_id) REFERENCES Label(id_label) ON DELETE CASCADE,FOREIGN KEY(media_id) REFERENCES Media(id_media) ON DELETE CASCADE);
CREATE TABLE Media(id_media INTEGER PRIMARY KEY AUTOINCREMENT,type INTEGER,subtype INTEGER,duration INTEGER DEFAULT -1,play_count UNSIGNED INTEGER,last_played_date UNSIGNED INTEGER,insertion_date UNSIGNED INTEGER,release_date UNSIGNED INTEGER,thumbnail TEXT,title TEXT COLLATE NOCASE,filename TEXT,is_favorite BOOLEAN NOT NULL DEFAULT 0,is_present BOOLEAN NOT NULL DEFAULT 1);
INSERT INTO Media(id_media,type,subtype,duration,play_count,last_played_date,insertion_date,release_date,thumbnail,title,filename,is_favorite,is_present) VALUES(1,0,NULL,-1,NULL,NULL,1512483241,NULL,NULL,'empty.m3u8','empty.m3u8',0,1);
CREATE INDEX index_last_played_date ON Media(last_played_date DESC);
CREATE TABLE MediaArtistRelation(media_id INTEGER NOT NULL,artist_id INTEGER,PRIMARY KEY (media_id, artist_id),FOREIGN KEY(media_id) REFERENCES Media(id_media) ON DELETE CASCADE,FOREIGN KEY(artist_id) REFERENCES Artist(id_artist) ON DELETE CASCADE);
CREATE TABLE MediaMetadata(id_media INTEGER,type INTEGER,value TEXT,PRIMARY KEY (id_media, type));
CREATE TABLE Movie(id_movie INTEGER PRIMARY KEY AUTOINCREMENT,media_id UNSIGNED INTEGER NOT NULL,title TEXT UNIQUE ON CONFLICT FAIL,summary TEXT,artwork_mrl TEXT,imdb_id TEXT,FOREIGN KEY(media_id) REFERENCES Media(id_media) ON DELETE CASCADE);
CREATE INDEX movie_media_idx ON Movie(media_id);
CREATE TABLE Playlist(id_playlist INTEGER PRIMARY KEY AUTOINCREMENT,name TEXT UNIQUE,file_id UNSIGNED INT DEFAULT NULL,creation_date UNSIGNED INT NOT NULL,artwork_mrl TEXT,FOREIGN KEY (file_id) REFERENCES File(id_file) ON DELETE CASCADE);
CREATE TABLE PlaylistMediaRelation(media_id INTEGER,playlist_id INTEGER,position INTEGER,PRIMARY KEY(media_id, playlist_id),FOREIGN KEY(media_id) REFERENCES Media(id_media) ON DELETE CASCADE,FOREIGN KEY(playlist_id) REFERENCES Playlist(id_playlist) ON DELETE CASCADE);
CREATE TABLE Settings(db_model_version UNSIGNED INTEGER NOT NULL);
INSERT INTO Settings(rowid,db_model_version) VALUES(1,6);
CREATE TABLE Show(id_show INTEGER PRIMARY KEY AUTOINCREMENT,name TEXT, release_date UNSIGNED INTEGER,short_summary TEXT,artwork_mrl TEXT,tvdb_id TEXT);
CREATE TABLE ShowEpisode(id_episode INTEGER PRIMARY KEY AUTOINCREMENT,media_id UNSIGNED INTEGER NOT NULL,artwork_mrl TEXT,episode_number UNSIGNED INT,title TEXT,season_number UNSIGNED INT,episode_summary TEXT,tvdb_id TEXT,show_id UNSIGNED INT,FOREIGN KEY(media_id) REFERENCES Media(id_media) ON DELETE CASCADE,FOREIGN KEY(show_id) REFERENCES Show(id_show) ON DELETE CASCADE);
CREATE INDEX show_episode_media_show_idx ON ShowEpisode(media_id, show_id);
CREATE TABLE VideoTrack(id_track INTEGER PRIMARY KEY AUTOINCREMENT,codec TEXT,width UNSIGNED INTEGER,height UNSIGNED INTEGER,fps FLOAT,media_id UNSIGNED INT,language TEXT,description TEXT,FOREIGN KEY ( media_id ) REFERENCES Media(id_media) ON DELETE CASCADE);
CREATE INDEX video_track_media_idx ON VideoTrack(media_id);
