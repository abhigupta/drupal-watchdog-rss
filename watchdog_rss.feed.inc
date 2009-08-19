<?php


/**
 * Publish a feed for all Watchdog Entries on the site.
 */
function watchdog_rss_feed_site($param = NULL) {

  if (httpauth_login()) {

    global $base_url, $locale;

    if (!isset($param)) {

      $last_time = '';
      $items = watchdog_rss_format_items($last_time);

      $channel = array(
        'version'     => '2.0',
        'title'       => t('@site_name - Watchdog Logs', array('@site_name' => variable_get('site_name', 'Drupal'))),
        'link'        => url('watchdog-feed/'. variable_get('watchdog_rss_key', ''), array('absolute' => TRUE)),
        'description' => t('Watchdog Logs'),
        'language'    => $locale,
        'lastBuildDate' => $last_time,
      );

      watchdog_rss_format_feed($items, $channel);
      return NULL;
    }

  }

  drupal_not_found();
}

/**
 * Output Watchdog RSS Feed
 */
function watchdog_rss_format_feed($items, $channel = array(), $namespaces = array()) {
  global $base_url, $locale;

  $channel_defaults = array(
    'version'     => '2.0',
    'title'       => t('@site_name - Comments', array('@site_name' => variable_get('site_name', 'Drupal'))),
    'link'        => $base_url,
    'description' => t('Comments'),
    'language'    => $locale,
    'lastBuildDate' => date('r', mktime()),
  );

  $channel = array_merge($channel_defaults, $channel);

  $build_date['lastBuildDate'] = $channel['lastBuildDate'];

  $namespaces = array_merge(array('xmlns:dc' => 'http://purl.org/dc/elements/1.1/'), $namespaces);

  $output = '<?xml version="1.0" encoding="utf-8"?>'."\n";
  $output .= '<rss version="'. $channel['version'] .'" xml:base="'. $base_url .'"'. drupal_attributes($namespaces) .">\n";
  $output .= format_rss_channel($channel['title'], $channel['link'],
  $channel['description'], $items, $channel['language'], $build_date);
  $output .= "</rss>\n";

  drupal_set_header('Content-Type: application/rss+xml; charset=utf-8');
  print $output;
}

/**
 * Create an array of Watchdog Entries as Items in RSS Feed
 */

function watchdog_rss_format_items(&$last_time) {
  global $base_url;

  $items = '';

  $SQL = "SELECT wid, type, message, link, location, referer, hostname, timestamp
            FROM {watchdog} ORDER BY wid DESC LIMIT 200";

  $events = db_query($SQL);

  while ($event = db_fetch_object($events)) {

    $title = "[$event->type] " . truncate_utf8($event->message, 56, TRUE, TRUE);
    $extra = array(
      array(
        'key' => 'pubDate',
        'value' => date('r', $event->timestamp)
      ),
      array(
        'key' => 'dc:creator',
        'value' => $event->type
      ),
      array(
        'key' => 'guid',
        'value' => 'log event '. $event->wid .' at '. url($base_url, array('absolute' => TRUE)),
        'attributes' => array('isPermaLink' => 'false')
      ),
      array(
        'key' => 'category',
        'value' => $event->type,
      )
    );

    $last_time = empty($last_time) ? date('r', $event->timestamp) : $last_time;
    $link = url('admin/reports/event/'. $event->wid, array('absolute' => TRUE));

    $content  = $event->message;
    $content .= !empty($event->link) ? ' ('. $event->link .')' : '';
    $content .= '<br /><br /><strong>Location</strong>: '. l($event->location, $event->location);
    $content .= ($event->referer !== $event->location) ?
      '<br /><strong>Referer</strong>: '. l($event->referer, $event->referer) : '';
    $content .= '<br /><strong>IP</strong>: '.
      l($event->hostname, 'http://www.melissadata.com/lookups/iplocation.asp', array('attributes' => 
      array(), 'query' => 'ipaddress='. $event->hostname));

    $items .= format_rss_item(strip_tags($title), $link, $content, $extra);
  }
  return $items;
}


/**
 * Provides a random string of the length requested.
 */
function watchdog_rss_random_gen($length) {
  $random= "";
  srand((double)microtime()*1000000);
  $char_list = "ABCDEFGHIJKLMNOPQRSTUVWXYZ";
  $char_list .= "abcdefghijklmnopqrstuvwxyz";
  $char_list .= "1234567890";

  for ($i = 0; $i < $length; $i++) {
    $random .= drupal_substr($char_list, (rand()%(drupal_strlen($char_list))), 1);
  }
  return $random;
}

/**
 * Use HTTP Authentication to Verify the User
 */
function httpauth_login() {
// Return if HTTP authentication is disabled.
//if (!variable_get('httpauth_status', FALSE)) {
//  return;
//}

// Load credentials.
  if (isset($_SERVER['PHP_AUTH_USER'], $_SERVER['PHP_AUTH_PW'])) {
    $name = $_SERVER['PHP_AUTH_USER'];
    $pass = $_SERVER['PHP_AUTH_PW'];
  }
  else if (isset($_GET['HTTP_AUTHORIZATION'])) {
    list($name, $pass) = httpauth_parse($_GET['HTTP_AUTHORIZATION']);
  }
  else {
    httpauth_unauthorized();
    exit();
  }


  if (isset($name, $pass)) {
    global $user;

    // Return True if the user is already logged in.
    if ($user->uid && strcasecmp($user->name, $name) == 0) {
      return user_access('access watchdog RSS feed');
    }

    //Reprompt for User Name and Password if user is blocked or denied access
    drupal_load('module', 'user');
    if (user_is_blocked($name) || drupal_is_denied('user', $name)) {
      httpauth_unauthorized();
      exit();
    }
    $user = user_authenticate(array('name' => $name, 'pass' => $pass));

    // Was authentication successful?
    if ($user->uid) {
      //user_login_submit('httpauth', array('name' => $name, 'pass' => $pass));
      return user_access('access watchdog RSS feed');
    }
    else {
    // We need common.inc for t(), and theme.inc for theme() (called indirectly by t()).
      module_load_include('inc', 'watchdog_rss', 'common');
      module_load_include('inc', 'watchdog_rss', 'theme');

      watchdog('user', 'Login attempt using HTTP authentication failed for %user.', array('%user' => $name));

      httpauth_unauthorized();
      exit();
    }
  }

  return FALSE;
}

/**
 * Set 401 Unauthorized status and WWW-Authenticate header.
 */
function httpauth_unauthorized() {
  module_load_include('inc', 'watchdog_rss', 'common');
  module_load_include('inc', 'watchdog_rss', 'unicode');

  $site_name = trim(variable_get('site_name', 'drupal'));
  $realm = mime_header_encode($site_name);

  drupal_set_header("HTTP/1.0 401 Unauthorized");
  drupal_set_header("WWW-Authenticate: Basic realm=\"$realm\"");
}

/**
 * Parse an HTTP authorization header.
 */
function httpauth_parse($header) {
  list($type, $credentials) = split(' ', $header);
  if ($type == 'Basic') {
    return explode(':', base64_decode($credentials));
  }
}