/*
 *      Copyright (C) 2005-2008 Team XBMC
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

#include "GUIWindowHome.h"
#include "guilib/Key.h"
#include "utils/JobManager.h"
#include "utils/RecentlyAddedVideosJob.h"
#include "utils/RecentlyAddedMusicJob.h"
#include "utils/RecentlyAddedTotalsJob.h"
#include "interfaces/AnnouncementManager.h"
#include "utils/log.h"

using namespace ANNOUNCEMENT;

CGUIWindowHome::CGUIWindowHome(void) : CGUIWindow(WINDOW_HOME, "Home.xml")
{
  m_updateRA = true;

  CAnnouncementManager::AddAnnouncer(this);
}

CGUIWindowHome::~CGUIWindowHome(void)
{
  CAnnouncementManager::RemoveAnnouncer(this);
}

void CGUIWindowHome::OnInitWindow()
{  
  if ( m_updateRA )
  {
    AddRecentlyAddedJobs();
  }

  CGUIWindow::OnInitWindow();
}

void CGUIWindowHome::Announce(EAnnouncementFlag flag, const char *sender, const char *message, const CVariant &data)
{
  CLog::Log(LOGDEBUG, "GOT ANNOUNCEMENT, type: %i, from %s, message %s",(int)flag, sender, message);

  if ( ( flag == Library ) &&
       ( ( strcmp(message, "UpdateVideo") == 0 ) ||
         ( strcmp(message, "UpdateAudio") == 0 ) ) )
  {
    // add the job immediatedly if the home window is active
    // otherwise defer it to the next initialisation
    if ( IsActive() )
      AddRecentlyAddedJobs();
    else
      m_updateRA = true;
  }
}

void CGUIWindowHome::AddRecentlyAddedJobs()
{
  CJobManager::GetInstance().AddJob(new CRecentlyAddedVideosJob(), NULL);
  CJobManager::GetInstance().AddJob(new CRecentlyAddedMusicJob(), NULL);
  CJobManager::GetInstance().AddJob(new CRecentlyAddedTotalsJob(), NULL);

  m_updateRA = false;
}
