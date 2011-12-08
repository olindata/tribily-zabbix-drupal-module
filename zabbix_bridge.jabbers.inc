<?php

// $Id$

/*
 * @file
 * For handling multiple jabbers
 *
 * @author rajkissu
 */

/**
 * Form builder; Present the form to edit a given user's jabbers.
 *
 * @ingroup forms
 */
function zabbix_bridge_jabbers() {

  global $user;

  drupal_set_title('Jabber ids');

  $output = "<div id=\"jabber-explain\"><p>Here you can add your jabber/xmpp id. We will notify you on your jabber account for alerts of the selected severities.</p>";
  $output .= "<p>Among many other services, <strong>Google Talk (GTalk)</strong> also supports the jabber protocol, so add your GTalk address here and give it a try!</p>";
  $output .= "<p><strong>Important Note:</strong> After adding a jabber id here, make sure to do a friend request from your jabber client to <strong>tribily@jabberes.org</strong>. Only after that is done will you start receiving alerts from Tribily!</p></div>\n";
  $output .= zabbix_jabbers_table($user->uid);
  $output .= l('Add Jabber id', 'jabbers/add');

  return $output;
}

/**
 * Form builder; add/edit a user's jabbers.
 *
 * @ingroup forms
 * @see zabbix_bridge_jabbers_form_validate()
 * @see zabbix_bridge_jabbers_form_submit()
 */
function zabbix_bridge_jabbers_form(&$form_state, $operation, $jabberid = NULL) {
  global $user;

  // Get details of jabber if we're updating
  if (($operation == 'update') && isset($jabberid)) {

    $sql = "select userid, jabberid, jabber, zabbixmediaid, enabled from {zabbix_jabbers} where jabberid = '%s'";
    $result = db_query($sql, $jabberid);
    $jabber = db_fetch_object($result);

    if($jabber->userid != $user->uid)
    {
        drupal_set_message("Access Denied - You have not the permissions to update this object", DRUPAL_MSG_TYPE_ERR);
        return;
    }
  }

  // Set display fo form to reflect operation
  if ('add' == $operation) {

    $title = 'Add Jabber';
    $submit = 'Add';
  }
  else {

    $title = 'Update Jabber';
    $submit = 'Update';
  }

  // Put it in the form
  $form['jabbers'] = array(
      '#type' => 'fieldset',
      '#title' => t(check_plain($title)),
      '#weight' => -10
  );

  // The jabber's id
  $form['jabbers']['jabberid'] = array(
      '#type' => 'hidden',
      '#value' => $jabberid,
  );

  // the jabber
  $form['jabbers']['jabber'] = array(
      '#type' => 'textfield',
      '#title' => t('Jabber id'),
      '#maxlength' => MEDIA_NAME_MAXLENGTH,
      '#value' => isset($jabber->jabber) ? $jabber->jabber : '',
      '#required' => TRUE,
      '#description' => t("Your jabber id. This is usually your email address, e.g. 'johndoe@example.com'. For GTalk, try your gmail address.")
  );

  // status - enabled/disabled
  $form['jabbers']['status'] = array('#type' => 'select',
      '#title' => t('Status'),
      '#default_value' => isset($jabber->enabled)
              ? (($jabber->enabled == 1) ? 'enabled' : 'disabled')
              : 'enabled',
      '#options' => array(
          'enabled' => 'enabled',
          'disabled' => 'disabled',
      ),
      '#description' => t('Specify status'),
      '#required' => TRUE
  );

  // make the severity SELECT options
  $result = db_query("SELECT severityid, name FROM {zabbix_severities}")
          or drupal_set_message('Unable to retrieve zabbix severities', DRUPAL_MSG_TYPE_ERR);

  $template_options = array();

  while ($data = db_fetch_array($result)) {

    $template_options[$data['severityid']] = $data['name'];
  }

  $default_value = array();

  // get the assigned severities for this jabber
  if (isset($jabber->enabled)) {

    $sql = "select severityid from {zabbix_jabbers_severities} where jabberid = '%s'";
    $result = db_query($sql, $jabberid);

    while ($data = db_fetch_array($result)) {

      $default_value[] = $data['severityid'];
    }
  }


  $form['jabbers']['severities'] = array(
      '#type' => 'checkboxes',
      '#title' => t('Severities'),
      '#default_value' => $default_value,
      '#options' => $template_options,
      '#description' => t('Select one or more severities this jabber id will receive alerts for.')
  );

  $form['jabbers']['submit'] = array('#type' => 'submit', '#value' => t($submit), '#weight' => 30);

  return $form;
}

/**
 * Submit function for the user jabber add/editing form. Note that it handles adding _AND_ updating
 *
 * @global object $user
 * @param array $form
 * @param array $form_state
 * @return <type>
 */
function zabbix_bridge_jabbers_form_submit($form, &$form_state) {
  // Global drupal user object
  global $user;

  // if this was an existing jabber, retrieve it
  if (isset($form_state['clicked_button']['#post']['jabberid'])) {
    $jabberid = $form_state['clicked_button']['#post']['jabberid'];
  }

  if ($jabberid <> '') {
    // Get the zabbix userid belonging to the jabberid we got from the submitted form
    $result = db_query("SELECT zda.zabbix_uid FROM {zabbix_drupal_account} zda inner join {zabbix_jabbers} zj on zj.userid = zda.drupal_uid where zj.jabberid = '%s'", $jabberid)
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
  $status = $form_state['clicked_button']['#post']['status'];
  $severities = $form_state['clicked_button']['#post']['severities'];
  $severity = zabbix_bridge_calculate_severity($severities);
  $sendto = $form_state['clicked_button']['#post']['jabber'];

  // Fill an array to send to zabbix
  $jabber = array();
  $jabber['users'] = array('userid' => $zabbix_uid);
  $jabber['medias'] = array(
      'mediatypeid' => 2,
      'active' => ('enabled' == $status) ? 0 : 1,
      'severity' => $severity,
      'period' => '1-7,00:00-23:59;',
      'sendto' => $sendto
  );

  // This logs into Zabbix, and returns FALSE if it fails
  zabbix_api_login()
          or drupal_set_message('Unable to login: ' . print_r(ZabbixAPI::getLastError(), TRUE), DRUPAL_MSG_TYPE_ERR);

  //create the jabber if it's not already existing
  if ($jabberid == '') {

    zabbix_bridge_debug(print_r($jabber, TRUE));
    $addedmedia = ZabbixAPI::fetch_string('user', 'addMedia', $jabber);

    if ($addedmedia) {
      drupal_set_message('Jabber successfully added.');
    }
    else {
      drupal_set_message('Failed to add jabber: ' . print_r(ZabbixAPI::getLastError(), TRUE), DRUPAL_MSG_TYPE_ERR);
    }
  }
  else {

    // get all user media for given user
    $media = array(
        'userids' => array($zabbix_uid),
        'extendoutput' => TRUE
    );
    $usermedia = ZabbixAPI::fetch_array('user', 'getMedia', $media)
            or drupal_set_message('Problem retrieving existing media properties: ' . print_r(ZabbixAPI::getLastError(), TRUE), DRUPAL_MSG_TYPE_ERR);

    $sql = "select zabbixmediaid from {zabbix_jabbers} where jabberid = '%s'";
    $result = db_query($sql, $jabberid);

    $data = db_fetch_array($result);
    $zabbixmediaid = $data['zabbixmediaid'];

    foreach ($usermedia as $key => $item) {

      // found the mediaid we're looking for, make the changes
      if ($item['mediaid'] == $zabbixmediaid) {

        $usermedia[$key]['sendto'] = $sendto;
        $usermedia[$key]['active'] = ('enabled' == $status) ? 0 : 1;
        $usermedia[$key]['severity'] = $severity;
        $usermedia[$key]['period'] = '1-7,00:00-23:59;';

        break;
      }
    }

    $usermedia = array(
        'users' => array('userid' => $zabbix_uid),
        'medias' => $usermedia
    );

    zabbix_bridge_debug(print_r($usermedia, TRUE));

    //update the jabber and enabled status
    $usermedia = ZabbixAPI::fetch_string('user', 'updateMedia', $usermedia)
            or drupal_set_message('Problem updating existing jabber properties', DRUPAL_MSG_TYPE_ERR);

    zabbix_bridge_debug('update: ' . print_r($usermedia, TRUE));
  }

  // get user media
  $usermedia = ZabbixAPI::fetch_array('user', 'getMedia', array(
              'userids' => array($zabbix_uid),
              'mediatypeid' => 2,
              'sendto' => $sendto,
              'extendoutput' => TRUE
          ));

  if (!$usermedia || !count($usermedia)) {

    return;
  }

  // add the jabber mapping to the drupal database so we can use it for display without contacting zabbix
  $sql = 'INSERT INTO {zabbix_jabbers} (userid, jabberid, zabbixmediaid, jabber, enabled)';
  $sql .= " VALUES('%s', '%s', '%s', '%s', '%s')";
  $sql .= " ON DUPLICATE KEY UPDATE jabber='%s', enabled='%s'";

  foreach ($usermedia as $media) {

    $zabbixmediaid = $media['mediaid'];
    $active = $media['active'];

    $result = db_query($sql, $user->uid, $jabberid, $zabbixmediaid, $sendto, $active, $sendto, $active);

    if (!$result) {

      drupal_set_message('Problem adding jabber-mapping to database', DRUPAL_MSG_TYPE_ERR);
    }
  }

  if ($jabberid == '') {

    $jabberid = db_last_insert_id('zabbix_jabbers', 'jabberid');
  }

  // if severities was added
  // Delete all previous jabber mappings, so we can add the new ones in a sec
  $sql = "DELETE FROM {zabbix_jabbers_severities} WHERE jabberid = '%s'";
  db_query($sql, $jabberid) or
          drupal_set_message('Error updating severities in drupal database.');

  // addd the jabber-severity mapping to the drupal db so we can use it for display without contacting zabbix
  $sql = 'INSERT INTO {zabbix_jabbers_severities} (jabberid, severityid)';
  $sql .= " VALUES('%s', '%s')";

  if (count($severities) > 0) {

    foreach ($severities as $severityid) {

      $result = db_query($sql, $jabberid, $severityid);

      if (!$result) {

        drupal_set_message('Problem adding jabber-severity-mapping to database', DRUPAL_MSG_TYPE_ERR);
      }
    }
  }

  drupal_goto("jabbers/overview");

  return;
}

/**
 *
 * @param <type> $form_state
 * @param <type> $jabber
 * @return <type>
 */
function zabbix_bridge_jabbers_delete_confirm(&$form_state, $jabber) {

  $form['_jabber'] = array(
      '#type' => 'value',
      '#value' => $jabber,
  );

  return confirm_form(
          $form,
          t('Are you sure you want to delete this jabber?'),
          isset($_GET['destination']) ? $_GET['destination'] : "jabbers/overview",
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
function zabbix_bridge_jabbers_delete_confirm_submit($form, &$form_state) {

  if ($form_state['values']['confirm']) {

    $jabber = $form_state['values']['_jabber'];

    if (zabbix_bridge_jabbers_delete($jabber)) {

      drupal_set_message(t('Jabber has been deleted successfully.'));
    }
    else {

      drupal_set_message(t('Jabber deletion unsuccessful'), DRUPAL_MSG_TYPE_ERR);
    }
  }

  drupal_goto("jabbers/overview");
}

/**
 * Deletes a jabber and it's assigned severity from the database, both on the drupal
 * side and on the zabbix side
 *
 * @param  array    $formvalues ...
 * @param  int      $jabberid     The drupal id of the jabber to be deleted
 * @return bool
 */
function zabbix_bridge_jabbers_delete($jabberid) {

  $sql = "select zabbixmediaid from {zabbix_jabbers} where jabberid = '%s'";
  $zabbixmediaid = db_result(db_query($sql, $jabberid));

  zabbix_api_login()
          or drupal_set_message('Unable to login. ' . print_r(ZabbixAPI::getLastError(), TRUE), DRUPAL_MSG_TYPE_ERR);

  ZabbixAPI::fetch_string('user', 'deleteMedia', array($zabbixmediaid))
          or drupal_set_message('Unable to delete media. ' . print_r(ZabbixAPI::getLastError(), TRUE), DRUPAL_MSG_TYPE_ERR);

  $sql = "delete from {zabbix_jabbers_severities} where jabberid = '%s'";
  $result = db_query($sql, $jabberid);

  $sql = "delete from {zabbix_jabbers} where jabberid = '%s'";
  $result = db_query($sql, $jabberid);

  return $result;
}

/**
 *
 * @param <type> $form_state
 * @param <type> $jabber
 * @return <type>
 */
function zabbix_bridge_jabbers_enable_disable_confirm(&$form_state, $jabber) {
  $form['_jabber'] = array(
      '#type' => 'hidden',
      '#value' => $jabber,
  );

  $sql = "select enabled from {zabbix_jabbers} where jabberid = '%s'";
  $enabled = db_result(db_query($sql, $jabber));

  $form['_current_state'] = array(
      '#type' => 'hidden',
      '#value' => $enabled,
  );

  return confirm_form(
          $form,
          t('Are you sure you want to change the enabled state of this jabber?'),
          isset($_GET['destination']) ? $_GET['destination'] : "jabbers/overview",
          $enabled ? t('This jabber will be enabled until you disable it.') :
                  t('This jabber will be disabled until you re-enable it.'),
          t('OK'),
          t('Cancel')
  );
}

/**
 *
 * @param array $form
 * @param array $form_state
 */
function zabbix_bridge_jabbers_enable_disable_confirm_submit($form, &$form_state) {

  if ($form_state['values']['confirm']) {

    $jabber = $form_state['values']['_jabber'];
    $zabbixmediaid = zabbix_bridge_drupal_to_zabbix_jabberid($jabber);
    $enabled = $form_state['values']['_current_state'];

    if (zabbix_bridge_jabbers_enable_disable($zabbixmediaid, $enabled)) {

      drupal_set_message(t('Jabber has been updated successfully.'));
    }
    else {

      drupal_set_message(t('Jabber update unsuccessful'), DRUPAL_MSG_TYPE_ERR);
    }
  }

  drupal_goto("jabbers/overview");
}

/**
 *
 * @param int $jabberid
 * @param int $enabled
 * @return <type>
 */
function zabbix_bridge_jabbers_enable_disable($zabbixmediaid, $enabled) {

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

    drupal_set_message('Unable to enable/disable jabber. ' . print_r(ZabbixAPI::getLastError(), TRUE), DRUPAL_MSG_TYPE_ERR);
  }
  else {

    $sql = "update {zabbix_jabbers} set enabled = '%s' where zabbixmediaid = %s";
    $result = db_query($sql, $enabled ? 0 : 1, $zabbixmediaid);
  }

  return $result;
}

/**
 *
 * @return <type>
 */
function zabbix_bridge_import_jabbers() {

  $jabbers = array();
  $jabbers = zabbix_bridge_get_all_jabbers_from_zabbix(TRUE);

  zabbix_bridge_debug(print_r($jabbers, TRUE));

  foreach ($jabbers as $jabber) {

    $sql = "SELECT jabberid FROM {zabbix_jabbers} WHERE zabbixmediaid='%s'";
    $results = db_query($sql, $jabber['mediaid']);

    $mediaid = db_fetch_object($results);

    if ($mediaid) {

      drupal_set_message('Skipped jabber ' . $jabber['mediaid'] . ' (' . $jabber['sendto'] . '), as it already exists.');
    }
    else {

      $sql = "SELECT drupal_uid FROM {zabbix_drupal_account} WHERE zabbix_uid='%s'";
      $results = db_query($sql, $jabber['userid']);

      zabbix_bridge_debug($sql);

      $user = db_fetch_object($results);

      if ($user) {

        $sql = "INSERT INTO {zabbix_jabbers} (userid, zabbixmediaid, jabber, enabled) VALUES ('%s', '%s', '%s', '%s')";
        $results = db_query($sql, $user->drupal_uid, $jabber['mediaid'], $jabber['sendto'], $jabber['active']);
      }
    }
  }

  drupal_goto("admin/settings/zabbix_bridge");
}
