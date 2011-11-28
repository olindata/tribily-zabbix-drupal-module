<?php

// $Id$

/*
 * @file
* For handling multiple mobile numbers
*
* @author rajkissu
*/

/**
 * Form builder; Present the form to edit a given user's mobile numbers.
 *
 * @ingroup forms
 */
function zabbix_bridge_mobiles() {

  global $user;

  drupal_set_title(t('Mobile Numbers'));

  $output = "<div id=\"mobile-explain\"><p>Here you can add your mobile number. We will send a text message to the numbers defined here.</p>";
  $output .= "<p><strong>Important Note:</strong> Mobile phone alerts happen only for the highest two alert levels. </p></div>\n";
  $output .= zabbix_mobiles_table($user->uid);
  $output .= l('Add Mobile Number', 'mobiles/add');

  return $output;
}

/**
 * Form builder; add/edit a user's mobile numbers.
 *
 * @ingroup forms
 * @see zabbix_bridge_mobiles_form_validate()
 * @see zabbix_bridge_mobiles_form_submit()
 */
function zabbix_bridge_mobiles_form(&$form_state, $operation, $mobileid = NULL) {
  global $user;

  // Get details of mobile if we're updating
  if (($operation == 'update') && isset($mobileid)) {

    $sql = "select userid, mobileid, number, zabbixmediaid, enabled from {zabbix_mobiles} where mobileid = '%s'";
    $result = db_query($sql, $mobileid);
    $mobile = db_fetch_object($result);

    // Access Denied
    if($mobile->userid != $user->uid)
    {
      drupal_set_message("Access Denied - You have not the permissions to update this object", DRUPAL_MSG_TYPE_ERR);
      return;
    }
  }

  // Set display fo form to reflect operation
  if ('add' == $operation) {

    $title = 'Add Mobile Number';
    $submit = 'Add';
  }
  else {

    $title = 'Update Mobile Number';
    $submit = 'Update';
  }

  // Put it in the form
  $form['mobiles'] = array('#type' => 'fieldset',
      '#title' => t(check_plain($title)),
      '#weight' => -10
  );

  // The mobile number's id
  $form['mobiles']['mobileid'] = array(
      '#type' => 'hidden',
      '#value' => $mobileid,
  );

  // the mobile's number
  $form['mobiles']['number'] = array(
      '#type' => 'textfield',
      '#title' => t('Mobile phone number'),
      '#maxlength' => MEDIA_NAME_MAXLENGTH,
      '#value' => isset($mobile->number) ? $mobile->number : '',
      '#description' => t("Your mobile phone number in international notation. For example, the country code for Netherlands is +31, so a local phone number of 01-23456789 would be entered here as +31123456789 (omit the leading 0 and just concatenate international and national number)"),
      '#required' => TRUE
  );

  // status - enabled/disabled
  $form['mobiles']['status'] = array('#type' => 'select',
      '#title' => t('Status'),
      '#default_value' => isset($mobile->enabled)
  ? (($mobile->enabled == 1) ? 'enabled' : 'disabled')
  : 'enabled',
      '#options' => array(
          'enabled' => 'enabled',
          'disabled' => 'disabled',
  ),
      '#description' => t('Specify status'),
      '#required' => TRUE
  );

  // make the severity SELECT options
  $result = db_query("SELECT severityid, name FROM {zabbix_severities} WHERE name IN ('High', 'Disaster')")
  or drupal_set_message(t('Unable to retrieve zabbix severities'), DRUPAL_MSG_TYPE_ERR);

  $template_options = array();

  while ($data = db_fetch_array($result)) {

    $template_options[$data['severityid']] = $data['name'];
  }

  $default_value = array();

  // get the assigned severities for this mobile
  if (isset($mobile->enabled)) {

    $sql = "select severityid from {zabbix_mobiles_severities} where mobileid = '%s'";
    $result = db_query($sql, $mobileid);

    while ($data = db_fetch_array($result)) {

      $default_value[] = $data['severityid'];
    }
  }


  $form['mobiles']['severities'] = array(
      '#type' => 'checkboxes',
      '#title' => t('Severities'),
      '#default_value' => $default_value,
      '#options' => $template_options,
      '#description' => t('Assign one or more severities to this mobile.')
  );

  $form['mobiles']['submit'] = array('#type' => 'submit', '#value' => t($submit), '#weight' => 30);

  return $form;
}

/**
 * Submit function for the user mobile add/editing form. Note that it handles adding _AND_ updating
 *
 * @global object $user
 * @param array $form
 * @param array $form_state
 * @return <type>
 */
function zabbix_bridge_mobiles_form_submit($form, &$form_state) {
  // Global drupal user object
  global $user;

  // if this was an existing mobile, retrieve it
  if (isset($form_state['clicked_button']['#post']['mobileid'])) {
    $mobileid = $form_state['clicked_button']['#post']['mobileid'];
  }

  if ($mobileid <> '') {
    // Get the zabbix userid belonging to the mobileid we got from the submitted form
    $result = db_query("SELECT zda.zabbix_uid, zda.drupal_uid FROM {zabbix_drupal_account} zda inner join {zabbix_mobiles} zm on zm.userid = zda.drupal_uid where zm.mobileid = :mobileid", array(
		':mobileid' => $mobileid
    ))
    or drupal_set_message(t('Unable to retrieve drupal-zabbix associations'), DRUPAL_MSG_TYPE_ERR);
    $data = db_fetch_array($result);
    $zabbix_uid = $data['zabbix_uid'];

    if($data['drupal_uid'] != $user->uid)
    {
      drupal_set_message("Access Denied - You have not the permissions to update this object", DRUPAL_MSG_TYPE_ERR);
      return;
    }
  }
  else {
    // Get the zabbix userid belonging to the logged in drupal user
    $result = db_query("SELECT zda.zabbix_uid FROM {zabbix_drupal_account} zda where zda.drupal_uid = :drupal_uid", array(
		':drupal_uid' => $user->uid
    ))
    or drupal_set_message(t('Unable to retrieve drupal-zabbix associations'), DRUPAL_MSG_TYPE_ERR);
    $data = db_fetch_array($result);
    $zabbix_uid = $data['zabbix_uid'];
  }

  // Get some of the posted values from the form
  // 0 - enabled, 1 - disabled
  $status = $form_state['clicked_button']['#post']['status'];
  $severities = $form_state['clicked_button']['#post']['severities'];
  $severity = zabbix_bridge_calculate_severity($severities);
  $sendto = $form_state['clicked_button']['#post']['number'];

  // Fill an array to send to zabbix
  $mobile = array();
  $mobile['users'] = array('userid' => $zabbix_uid);
  $mobile['medias'] = array(
      'mediatypeid' => 3,
      'active' => ('enabled' == $status) ? 0 : 1,
      'severity' => $severity,
      'period' => '1-7,00:00-23:59;',
      'sendto' => $sendto
  );

  // This logs into Zabbix, and returns FALSE if it fails
  zabbix_api_login()
  or drupal_set_message(t('Unable to login: ') . print_r(ZabbixAPI::getLastError(), TRUE), DRUPAL_MSG_TYPE_ERR);

  //create the mobile if it's not already existing
  if ($mobileid == '') {

    zabbix_bridge_debug(print_r($mobile, TRUE));
    $addedmedia = ZabbixAPI::fetch_string('user', 'addMedia', $mobile);

    if ($addedmedia) {
      drupal_set_message(t('Mobile number successfully added.'));
    }
    else {
      drupal_set_message(t('Failed to add mobile number: ') . print_r(ZabbixAPI::getLastError(), TRUE), DRUPAL_MSG_TYPE_ERR);
    }
  }
  else {

    // get all user media for given user
    $media = array(
        'userids' => array($zabbix_uid),
        'extendoutput' => TRUE
    );
    $usermedia = ZabbixAPI::fetch_array('user', 'getMedia', $media)
    or drupal_set_message(t('Problem retrieving existing media properties: ') . print_r(ZabbixAPI::getLastError(), TRUE), DRUPAL_MSG_TYPE_ERR);

    $sql = "select zabbixmediaid from {zabbix_mobiles} where mobileid = '%s'";
    $result = db_query($sql, $mobileid);

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

    //update the mobile and enabled status
    $usermedia = ZabbixAPI::fetch_string('user', 'updateMedia', $usermedia)
    or drupal_set_message(t('Problem updating existing mobile properties'), DRUPAL_MSG_TYPE_ERR);

    zabbix_bridge_debug('update: ' . print_r($usermedia, TRUE));
  }

  // get user media
  $usermedia = ZabbixAPI::fetch_array('user', 'getMedia', array(
              'userids' => array($zabbix_uid),
              'mediatypeid' => 3,
              'sendto' => $sendto,
              'extendoutput' => TRUE
  ));

  if (!$usermedia || !count($usermedia)) {

    return;
  }

  // add the mobile mapping to the drupal database so we can use it for display without contacting zabbix
  $sql = 'INSERT INTO {zabbix_mobiles} (userid, mobileid, zabbixmediaid, number, enabled)';
  $sql .= " VALUES('%s', '%s', '%s', '%s', '%s')";
  $sql .= " ON DUPLICATE KEY UPDATE number='%s', enabled='%s'";

  foreach ($usermedia as $media) {

    $zabbixmediaid = $media['mediaid'];
    $active = $media['active'];

    $result = db_query($sql, $user->uid, $mobileid, $zabbixmediaid, $sendto, $active, $sendto, $active);

    if (!$result) {

      drupal_set_message(t('Problem adding mobile-mapping to database'), DRUPAL_MSG_TYPE_ERR);
    }
  }

  if ($mobileid == '') {

    $mobileid = db_last_insert_id('zabbix_mobiles', 'mobileid');
  }

  // if severities was added
  // Delete all previous mobile mappings, so we can add the new ones in a sec
  $sql = "DELETE FROM {zabbix_mobiles_severities} WHERE mobileid = '%s'";
  db_query($sql, $mobileid) or
  drupal_set_message(t('Error updating severities in drupal database.'));

  // addd the mobile-severity mapping to the drupal db so we can use it for display without contacting zabbix
  $sql = 'INSERT INTO {zabbix_mobiles_severities} (mobileid, severityid)';
  $sql .= " VALUES('%s', '%s')";

  if (count($severities) > 0) {

    foreach ($severities as $severityid) {

      $result = db_query($sql, $mobileid, $severityid);

      if (!$result) {

        drupal_set_message(t('Problem adding mobile-severity-mapping to database'), DRUPAL_MSG_TYPE_ERR);
      }
    }
  }

  drupal_goto("mobiles/overview");

  return;
}

/**
 *
 * @param <type> $form_state
 * @param <type> $mobile
 * @return <type>
 */
function zabbix_bridge_mobiles_delete_confirm(&$form_state, $mobile) {

  $form['_mobile'] = array(
      '#type' => 'value',
      '#value' => $mobile,
  );

  return confirm_form(
  $form,
  t('Are you sure you want to delete this mobile number?'),
  isset($_GET['destination']) ? $_GET['destination'] : "mobiles/overview",
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
function zabbix_bridge_mobiles_delete_confirm_submit($form, &$form_state) {

  if ($form_state['values']['confirm']) {

    $mobile = $form_state['values']['_mobile'];

    if (zabbix_bridge_mobiles_delete($mobile)) {

      drupal_set_message(t('Mobile number has been deleted successfully.'));
    }
    else {

      drupal_set_message(t('Mobile number deletion unsuccessful'), DRUPAL_MSG_TYPE_ERR);
    }
  }

  drupal_goto("mobiles/overview");
}

/**
 * Deletes a mobile number and it's assigned severity from the database, both on the drupal
 * side and on the zabbix side
 *
 * @param  array    $formvalues ...
 * @param  int      $mobileid     The drupal id of the mobile number to be deleted
 * @return bool
 */
function zabbix_bridge_mobiles_delete($mobileid) {

  $sql = "select zabbixmediaid from {zabbix_mobiles} where mobileid = '%s'";
  $zabbixmediaid = db_result(db_query($sql, $mobileid));

  zabbix_api_login()
  or drupal_set_message(t('Unable to login. ') . print_r(ZabbixAPI::getLastError(), TRUE), DRUPAL_MSG_TYPE_ERR);

  ZabbixAPI::fetch_string('user', 'deleteMedia', array($zabbixmediaid))
  or drupal_set_message(t('Unable to delete media. ') . print_r(ZabbixAPI::getLastError(), TRUE), DRUPAL_MSG_TYPE_ERR);

  $sql = "delete from {zabbix_mobiles_severities} where mobileid = '%s'";
  $result = db_query($sql, $mobileid);

  $sql = "delete from {zabbix_mobiles} where mobileid = '%s'";
  $result = db_query($sql, $mobileid);

  return $result;
}

/**
 *
 * @param <type> $form_state
 * @param <type> $mobile
 * @return <type>
 */
function zabbix_bridge_mobiles_enable_disable_confirm(&$form_state, $mobile) {
  $form['_mobile'] = array(
      '#type' => 'hidden',
      '#value' => $mobile,
  );

  $sql = "select enabled from {zabbix_mobiles} where mobileid = '%s'";
  $enabled = db_result(db_query($sql, $mobile));

  $form['_current_state'] = array(
      '#type' => 'hidden',
      '#value' => $enabled,
  );

  return confirm_form(
  $form,
  t('Are you sure you want to change the enabled state of this mobile number?'),
  isset($_GET['destination']) ? $_GET['destination'] : "mobiles/overview",
  $enabled ? t('This mobile number will be enabled until you disable it.') :
  t('This mobile number will be disabled until you re-enable it.'),
  t('OK'),
  t('Cancel')
  );
}

/**
 *
 * @param array $form
 * @param array $form_state
 */
function zabbix_bridge_mobiles_enable_disable_confirm_submit($form, &$form_state) {

  if ($form_state['values']['confirm']) {

    $mobile = $form_state['values']['_mobile'];
    $zabbixmediaid = zabbix_bridge_drupal_to_zabbix_mobileid($mobile);
    $enabled = $form_state['values']['_current_state'];

    if (zabbix_bridge_mobiles_enable_disable($zabbixmediaid, $enabled)) {

      drupal_set_message(t('Mobile number has been updated successfully.'));
    }
    else {

      drupal_set_message(t('Mobile number update unsuccessful'), DRUPAL_MSG_TYPE_ERR);
    }
  }

  drupal_goto("mobiles/overview");
}

/**
 *
 * @param int $mobileid
 * @param int $enabled
 * @return <type>
 */
function zabbix_bridge_mobiles_enable_disable($zabbixmediaid, $enabled) {

  global $user;

  $result = db_query("SELECT zabbix_uid FROM {zabbix_drupal_account} WHERE drupal_uid = :drupal_uid", array(
		':drupal_uid' => $user->uid
  ))
  or drupal_set_message(t('Unable to retrieve drupal-zabbix associations'), DRUPAL_MSG_TYPE_ERR);

  $data = db_fetch_array($result);

  if (!$data) {

    drupal_set_message(t('No drupal-zabbix association exists for this user!'), DRUPAL_MSG_TYPE_ERR);

    return;
  }

  zabbix_bridge_debug(print_r($data, TRUE));

  zabbix_api_login()
  or drupal_set_message(t('Unable to login. ') . print_r(ZabbixAPI::getLastError(), TRUE), DRUPAL_MSG_TYPE_ERR);

  // get user media
  $media = ZabbixAPI::fetch_array('user', 'getMedia', array(
              'userids' => array($data['zabbix_uid']),
              'extendoutput' => TRUE
  ));

  $result = ZabbixAPI::fetch_string('user', 'getMedia', $media)
  or drupal_set_message(t('Unable to retrieve user media. ') . print_r(ZabbixAPI::getLastError(), TRUE), DRUPAL_MSG_TYPE_ERR);

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

    drupal_set_message(t('Unable to enable/disable mobile. ') . print_r(ZabbixAPI::getLastError(), TRUE), DRUPAL_MSG_TYPE_ERR);
  }
  else {

    $sql = "update {zabbix_mobiles} set enabled = '%s' where zabbixmediaid = %s";
    $result = db_query($sql, $enabled ? 0 : 1, $zabbixmediaid);
  }

  return $result;
}

/**
 *
 * @return <type>
 */
function zabbix_bridge_import_mobiles() {

  $mobiles = array();
  $mobiles = zabbix_bridge_get_all_mobiles_from_zabbix(TRUE);

  zabbix_bridge_debug(print_r($mobiles, TRUE));

  foreach ($mobiles as $mobile) {

    $sql = "SELECT mobileid FROM {zabbix_mobiles} WHERE zabbixmediaid='%s'";
    $results = db_query($sql, $mobile['mediaid']);

    $mediaid = db_fetch_object($results);

    if ($mediaid) {

      drupal_set_message(t('Skipped mobile ' . $mobile['mediaid'] . ' (' . $mobile['sendto'] . '), as it already exists.'));
    }
    else {

      $sql = "SELECT drupal_uid FROM {zabbix_drupal_account} WHERE zabbix_uid='%s'";
      $results = db_query($sql, $mobile['userid']);

      zabbix_bridge_debug($sql);

      $user = db_fetch_object($results);

      if ($user) {

        $sql = "INSERT INTO {zabbix_mobiles} (userid, zabbixmediaid, number, enabled) VALUES ('%s', '%s', '%s', '%s')";
        $results = db_query($sql, $user->drupal_uid, $mobile['mediaid'], $mobile['sendto'], $mobile['active']);
      }
    }
  }

  drupal_goto("admin/settings/zabbix_bridge");
}
