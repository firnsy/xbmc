/*
 *      Copyright (C) 2005-2011 Team XBMC
 *      http://www.xbmc.org
 *
 *  This Program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2, or (at your option)
 *  any later version.
 *
 *  This Program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with XBMC; see the file COPYING.  If not, write to
 *  the Free Software Foundation, 675 Mass Ave, Cambridge, MA 02139, USA.
 *  http://www.gnu.org/copyleft/gpl.html
 *
 */

#include "utils/log.h"
#include "music/MusicDatabase.h"
#include "FileItem.h"
#include "music/tags/MusicInfoTag.h"
#include "RecentlyAddedMusicJob.h"
#include "guilib/GUIWindow.h"
#include "guilib/GUIWindowManager.h"
#include "guilib/Key.h"
#include "utils/Crc32.h"

CRecentlyAddedMusicJob::CRecentlyAddedMusicJob()
{
} 

bool CRecentlyAddedMusicJob::Update()
{
  CGUIWindow* home = g_windowManager.GetWindow(WINDOW_HOME);

  if ( home == NULL )
    return false;
 
  CLog::Log(LOGDEBUG, "CRecentlyAddedMusicJob::Update() - Running RecentlyAdded home screen update");
 
  CFileItemList  musicItems;
  CMusicDatabase musicdatabase;
  
  musicdatabase.Open();

  if (musicdatabase.GetRecentlyAddedAlbumSongs("musicdb://", musicItems, 5))
  {  
    for (int i = 0; i < musicItems.Size(); ++i)
    {  
      CFileItemPtr item = musicItems.Get(i);
      CStdString   value;
      value.Format("%i", i+1);
      
      CStdString   strThumb;
      CStdString   strAlbum  = item->GetMusicInfoTag()->GetAlbum();
      CStdString   strArtist = item->GetMusicInfoTag()->GetArtist();
      
      long idAlbum = musicdatabase.GetAlbumByName(strAlbum,strArtist);
      if (idAlbum != -1)
        musicdatabase.GetAlbumThumb(idAlbum,strThumb);
      
      home->SetProperty( "LatestSong." + value + ".Title"   , item->GetMusicInfoTag()->GetTitle());
      home->SetProperty( "LatestSong." + value + ".Year"    , item->GetMusicInfoTag()->GetYear());
      home->SetProperty( "LatestSong." + value + ".Artist"  , strArtist);      
      home->SetProperty( "LatestSong." + value + ".Album"   , strAlbum);
      home->SetProperty( "LatestSong." + value + ".Rating"  , item->GetMusicInfoTag()->GetRating());
      home->SetProperty( "LatestSong." + value + ".Path"    , item->GetMusicInfoTag()->GetURL());
      home->SetProperty( "LatestSong." + value + ".Thumb"   , strThumb);
      home->SetProperty( "LatestSong." + value + ".Fanart"  , item->GetCachedFanart());
    }
  }
  VECALBUMS albums;
  if (musicdatabase.GetRecentlyAddedAlbums(albums, 5))
    for (int i=0; i<(int)albums.size(); ++i)
    {
      CAlbum& album=albums[i];
      CStdString   value;
      value.Format("%i", i+1);
      
      CLog::Log(LOGDEBUG, "Album ID %i", album.idAlbum);

      CStdString   strPath;
      CStdString   strThumb;
      CStdString   strSQLAlbum;
      CStdString   strThumbPath;
      
      strSQLAlbum.Format("idAlbum=%i", album.idAlbum);
      
      int iYear    = atoi(musicdatabase.GetSingleValue("album", "iYear", strSQLAlbum));
      int iRating    = atoi(musicdatabase.GetSingleValue("albumview", "iRating", strSQLAlbum));
      musicdatabase.GetAlbumThumb(album.idAlbum,strThumb);
      Crc32 crc;
      musicdatabase.GetAlbumPath(album.idAlbum, strPath);
      crc.ComputeFromLowerCase(strPath);
      strThumbPath.Format("special://profile/Thumbnails/Music/Fanart/%08x.tbn", (unsigned __int32)crc);
      
      home->SetProperty( "LatestAlbum." + value + ".Title"   , musicdatabase.GetAlbumById(album.idAlbum));
      home->SetProperty( "LatestAlbum." + value + ".Year"    , iYear);
      home->SetProperty( "LatestAlbum." + value + ".Artist"  , musicdatabase.GetSingleValue("albumview", "strArtist", strSQLAlbum));      
      home->SetProperty( "LatestAlbum." + value + ".Rating"  , iRating);
      home->SetProperty( "LatestAlbum." + value + ".Path"    , strPath);
      home->SetProperty( "LatestAlbum." + value + ".Thumb"   , strThumb);
      home->SetProperty( "LatestAlbum." + value + ".Fanart"  , strThumbPath);
    }
  musicdatabase.Close();
  return true;
}

bool CRecentlyAddedMusicJob::DoWork()
{
  return Update(); 
}
