<?php

// $Id$

/*
 * @file
 * For handling multiple webpages
 *
 * @author rajkissu
 */

/**
 * Form builder; Present the form to edit a given user's webpages.
 *
 * @ingroup forms
 */
function zabbix_bridge_webpages() {

  global $user;

  drupal_set_title('Webpage ids');

  $output = "<div id=\"webpage-explain\"><p>Here you can add your webpages you want to monitor. We will notify you when your webpage is unreachable or slow to respond.</p>";
  $output .= zabbix_webpages_table($user->uid);
  $output .= l('Add Webpage id', 'webpages/add');

  return $output;
}

/**
 * Form builder; add/edit a user's webpages.
 *
 * @ingroup forms
 * @see zabbix_bridge_webpages_form_validate()
 * @see zabbix_bridge_webpages_form_submit()
 */
function zabbix_bridge_webpages_form(&$form_state, $operation, $webpageid = NULL) {
  global $user;

  // Get details of webpage if we're updating
  if (($operation == 'update') && isset($webpageid)) {

    $sql = "select userid, webpageid, webpage, enabled, description from {zabbix_webpages} where webpageid = '%s'";
    $result = db_query($sql, $webpageid);
    $webpage = db_fetch_object($result);

    if($webpage->userid != $user->uid)
    {
        drupal_set_message("Access Denied - You have not the permissions to update this object", DRUPAL_MSG_TYPE_ERR);
        return;
    }
  }

  // Set display fo form to reflect operation
  if ('add' == $operation) {

    $title = 'Add Webpage';
    $submit = 'Add';
  }
  else {

    $title = 'Update Webpage';
    $submit = 'Update';
  }

  // Put it in the form
  $form['webpages'] = array(
      '#type' => 'fieldset',
      '#title' => t($title),
      '#weight' => -10
  );

  // The webpage's id
  $form['webpages']['webpageid'] = array(
      '#type' => 'hidden',
      '#value' => $webpageid,
  );

  // the webpage
  $form['webpages']['webpage'] = array(
      '#type' => 'textfield',
      '#title' => t('Webpage'),
      '#maxlength' => MEDIA_NAME_MAXLENGTH,
      '#value' => isset($webpage->webpage) ? $webpage->webpage : '',
      '#required' => TRUE,
      '#description' => t("The webpage you wish to monitor. Currently https is not supported. Please include 'http://' in the url.")
  );

  // status - enabled/disabled
  $form['webpages']['status'] = array('#type' => 'select',
      '#title' => t('Status'),
      '#default_value' => isset($webpage->enabled)
              ? (($webpage->enabled == 1) ? 'enabled' : 'disabled')
              : 'enabled',
      '#options' => array(
          'enabled' => 'enabled',
          'disabled' => 'disabled',
      ),
      '#description' => t('Specify status'),
      '#required' => TRUE
  );

  // the description of the webpage
  $form['webpages']['description'] = array(
      '#type' => 'textfield',
      '#title' => t('Webpage description'),
      '#maxlength' => MEDIA_NAME_MAXLENGTH,
      '#value' => isset($webpage->description) ? $webpage->description : '',
      '#required' => TRUE,
      '#description' => t("A short description of the page you are monitoring.")
  );

  $form['webpages']['submit'] = array('#type' => 'submit', '#value' => t($submit), '#weight' => 30);

  return $form;
}

/**
 * Submit function for the user webpage add/editing form. Note that it handles adding _AND_ updating
 *
 * @global object $user
 * @param array $form
 * @param array $form_state
 * @return <type>
 */
function zabbix_bridge_webpages_form_submit($form, &$form_state) {
  // Global drupal user object
  global $user;

  // if this was an existing webpage, retrieve it
  if (isset($form_state['clicked_button']['#post']['webpageid'])) {
    $webpageid = $form_state['clicked_button']['#post']['webpageid'];
  }

  if ($webpageid <> '') {
    // Get the zabbix userid belonging to the webpageid we got from the submitted form
    $result = db_query("SELECT zda.zabbix_uid FROM {zabbix_drupal_account} zda inner join {zabbix_webpages} zj on zj.userid = zda.drupal_uid where zj.webpageid = '%s'", $webpageid)
            or drupal_set_message('Unable to retrieve drupal-zabbix associations', DRUPAL_MSG_TYPE_ERR);
    $data = db_fetch_array($result);
    $zabbix_uid = $data['zabbix_uid'];
  }
  else {
    // Get the zabbix userid belonging to the logged in drupal user
    $result = db_query("SELECT zda.zabbix_uid FROM {zabbix_drupal_account} zda where zda.drupal_uid = '%s'", $user->uid)
            or drupal_set_message('Unable to retrieve drupal-zabbix associations', DRUPAL_MSG_TYPE_ERR);
    $data = db_fetch_array($result);
    $zabbix_uid = $data['zabbix_uid'];
  }

  // Get some of the posted values from the form
  // 0 - enabled, 1 - disabled
  $status      = $form_state['clicked_button']['#post']['status'];
  $webpage_url = $form_state['clicked_button']['#post']['webpage'];
  $description = $form_state['clicked_button']['#post']['description'];

  // Fill an array to send to zabbix
  $webpage = array();
  $webpage['users'] = array('userid' => $zabbix_uid);
//  $webpage['medias'] = array(
//      'mediatypeid' => 2,
//      'active' => ('enabled' == $status) ? 0 : 1,
//      'severity' => $severity,
//      'period' => '1-7,00:00-23:59;',
//      'sendto' => $sendto
//  );

  // This logs into Zabbix, and returns FALSE if it fails
  zabbix_api_login()
          or drupal_set_message('Unable to login: ' . print_r(ZabbixAPI::getLastError(), TRUE), DRUPAL_MSG_TYPE_ERR);

  //create the webpage if it's not already existing
  if ($webpageid == '') {

    zabbix_bridge_debug(print_r($webpage, TRUE));
//    $addedmedia = ZabbixAPI::fetch_string('user', 'addMedia', $webpage);

    if ($addedmedia) {
      drupal_set_message('Webpage successfully added.');
    }
    else {
      drupal_set_message('Failed to add webpage: ' . print_r(ZabbixAPI::getLastError(), TRUE), DRUPAL_MSG_TYPE_ERR);
    }
  }
  else {

//    // get all user media for given user
//    $media = array(
//        'userids' => array($zabbix_uid),
//        'extendoutput' => TRUE
//    );
//    $usermedia = ZabbixAPI::fetch_array('user', 'getMedia', $media)
//            or drupal_set_message('Problem retrieving existing media properties: ' . print_r(ZabbixAPI::getLastError(), TRUE), DRUPAL_MSG_TYPE_ERR);
//
//    $sql = "select zabbixmediaid from {zabbix_webpages} where webpageid = '%s'";
//    $result = db_query($sql, $webpageid);
//
//    $data = db_fetch_array($result);
//    $zabbixmediaid = $data['zabbixmediaid'];
//
//    foreach ($usermedia as $key => $item) {
//
//      // found the mediaid we're looking for, make the changes
//      if ($item['mediaid'] == $zabbixmediaid) {
//
//        $usermedia[$key]['sendto'] = $sendto;
//        $usermedia[$key]['active'] = ('enabled' == $status) ? 0 : 1;
//        $usermedia[$key]['severity'] = $severity;
//        $usermedia[$key]['period'] = '1-7,00:00-23:59;';
//
//        break;
//      }
//    }
//
//    $usermedia = array(
//        'users' => array('userid' => $zabbix_uid),
//        'medias' => $usermedia
//    );
//
//    zabbix_bridge_debug(print_r($usermedia, TRUE));
//
//    //update the webpage and enabled status
//    $usermedia = ZabbixAPI::fetch_string('user', 'updateMedia', $usermedia)
//            or drupal_set_message('Problem updating existing webpage properties', DRUPAL_MSG_TYPE_ERR);
//
//    zabbix_bridge_debug('update: ' . print_r($usermedia, TRUE));
  }

  // add the webpage mapping to the drupal database so we can use it for display without contacting zabbix
  $sql = 'INSERT INTO {zabbix_webpages} (userid, webpageid, description, webpage, enabled)';
  $sql .= " VALUES('%s', '%s', '%s', '%s', '%s')";
  $sql .= " ON DUPLICATE KEY UPDATE webpage='%s', enabled='%s'";

  foreach ($usermedia as $media) {

    $active = $media['active'];

    $result = db_query($sql, $user->uid, $webpageid, $description, $webpage_url, $active, $webpage_url, $active);

    if (!$result) {

      drupal_set_message('Problem adding webpage-mapping to database', DRUPAL_MSG_TYPE_ERR);
    }
  }

  if ($webpageid == '') {

    $webpageid = db_last_insert_id('zabbix_webpages', 'webpageid');
  }


  drupal_goto("webpages/overview");

  return;
}

/**
 *
 * @param <type> $form_state
 * @param <type> $webpage
 * @return <type>
 */
function zabbix_bridge_webpages_delete_confirm(&$form_state, $webpage) {

  $form['_webpage'] = array(
      '#type' => 'value',
      '#value' => $webpage,
  );

  return confirm_form(
          $form,
          t('Are you sure you want to delete this webpage?'),
          isset($_GET['destination']) ? $_GET['destination'] : "webpages/overview",
          t('This action cannot be undone.'),
          t('Delete'),
          t('Cancel')
  );
}

/**
 *
 * @param array $form
 * @param array $form_state
 */
function zabbix_bridge_webpages_delete_confirm_submit($form, &$form_state) {

  if ($form_state['values']['confirm']) {

    $webpage = $form_state['values']['_webpage'];

    if (zabbix_bridge_webpages_delete($webpage)) {

      drupal_set_message(t('Webpage has been deleted successfully.'));
    }
    else {

      drupal_set_message(t('Webpage deletion unsuccessful'), DRUPAL_MSG_TYPE_ERR);
    }
  }

  drupal_goto("webpages/overview");
}

/**
 * Deletes a webpage and it's assigned severity from the database, both on the drupal
 * side and on the zabbix side
 *
 * @param  array    $formvalues ...
 * @param  int      $webpageid     The drupal id of the webpage to be deleted
 * @return bool
 */
function zabbix_bridge_webpages_delete($webpageid) {

  $sql = "select zabbixmediaid from {zabbix_webpages} where webpageid = '%s'";
  $zabbixmediaid = db_result(db_query($sql, $webpageid));

  zabbix_api_login()
          or drupal_set_message('Unable to login. ' . print_r(ZabbixAPI::getLastError(), TRUE), DRUPAL_MSG_TYPE_ERR);

  ZabbixAPI::fetch_string('user', 'deleteMedia', array($zabbixmediaid))
          or drupal_set_message('Unable to delete media. ' . print_r(ZabbixAPI::getLastError(), TRUE), DRUPAL_MSG_TYPE_ERR);

  $sql = "delete from {zabbix_webpages_severities} where webpageid = '%s'";
  $result = db_query($sql, $webpageid);

  $sql = "delete from {zabbix_webpages} where webpageid = '%s'";
  $result = db_query($sql, $webpageid);

  return $result;
}

/**
 *
 * @param <type> $form_state
 * @param <type> $webpage
 * @return <type>
 */
function zabbix_bridge_webpages_enable_disable_confirm(&$form_state, $webpage) {
  $form['_webpage'] = array(
      '#type' => 'hidden',
      '#value' => $webpage,
  );

  $sql = "select enabled from {zabbix_webpages} where webpageid = '%s'";
  $enabled = db_result(db_query($sql, $webpage));

  $form['_current_state'] = array(
      '#type' => 'hidden',
      '#value' => $enabled,
  );

  return confirm_form(
          $form,
          t('Are you sure you want to change the enabled state of this webpage?'),
          isset($_GET['destination']) ? $_GET['destination'] : "webpages/overview",
          $enabled ? t('This webpage will be enabled until you disable it.') :
                  t('This webpage will be disabled until you re-enable it.'),
          t('OK'),
          t('Cancel')
  );
}

/**
 *
 * @param array $form
 * @param array $form_state
 */
function zabbix_bridge_webpages_enable_disable_confirm_submit($form, &$form_state) {

  if ($form_state['values']['confirm']) {

    $webpage = $form_state['values']['_webpage'];
    $zabbixmediaid = zabbix_bridge_drupal_to_zabbix_webpageid($webpage);
    $enabled = $form_state['values']['_current_state'];

    if (zabbix_bridge_webpages_enable_disable($zabbixmediaid, $enabled)) {

      drupal_set_message(t('Webpage has been updated successfully.'));
    }
    else {

      drupal_set_message(t('Webpage update unsuccessful'), DRUPAL_MSG_TYPE_ERR);
    }
  }

  drupal_goto("webpages/overview");
}

/**
 *
 * @param int $webpageid
 * @param int $enabled
 * @return <type>
 */
function zabbix_bridge_webpages_enable_disable($zabbixmediaid, $enabled) {

  global $user;

  $result = db_query("SELECT zabbix_uid FROM {zabbix_drupal_account} WHERE drupal_uid = '%s'", $user->uid)
          or drupal_set_message('Unable to retrieve drupal-zabbix associations', DRUPAL_MSG_TYPE_ERR);

  $data = db_fetch_array($result);

  if (!$data) {

    drupal_set_message('No drupal-zabbix association exists for this user!', DRUPAL_MSG_TYPE_ERR);

    return;
  }

  zabbix_bridge_debug(print_r($data, TRUE));

  zabbix_api_login()
          or drupal_set_message('Unable to login. ' . print_r(ZabbixAPI::getLastError(), TRUE), DRUPAL_MSG_TYPE_ERR);

  // get user media
  $media = ZabbixAPI::fetch_array('user', 'getMedia', array(
              'userids' => array($data['zabbix_uid']),
              'extendoutput' => TRUE
          ));

  $result = ZabbixAPI::fetch_string('user', 'getMedia', $media)
          or drupal_set_message('Unable to retrieve user media. ' . print_r(ZabbixAPI::getLastError(), TRUE), DRUPAL_MSG_TYPE_ERR);

  foreach ($media as $key => $item) {

    if ($item['mediaid'] == $zabbixmediaid) {

      // this is the media we're looking for, change the status
      $media[$key]['active'] = $enabled ? 0 : 1;
      break;
    }
  }

  $usermedia['users'] = array('userid' => $data['zabbix_uid']);
  $usermedia['medias'] = $media;

  zabbix_bridge_debug(print_r($usermedia, TRUE));

  $result = ZabbixAPI::fetch_string('user', 'updateMedia', $usermedia);

  zabbix_bridge_debug(print_r($result, TRUE));

  if (!$result) {

    drupal_set_message('Unable to enable/disable webpage. ' . print_r(ZabbixAPI::getLastError(), TRUE), DRUPAL_MSG_TYPE_ERR);
  }
  else {

    $sql = "update {zabbix_webpages} set enabled = '%s' where zabbixmediaid = %s";
    $result = db_query($sql, $enabled ? 0 : 1, $zabbixmediaid);
  }

  return $result;
}

/**
 *
 * @return <type>
 */
function zabbix_bridge_import_webpages() {

  $webpages = array();
  $webpages = zabbix_bridge_get_all_webpages_from_zabbix(TRUE);

  zabbix_bridge_debug(print_r($webpages, TRUE));

  foreach ($webpages as $webpage) {

    $sql = "SELECT webpageid FROM {zabbix_webpages} WHERE zabbixmediaid='%s'";
    $results = db_query($sql, $webpage['mediaid']);

    $mediaid = db_fetch_object($results);

    if ($mediaid) {

      drupal_set_message('Skipped webpage ' . $webpage['mediaid'] . ' (' . $webpage['sendto'] . '), as it already exists.');
    }
    else {

      $sql = "SELECT drupal_uid FROM {zabbix_drupal_account} WHERE zabbix_uid='%s'";
      $results = db_query($sql, $webpage['userid']);

      zabbix_bridge_debug($sql);

      $user = db_fetch_object($results);

      if ($user) {

        $sql = "INSERT INTO {zabbix_webpages} (userid, zabbixmediaid, webpage, enabled) VALUES ('%s', '%s', '%s', '%s')";
        $results = db_query($sql, $user->drupal_uid, $webpage['mediaid'], $webpage['sendto'], $webpage['active']);
      }
    }
  }

  drupal_goto("admin/settings/zabbix_bridge");
}
