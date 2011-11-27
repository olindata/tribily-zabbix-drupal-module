<?php

// $Id$

/*
 * @file
 * For handling multiple emails
 *
 * @author rajkissu
 */

/**
 * Form builder; Present the form to edit a given user's emails.
 *
 * @ingroup forms
 */
function zabbix_bridge_emails() {

  global $user;

  drupal_set_title('Emails');

  $output = zabbix_emails_table($user->uid);
  $output .= l('Add Email', 'emails/add');

  return $output;
}

/**
 * Form builder; add/edit a user's emails.
 *
 * @ingroup forms
 * @see zabbix_bridge_emails_form_validate()
 * @see zabbix_bridge_emails_form_submit()
 */
function zabbix_bridge_emails_form(&$form_state, $operation, $emailid = NULL) {
  global $user;

  // Get details of email if we're updating
  if (($operation == 'update') && isset($emailid)) {

    $sql = "select userid, emailid, email, zabbixmediaid, enabled from {zabbix_emails} where emailid = '%s'";
    $result = db_query($sql, $emailid);
    $email = db_fetch_object($result);

    // Access Denied
    if($email->userid != $user->uid)
    {
        drupal_set_message("Access Denied - You have not the permissions to update this object", DRUPAL_MSG_TYPE_ERR);
        return;
    }
  }

  // Set display fo form to reflect operation
  if ('add' == $operation) {

    $title = 'Add Email';
    $submit = 'Add';
  }
  else {

    $title = 'Update Email';
    $submit = 'Update';
  }

  // Put it in the form
  $form['emails'] = array('#type' => 'fieldset',
      '#title' => t($title),
      '#weight' => -10
  );

  // The email's id
  $form['emails']['emailid'] = array(
      '#type' => 'hidden',
      '#value' => $emailid,
  );

  // the email's email
  $form['emails']['email'] = array(
      '#type' => 'textfield',
      '#title' => t('Email'),
      '#maxlength' => MEDIA_NAME_MAXLENGTH,
      '#value' => isset($email->email) ? $email->email : '',
      '#required' => TRUE
  );

  // status - enabled/disabled
  $form['emails']['status'] = array('#type' => 'select',
      '#title' => t('Status'),
      '#default_value' => isset($email->enabled) ?
              ($email->enabled == 1) ? 'disabled' : 'enabled' :
              'enabled',
      '#options' => array(
          'enabled' => 'enabled',
          'disabled' => 'disabled',
      ),
      '#description' => t('Specify status'),
      '#required' => TRUE
  );

  // make the severity SELECT options
  $result = db_query('SELECT severityid, name FROM {zabbix_severities}')
          or drupal_set_message('Unable to retrieve zabbix severities', DRUPAL_MSG_TYPE_ERR);

  $template_options = array();

  while ($data = db_fetch_array($result)) {

    $template_options[$data['severityid']] = $data['name'];
  }

  $default_value = array();

  // get the assigned severities for this email
  if (isset($email->enabled)) {

    $sql = "select severityid from {zabbix_emails_severities} where emailid = '%s'";
    $result = db_query($sql, $emailid);

    while ($data = db_fetch_array($result)) {

      $default_value[] = $data['severityid'];
    }
  }


  $form['emails']['severities'] = array(
      '#type' => 'checkboxes',
      '#title' => t('Severities'),
      '#default_value' => $default_value,
      '#options' => $template_options,
      '#description' => t('Assign one or more severities to this email.')
  );

  $form['emails']['submit'] = array('#type' => 'submit', '#value' => t($submit), '#weight' => 30);

  return $form;
}

/**
 * Submit function for the user email add/editing form. Note that it handles adding _AND_ updating
 *
 * @global object $user
 * @param array $form
 * @param array $form_state
 * @return <type>
 */
function zabbix_bridge_emails_form_submit($form, &$form_state) {
  // Global drupal user object
  global $user;

  // if this was an existing email, retrieve it
  if (isset($form_state['clicked_button']['#post']['emailid'])) {
    $emailid = $form_state['clicked_button']['#post']['emailid'];
  }

  if ($emailid <> '') {
    // Get the zabbix userid belonging to the emailid we got from the submitted form
    $result = db_query("SELECT zda.zabbix_uid, zda.drupal_uid FROM {zabbix_drupal_account} zda inner join {zabbix_emails} ze on ze.userid = zda.drupal_uid where ze.emailid = :emailid", array(
		':emailid' => $emailid)
		)
            or drupal_set_message('Unable to retrieve drupal-zabbix associations', DRUPAL_MSG_TYPE_ERR);
    $data = db_fetch_array($result);
    $zabbix_uid = $data['zabbix_uid'];

    // Access Denied
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
            or drupal_set_message('Unable to retrieve drupal-zabbix associations', DRUPAL_MSG_TYPE_ERR);
    $data = db_fetch_array($result);
    $zabbix_uid = $data['zabbix_uid'];
  }

  // Get some of the posted values from the form
  // 0 - enabled, 1 - disabled
  $status = $form_state['clicked_button']['#post']['status'];
  $severities = $form_state['clicked_button']['#post']['severities'];
  $severity = zabbix_bridge_calculate_severity($severities);
  $sendto = $form_state['clicked_button']['#post']['email'];

  // Fill an array to send to zabbix
  $email = array();
  $email['users'] = array('userid' => $zabbix_uid);
  $email['medias'] = array(
      'mediatypeid' => 1,
      'active' => ('enabled' == $status) ? 0 : 1,
      'severity' => $severity,
      'period' => '1-7,00:00-23:59;',
      'sendto' => $sendto
  );

  // This logs into Zabbix, and returns FALSE if it fails
  zabbix_api_login()
          or drupal_set_message('Unable to login: ' . print_r(ZabbixAPI::getLastError(), TRUE), DRUPAL_MSG_TYPE_ERR);

  //create the email if it's not already existing
  if ($emailid == '') {

    zabbix_bridge_debug(print_r($email, TRUE));
    $addedmedia = ZabbixAPI::fetch_string('user', 'addMedia', $email);

    if ($addedmedia) {
      drupal_set_message('Email successfully added.');
    }
    else {
      drupal_set_message('Failed to add email: ' . print_r(ZabbixAPI::getLastError(), TRUE), DRUPAL_MSG_TYPE_ERR);
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

    $sql = "select zabbixmediaid from {zabbix_emails} where emailid = '%s'";
    $result = db_query($sql, $emailid);

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

    //update the email and enabled status
    $usermedia = ZabbixAPI::fetch_string('user', 'updateMedia', $usermedia)
            or drupal_set_message('Problem updating existing email properties', DRUPAL_MSG_TYPE_ERR);

    zabbix_bridge_debug('update: ' . print_r($usermedia, TRUE));
  }

  // get user media
  $usermedia = ZabbixAPI::fetch_array('user', 'getMedia', array(
              'userids' => array($zabbix_uid),
              'mediatypeid' => 1,
              'sendto' => $sendto,
              'extendoutput' => TRUE
          ));

  if (!$usermedia || !count($usermedia)) {

    return;
  }

  // add the email mapping to the drupal database so we can use it for display without contacting zabbix
  $sql = 'INSERT INTO {zabbix_emails} (userid, emailid, zabbixmediaid, email, enabled)';
  $sql .= " VALUES('%s', '%s', '%s', '%s', '%s')";
  $sql .= " ON DUPLICATE KEY UPDATE email='%s', enabled='%s'";

  foreach ($usermedia as $media) {

    $zabbixmediaid = $media['mediaid'];
    $active = $media['active'];

    $result = db_query($sql, $user->uid, $emailid, $zabbixmediaid, $sendto, $active, $sendto, $active);

    if (!$result) {

      drupal_set_message('Problem adding email-mapping to database', DRUPAL_MSG_TYPE_ERR);
    }
  }

  if ($emailid == '') {

    $emailid = db_last_insert_id('zabbix_emails', 'emailid');
  }

  // if severities was added
  // Delete all previous email mappings, so we can add the new ones in a sec
  $sql = "DELETE FROM {zabbix_emails_severities} WHERE emailid = '%s'";
  db_query($sql, $emailid) or
          drupal_set_message('Error updating severities in drupal database.');

  // addd the email-severity mapping to the drupal db so we can use it for display without contacting zabbix
  $sql = 'INSERT INTO {zabbix_emails_severities} (emailid, severityid)';
  $sql .= " VALUES('%s', '%s')";

  if (count($severities) > 0) {

    foreach ($severities as $severityid) {

      $result = db_query($sql, $emailid, $severityid);

      if (!$result) {

        drupal_set_message('Problem adding email-severity-mapping to database', DRUPAL_MSG_TYPE_ERR);
      }
    }
  }

  drupal_goto("emails/overview");

  return;
}

/**
 *
 * @param <type> $form_state
 * @param <type> $email
 * @return <type>
 */
function zabbix_bridge_emails_delete_confirm(&$form_state, $email) {

  $form['_email'] = array(
      '#type' => 'value',
      '#value' => $email,
  );

  return confirm_form(
          $form,
          t('Are you sure you want to delete this email?'),
          isset($_GET['destination']) ? $_GET['destination'] : "emails/overview",
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
function zabbix_bridge_emails_delete_confirm_submit($form, &$form_state) {

  if ($form_state['values']['confirm']) {

    $email = $form_state['values']['_email'];

    if (zabbix_bridge_emails_delete($email)) {

      drupal_set_message(t('Email has been deleted successfully.'));
    }
    else {

      drupal_set_message(t('Email deletion unsuccessful'), DRUPAL_MSG_TYPE_ERR);
    }
  }

  drupal_goto("emails/overview");
}

/**
 * Deletes an email and it's assigned severity from the database, both on the drupal
 * side and on the zabbix side
 *
 * @param  array    $formvalues ...
 * @param  int      $emailid     The drupal id of the email to be deleted
 * @return bool
 */
function zabbix_bridge_emails_delete($emailid) {

  $sql = "select zabbixmediaid from {zabbix_emails} where emailid = '%s'";
  $zabbixmediaid = db_result(db_query($sql, $emailid));

  zabbix_api_login()
          or drupal_set_message('Unable to login. ' . print_r(ZabbixAPI::getLastError(), TRUE), DRUPAL_MSG_TYPE_ERR);

  ZabbixAPI::fetch_string('user', 'deleteMedia', array($zabbixmediaid))
          or drupal_set_message('Unable to delete media. ' . print_r(ZabbixAPI::getLastError(), TRUE), DRUPAL_MSG_TYPE_ERR);

  $sql = "delete from {zabbix_emails_severities} where emailid = '%s'";
  $result = db_query($sql, $emailid);

  $sql = "delete from {zabbix_emails} where emailid = '%s'";
  $result = db_query($sql, $emailid);

  return $result;
}

/**
 *
 * @param <type> $form_state
 * @param <type> $email
 * @return <type>
 */
function zabbix_bridge_emails_enable_disable_confirm(&$form_state, $email) {
  $form['_email'] = array(
      '#type' => 'hidden',
      '#value' => $email,
  );

  $sql = "select enabled from {zabbix_emails} where emailid = '%s'";
  $enabled = db_result(db_query($sql, $email));

  $form['_current_state'] = array(
      '#type' => 'hidden',
      '#value' => $enabled,
  );

  return confirm_form(
          $form,
          t('Are you sure you want to change the enabled state of this email?'),
          isset($_GET['destination']) ? $_GET['destination'] : "emails/overview",
          $enabled ? t('This email will be enabled until you disable it.') :
                  t('This email will be disabled until you re-enable it.'),
          t('OK'),
          t('Cancel')
  );
}

/**
 *
 * @param array $form
 * @param array $form_state
 */
function zabbix_bridge_emails_enable_disable_confirm_submit($form, &$form_state) {

  if ($form_state['values']['confirm']) {

    $email = $form_state['values']['_email'];
    $zabbixmediaid = zabbix_bridge_drupal_to_zabbix_emailid($email);
    $enabled = $form_state['values']['_current_state'];

    if (zabbix_bridge_emails_enable_disable($zabbixmediaid, $enabled)) {

      drupal_set_message(t('Email has been updated successfully.'));
    }
    else {

      drupal_set_message(t('Email update unsuccessful'), DRUPAL_MSG_TYPE_ERR);
    }
  }

  drupal_goto("emails/overview");
}

/**
 *
 * @param int $emailid
 * @param int $enabled
 * @return <type>
 */
function zabbix_bridge_emails_enable_disable($zabbixmediaid, $enabled) {

  global $user;

  $result = db_query("SELECT zabbix_uid FROM {zabbix_drupal_account} WHERE drupal_uid = :drupal_uid", array(
		':drupal_uid' => $user->uid
		))
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

    drupal_set_message('Unable to enable/disable email. ' . print_r(ZabbixAPI::getLastError(), TRUE), DRUPAL_MSG_TYPE_ERR);
  }
  else {

    $sql = "update {zabbix_emails} set enabled = '%s' where zabbixmediaid = %s";
    $result = db_query($sql, $enabled ? 0 : 1, $zabbixmediaid);
  }

  return $result;
}

/**
 *
 * @return <type>
 */
function zabbix_bridge_import_emails() {

  $emails = array();
  $emails = zabbix_bridge_get_all_emails_from_zabbix(TRUE);

  zabbix_bridge_debug(print_r($emails, TRUE));

  foreach ($emails as $email) {

    $sql = "SELECT emailid FROM {zabbix_emails} WHERE zabbixmediaid='%s'";
    $results = db_query($sql, $email['mediaid']);

    $mediaid = db_fetch_object($results);

    if ($mediaid) {

      drupal_set_message('Skipped email ' . $email['mediaid'] . ' (' . $email['sendto'] . '), as it already exists.');
    }
    else {

      $sql = "SELECT drupal_uid FROM {zabbix_drupal_account} WHERE zabbix_uid='%s'";
      $results = db_query($sql, $email['userid']);

      $user = db_fetch_object($results);

      if ($user) {

        $sql = "INSERT INTO {zabbix_emails} (userid, zabbixmediaid, email, enabled) VALUES ('%s', '%s', '%s', '%s')";
        $results = db_query($sql, $user->drupal_uid, $email['mediaid'], $email['sendto'], $email['active']);
      }
    }
  }

  drupal_goto("admin/settings/zabbix_bridge");
}
