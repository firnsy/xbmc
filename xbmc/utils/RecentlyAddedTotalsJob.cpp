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
#include "video/VideoDatabase.h"
#include "music/MusicDatabase.h"
#include "FileItem.h"
#include "RecentlyAddedTotalsJob.h"
#include "guilib/GUIWindow.h"
#include "guilib/GUIWindowManager.h"
#include "guilib/Key.h"

CRecentlyAddedTotalsJob::CRecentlyAddedTotalsJob()
{
} 

bool CRecentlyAddedTotalsJob::Update()
{
  CGUIWindow* home = g_windowManager.GetWindow(WINDOW_HOME);

  if ( home == NULL )
    return false;

  CLog::Log(LOGDEBUG, "CRecentlyAddedTotalsJob::Update() - Running RecentlyAdded home screen update");
 
  CStdString     EpCount;
  CStdString     EpWatched;
  CStdString     TvShowsWatched;
  CVideoDatabase videodatabase;  
  CMusicDatabase musicdatabase;
  
  musicdatabase.Open();
  videodatabase.Open();
  
  int tvShowCount     = atoi(videodatabase.GetSingleValue("tvshowview"     , "count(1)"));
  int movieTotals     = atoi(videodatabase.GetSingleValue("movieview"      , "count(1)"));
  int movieWatched    = atoi(videodatabase.GetSingleValue("movieview"      , "count(playCount)"));
  int MusVidTotals    = atoi(videodatabase.GetSingleValue("musicvideoview" , "count(1)"));
  int MusVidWatched   = atoi(videodatabase.GetSingleValue("musicvideoview" , "count(playCount)"));
  int MusSongTotals   = atoi(musicdatabase.GetSingleValue("songview"       , "count(1)"));
  int MusAlbumTotals  = atoi(musicdatabase.GetSingleValue("songview"       , "count(distinct strAlbum)"));
  int MusArtistTotals = atoi(musicdatabase.GetSingleValue("songview"       , "count(distinct strArtist)"));
  
  videodatabase.GetArbitraryQuery("SELECT sum(watchedcount) FROM tvshowview", "", "", "", "", "", "", EpWatched);
  videodatabase.GetArbitraryQuery("SELECT sum(totalcount) FROM tvshowview", "", "", "", "", "", "", EpCount);
  videodatabase.GetArbitraryQuery("SELECT sum(watchedcount = totalcount) FROM tvshowview", "", "", "", "", "", "", TvShowsWatched);

  musicdatabase.Close();
  videodatabase.Close();
  
  home->SetProperty( "TVShows.Count"         , tvShowCount);
  home->SetProperty( "TVShows.Watched"       , atoi(TvShowsWatched));
  home->SetProperty( "TVShows.UnWatched"     , tvShowCount - atoi(TvShowsWatched));
  home->SetProperty( "Episodes.Count"        , atoi(EpCount));
  home->SetProperty( "Episodes.Watched"      , atoi(EpWatched));
  home->SetProperty( "Episodes.UnWatched"    , atoi(EpCount)-atoi(EpWatched));  
  home->SetProperty( "Movies.Count"          , movieTotals);
  home->SetProperty( "Movies.Watched"        , movieWatched);
  home->SetProperty( "Movies.UnWatched"      , movieTotals - movieWatched);
  home->SetProperty( "MusicVideos.Count"     , MusVidTotals);
  home->SetProperty( "MusicVideos.Watched"   , MusVidWatched);
  home->SetProperty( "MusicVideos.UnWatched" , MusVidTotals - MusVidWatched);
  home->SetProperty( "Music.SongsCount"      , MusSongTotals);
  home->SetProperty( "Music.AlbumsCount"     , MusAlbumTotals);
  home->SetProperty( "Music.ArtistsCount"    , MusArtistTotals);
  
  return true;
}

bool CRecentlyAddedTotalsJob::DoWork()
{
  return Update(); 
}
