$Id

Drupal watchdog_rss.module README.txt
==============================================================================

This module provides RSS feeds for Watchdog. This is useful for allowing admin
to subscribe to the RSS feed in order to keep an eye on any error or messags
from the website.

It currently provides a single feed with all the Watchdog Entries.

The Feed is available publically, but it is protected by creating a 
unique URL to your websites feed. Don't share this URL with others as it
may compromise the security of your system. You can also reset the feed URL 
from settings page.


Installation
------------------------------------------------------------------------------
 
 Required:
  - Copy the module files to modules/
  - Enable the module as usual from Drupal's admin pages.
  - Go to /admin/settings/watchdog_rss to access the Feed URL or Reset URL