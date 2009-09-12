<?php

/**
 * Watchdog RSS Admin Form to Reset the URL
 */
function watchdog_rss_admin_form() {

  $link = url('watchdog-feed/'. variable_get('watchdog_rss_key', ''), array('absolute' => TRUE));

  $form['div_tag'] = array(
    '#type' => 'markup',
    '#value' => '<br /><h3>Feed URL</h3>'. l($link, $link) .'<br />'.
    '<p>Reset your Feed URL if you want to change the URL of your feed</p>',
  );

  $form['submit'] = array(
    '#type' => 'submit',
    '#value' => t('Reset Feed URL'),
  );

  return $form;
}

/**
 * Reset the Feed URL
 */
function watchdog_rss_admin_form_submit($form, &$form_state) {

  variable_del('watchdog_rss_key');
  variable_set('watchdog_rss_key', sha1(watchdog_rss_random_gen(rand(5, 50))));

}