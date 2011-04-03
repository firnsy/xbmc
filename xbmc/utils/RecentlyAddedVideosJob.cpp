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
#include "video/VideoInfoTag.h"
#include "FileItem.h"
#include "RecentlyAddedVideosJob.h"
#include "guilib/GUIWindow.h"
#include "guilib/GUIWindowManager.h"
#include "guilib/Key.h"

CRecentlyAddedVideosJob::CRecentlyAddedVideosJob()
{
} 

bool CRecentlyAddedVideosJob::Update()
{
  CGUIWindow* home = g_windowManager.GetWindow(WINDOW_HOME);

  if ( home == NULL )
    return false;

  CLog::Log(LOGDEBUG, "CRecentlyAddedVideosJob::Update() - Running RecentlyAdded home screen update");
 
  CFileItemList  items;
  CVideoDatabase videodatabase;
  
  videodatabase.Open();

  if (videodatabase.GetRecentlyAddedMoviesNav("videodb://1/", items, 5))
  {  
    for (int i = 0; i < items.Size(); ++i)
    {  
      CFileItemPtr item = items.Get(i);
      CStdString   value;
      CStdString   strRating;
     
      value.Format("%i", i+1);
      strRating.Format("%.1f", item->GetVideoInfoTag()->m_fRating);
      
      home->SetProperty( "LatestMovie." + value + ".Title"       , item->GetLabel());
      home->SetProperty( "LatestMovie." + value + ".Thumb"       , item->GetThumbnailImage());
      home->SetProperty( "LatestMovie." + value + ".Rating"      , atoi(strRating));
      home->SetProperty( "LatestMovie." + value + ".Year"        , item->GetVideoInfoTag()->m_iYear);
      home->SetProperty( "LatestMovie." + value + ".Plot"        , item->GetVideoInfoTag()->m_strPlot);
      home->SetProperty( "LatestMovie." + value + ".RunningTime" , item->GetVideoInfoTag()->m_strRuntime);
      home->SetProperty( "LatestMovie." + value + ".Path"        , item->GetVideoInfoTag()->m_strFileNameAndPath);
      home->SetProperty( "LatestMovie." + value + ".Trailer"     , item->GetVideoInfoTag()->m_strTrailer);
      home->SetProperty( "LatestMovie." + value + ".Fanart"      , item->GetCachedFanart());
    }
  }

  CFileItemList  TVShowItems;  
  if (videodatabase.GetRecentlyAddedEpisodesNav("videodb://1/", TVShowItems, 5))
  {  
    for (int i = 0; i < TVShowItems.Size(); ++i)
    { 
      CFileItemPtr item          = TVShowItems.Get(i);
      int          EpisodeSeason = item->GetVideoInfoTag()->m_iSeason;
      int          EpisodeNumber = item->GetVideoInfoTag()->m_iEpisode;
      CStdString   EpisodeNo;
      CStdString   value;
      CStdString   strRating;
     
      EpisodeNo.Format("s%02de%02d", EpisodeSeason, EpisodeNumber);
      value.Format("%i", i+1);
      strRating.Format("%.1f", item->GetVideoInfoTag()->m_fRating);
      
      home->SetProperty( "LatestEpisode." + value + ".ShowTitle"     , item->GetVideoInfoTag()->m_strShowTitle);
      home->SetProperty( "LatestEpisode." + value + ".EpisodeTitle"  , item->GetVideoInfoTag()->m_strTitle);
      home->SetProperty( "LatestEpisode." + value + ".Rating"        , atoi(strRating));      
      home->SetProperty( "LatestEpisode." + value + ".Plot"          , item->GetVideoInfoTag()->m_strPlot);
      home->SetProperty( "LatestEpisode." + value + ".EpisodeNo"     , EpisodeNo);
      home->SetProperty( "LatestEpisode." + value + ".EpisodeSeason" , EpisodeSeason);
      home->SetProperty( "LatestEpisode." + value + ".EpisodeNumber" , EpisodeNumber);
      home->SetProperty( "LatestEpisode." + value + ".Path"          , item->GetVideoInfoTag()->m_strFileNameAndPath);
      home->SetProperty( "LatestEpisode." + value + ".Thumb"         , item->GetCachedVideoThumb());
      home->SetProperty( "LatestEpisode." + value + ".Fanart"        , item->GetCachedFanart());
    }
  }
  
  videodatabase.Close();
  return true;
}

bool CRecentlyAddedVideosJob::DoWork()
{
  return Update(); 
}
