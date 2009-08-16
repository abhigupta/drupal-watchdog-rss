$Id

Drupal watchdog_rss.module README.txt
==============================================================================

This module provides RSS feeds for Watchdog. This is useful for allowing admin
to subscribe to the RSS feed in order to keep an eye on any error or messags
from the website.

It currently provides a single feed with all the Watchdog Entries.

The Feed is protected using 2 methonds:

1) Creating a unique URL to your websites feed. You can also reset 
   the feed URL from settings page.

2) HTTP Authentication. Accessing the feed requires HTTP Authentication.
   The authentication is linked to the user accounts in Drupal. There is 
   a new permission "Access Watchdog RSS Feed" that can be assigned to
   a new user.

Installation
------------------------------------------------------------------------------
 
 Required:
  - Copy the module files to modules/
  - Enable the module as usual from Drupal's admin pages.
  - Go to /admin/settings/watchdog_rss to access the Feed URL or Reset URL
