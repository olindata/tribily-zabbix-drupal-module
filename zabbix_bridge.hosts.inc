<?php

// $Id$

/**
 * @file
 * Settings form for hosts
 *
 * @author raj
 */
include_once 'zabbix_functions.php';

/**
 * Form builder; Present the form to edit a given user's hosts.
 *
 * @ingroup forms
 */
function zabbix_hosts() {
  global $user;

  drupal_set_title('Hosts to monitor');

  $output = zabbix_hosts_table($user->uid);

  $output .= l('Add host', 'hosts/add');

  return $output;
}

/**
 *
 * @param int $hostid
 * @return object
 */
function host_user_load($hostid) {
  $query = "SELECT * FROM {zabbix_hosts} WHERE hostid = '%d'";
  $rs = db_query($query, $hostid);

  if ($rs) {
    while ($data = db_fetch_object($rs)) {
      return $data;
    }
  }

  return FALSE;
}

/**
 *
 * @param <type> $form_state
 * @param <type> $host
 * @return <type>
 */
function host_delete_confirm(&$form_state, $host) {
  $form['_host'] = array(
      '#type' => 'value',
      '#value' => $host,
  );

  return confirm_form(
          $form,
          t('Are you sure you want to delete this host?'),
          isset($_GET['destination']) ? $_GET['destination'] : "hosts/overview",
          t('This action cannot be undone.'),
          t('Delete'),
          t('Cancel')
  );
}

/**
 *
 * @param <type> $form_state
 * @param <type> $host
 * @return <type>
 */
function host_enable_disable_confirm(&$form_state, $host) {
  $form['_host'] = array(
      '#type' => 'hidden',
      '#value' => $host,
  );

  $sql = "select enabled from {zabbix_hosts} where hostid = '%s'";
  $enabled = db_result(db_query($sql, $host));

  $form['_current_state'] = array(
      '#type' => 'hidden',
      '#value' => $enabled,
  );

  return confirm_form(
          $form,
          t('Are you sure you want to change the enabled state of this host?'),
          isset($_GET['destination']) ? $_GET['destination'] : "hosts/overview",
          $enabled ? t('This host will be monitored until you disable it.') :
                  t('This host will not be monitored until you re-enable it.'),
          t('OK'),
          t('Cancel')
  );
}

/**
 *
 * @param array $form
 * @param array $form_state
 */
function host_enable_disable_confirm_submit($form, &$form_state) {
  if ($form_state['values']['confirm']) {
    $host = $form_state['values']['_host'];
    $zabbixhostid = zabbix_bridge_drupal_to_zabbix_hostid($host);
    $enabled = $form_state['values']['_current_state'];

    if (zabbix_host_enable_disable($zabbixhostid, $enabled)) {
      drupal_set_message(t('Host has been updated successfully.'));
    }
    else {
      drupal_set_message(t('Host update unsuccessful'), DRUPAL_MSG_TYPE_ERR);
    }
  }

  drupal_goto("hosts/overview");
}

/**
 *
 * @param array $form
 * @param array $form_state
 */
function host_delete_confirm_submit($form, &$form_state) {
  if ($form_state['values']['confirm']) {
    $host = $form_state['values']['_host'];

    if (zabbix_host_delete($host)) {
      drupal_set_message(t('Host has been deleted successfully.'));
    }
    else {
      drupal_set_message(t('Host deletion unsuccessful'), DRUPAL_MSG_TYPE_ERR);
    }
  }

  drupal_goto("hosts/overview");
}

/**
 *
 * @param int $hostid
 * @param int $enabled
 * @return <type>
 */
function zabbix_host_enable_disable($hostid, $enabled) {

  zabbix_api_login()
          or drupal_set_message('Unable to login. ' . print_r(ZabbixAPI::getLastError(), TRUE), DRUPAL_MSG_TYPE_ERR);

  $result = ZabbixAPI::fetch_string('host', 'update', array('hostid' => $hostid, 'status' => $enabled ? 0 : 1));

  if (!$result) {
    drupal_set_message('Unable to enable/disable host. ' . print_r(ZabbixAPI::getLastError(), TRUE), DRUPAL_MSG_TYPE_ERR);
  }
  else {
    $sql = "update {zabbix_hosts} set enabled = '%s' where zabbixhostid = %s";
    $result = db_query($sql, $enabled ? 0 : 1, $hostid);
  }

  return $result;
}

/**
 * Deletes a host and it's assigned roles from the database, both on the drupal 
 * side and on the zabbix side
 * 
 * @param  array    $formvalues ...
 * @param  int      $hostid     The drupal id of the host to be deleted  
 * @return bool                 
 */
function zabbix_host_delete($hostid) {

  $sql = "select zabbixhostid from {zabbix_hosts} where hostid = '%s'";
  $zabbixhostid = db_result(db_query($sql, $hostid));

  zabbix_api_login()
          or drupal_set_message('Unable to login. ' . print_r(ZabbixAPI::getLastError(), TRUE), DRUPAL_MSG_TYPE_ERR);

  ZabbixAPI::fetch_string('host', 'delete', array($zabbixhostid))
          or drupal_set_message('Unable to delete host. ' . print_r(ZabbixAPI::getLastError(), TRUE), DRUPAL_MSG_TYPE_ERR);

  $sql = "delete from {zabbix_hosts_roles} where hostid = '%s'";
  $result = db_query($sql, $hostid);

  $sql = "delete from {zabbix_hosts} where hostid = '%s'";
  $result = db_query($sql, $hostid);

  return $result;
}

/**
 * Form builder; add/edit a user's hosts.
 * 
 * @ingroup forms
 * @see zabbix_hosts_form_validate()
 * @see zabbix_hosts_form_submit()
 */
function zabbix_bridge_hosts_form(&$form_state, $operation, $hostid = NULL) {

  if (($operation == 'update') && isset($hostid)) {
    $sql = "select userid, hostid, zabbixhostid, hostname, enabled from {zabbix_hosts} where hostid = '%s'";
    $result = db_query($sql, $hostid);
    $host = db_fetch_object($result);
  }

  if ('add' == $operation) {
    $title = 'Add Host';
    $submit = 'Add';
  }
  else {
    $title = 'Update host';
    $submit = 'Update';
  }

  $form['hosts'] = array('#type' => 'fieldset',
      '#title' => t($title),
      '#weight' => -10
  );

  $form['hosts']['hostid'] = array(
      '#type' => 'hidden',
      '#value' => $hostid,
  );

  // the host's name
  $form['hosts']['name'] = array(
      '#type' => 'textfield',
      '#title' => t('Name'),
      '#maxlength' => HOST_NAME_MAXLENGTH,
      '#value' => isset($host->hostname) ? $host->hostname : '',
      '#required' => TRUE
  );

  // status - monitored/unmonitored
  $form['hosts']['status'] = array('#type' => 'select',
      '#title' => t('Status'),
      '#default_value' => isset($host->enabled) ?
              ($host->enabled == 1) ? 'unmonitored' : 'monitored' :
              'monitored',
      '#options' => array(
          'monitored' => 'monitored',
          'unmonitored' => 'unmonitored',
      ),
      '#description' => t('Specify monitoring status'),
      '#required' => TRUE
  );

  // make the template SELECT options
  $result = db_query('SELECT roleid, name FROM {zabbix_role}')
          or drupal_set_message('Unable to retrieve zabbix roles', DRUPAL_MSG_TYPE_ERR);

  $template_options = array();

  while ($data = db_fetch_array($result)) {
    $template_options[$data['roleid']] = $data['name'];
  }

  $default_value = array();

  // get the assigned roles for this host
  if (isset($host->enabled)) {

    $sql = "select roleid from {zabbix_hosts_roles} where hostid = '%s'";
    $result = db_query($sql, $hostid);

    while ($data = db_fetch_array($result)) {
      $default_value[] = $data['roleid'];
    }
  }


  $form['hosts']['roles'] = array(
      '#type' => 'checkboxes',
      '#title' => t('Roles'),
      '#default_value' => $default_value,
      '#options' => $template_options,
      '#description' => t('Assign this host to one or more roles.')
  );

  $form['hosts']['submit'] = array('#type' => 'submit', '#value' => t($submit), '#weight' => 30);

  return $form;
}

/**
 * Submit function for the user host add/editing form. Note that it handles adding _AND_ updating
 *
 * @global object $user
 * @param array $form
 * @param array $form_state
 * @return <type>
 */
function zabbix_bridge_hosts_form_submit($form, &$form_state) {
  global $user;

  $result = db_query("SELECT zabbix_hostgrp_id FROM {zabbix_drupal_account} WHERE drupal_uid='%s'", $user->uid)
          or drupal_set_message('Unable to retrieve drupal-zabbix associations', DRUPAL_MSG_TYPE_ERR);

  $data = db_fetch_array($result);

  if (!$data)
    drupal_set_message('No drupal-zabbix association exists for this user!', DRUPAL_MSG_TYPE_ERR);

  $host = array();
  $host['host'] = $form_state['clicked_button']['#post']['name'];
  $host['groups'] = array('groupid' => $data['zabbix_hostgrp_id']);

  // 0 - status monitored, 1 - status not monitored
  $status = $form_state['clicked_button']['#post']['status'];
  $host['status'] = ('monitored' == $status) ? 0 : 1;

  if (isset($form_state['clicked_button']['#post']['hostid'])) {
    $hostid = $form_state['clicked_button']['#post']['hostid'];
  }

  // This logs into Zabbix, and returns FALSE if it fails
  zabbix_api_login()
          or drupal_set_message('Unable to login: ' . print_r(ZabbixAPI::getLastError(), TRUE), DRUPAL_MSG_TYPE_ERR);

  //create the host if it's not already existing
  if ($hostid == '') {
    zabbix_bridge_debug(drupal_set_message("host data: ". print_r($host, TRUE)));
    $zabbixhostid = ZabbixAPI::fetch_string('host', 'create', array($host));
    if ($zabbixhostid) {
      drupal_set_message('Host successfully added.');
      zabbix_bridge_debug(drupal_set_message("zabbixhostid: $zabbixhostid"));
    }
    else {
      drupal_set_message('Failed to add host' . print_r(ZabbixAPI::getLastError(), TRUE), DRUPAL_MSG_TYPE_ERR);
    }
  }
  else {
    $sql = "select zabbixhostid from {zabbix_hosts} where hostid = '%s'";
    $result = db_query($sql, $hostid);
    $data = db_fetch_array($result);
    $zabbixhostid = $data['zabbixhostid'];

    //update the hostname and enabled status
    ZabbixAPI::fetch_string('host', 'update', array('hostid' => $zabbixhostid, 'host' => $host['host'], 'status' => $host['status']))
            or drupal_set_message('Problem updating existing host properties', DRUPAL_MSG_TYPE_ERR);
  }

  //remove all roles from the host, so we can add the new ones later on
  if ($hostid != '') {
    $sql = "select templateid from {zabbix_hosts_roles} zhr inner join {zabbix_roles_templates} zrt on zrt.roleid = zhr.roleid where zhr.hostid = '%s'";
    $result = db_query($sql, $hostid);

    $template_ids = array();
    while ($data = db_fetch_array($result)) {
      $template_ids[] = $data['templateid'];
    }

    if (ZabbixAPI::fetch_string('template', 'massRemove', array('templates' => $template_ids,
                'hosts' => array('hostid' => $zabbixhostid)))) {
      drupal_set_message('Old templates removed from host succesfully');
    }
    else {
      drupal_set_message('Problem removing old templates', DRUPAL_MSG_TYPE_ERR);
    }
  }

  $roles = $form_state['clicked_button']['#post']['roles'];
  zabbix_bridge_debug('$roles: ' . print_r($roles, TRUE));

  $role_ids = array();
  $rolesadded = 0;

  // get the role ids
  if (count($roles) > 0) {

    foreach ($roles as $key => $value) {
      if (0 === $value) {
        continue;
      }
      $role_ids[] = $key;
      $rolesadded += 1;
    }
  }

  if ($rolesadded > 0) {
    // retrieve zabbix template ids based on role ids
    $result2 = db_query("SELECT * FROM {zabbix_roles_templates} WHERE roleid IN ('". implode("','", $role_ids)."')");
    
    $addedtemplate_ids = array();
    while ($data2 = db_fetch_array($result2)) {
      $addedtemplate_ids[$data2['templateid']] = $data2['templateid'];
      zabbix_bridge_debug('$data2: ' . print_r($data2, TRUE));
    }

    zabbix_bridge_debug('$addedtemplate_ids: ' . print_r($addedtemplate_ids, TRUE));

    $template_add = ZabbixAPI::fetch_string('template', 'massAdd', array('templates' => $addedtemplate_ids,
                'hosts' => array('hostid' => $zabbixhostid)));

    //zabbix_bridge_debug($template_add);

    if (!$template_add) {
      drupal_set_message('Unable to link templates: ' . print_r(ZabbixAPI::getLastError(), TRUE), DRUPAL_MSG_TYPE_ERR);
    }
    else {
      drupal_set_message('Host succesfully added to zabbix with selected roles.');
    }
  }

  // add the host mapping to the drupal database so we can use it for display without contacting zabbix
  $sql = 'INSERT INTO {zabbix_hosts} (userid, hostid, zabbixhostid, hostname, enabled)';
  $sql .= " VALUES('%s', '%s', '%s', '%s', '%s')";
  $sql .= " ON DUPLICATE KEY UPDATE hostname='%s', enabled='%s'";

  $result = db_query($sql, $user->uid, $hostid, $zabbixhostid, $host['host'], $host['status'], $host['host'], $host['status']);

  if (!$result) {
    drupal_set_message('Problem adding hostmapping to database', DRUPAL_MSG_TYPE_ERR);
  }
  elseif ($hostid == '') {
    $hostid = db_last_insert_id('zabbix_hosts', 'hostid');
  }

  // if the templates was added to zabbix succesfully
  if ($template_add) {

    // Delete all previous host mappings, so we can add the new ones in a sec
    $sql = "DELETE FROM {zabbix_hosts_roles} WHERE hostid = '%s'";
    db_query($sql, $hostid) or
            drupal_set_message('Error updating roles in drupal database.');

    // addd the host-role mapping to the drupal db so we can use it for display without contacting zabbix
    $sql = 'INSERT INTO {zabbix_hosts_roles} (hostid, roleid)';
    $sql .= " VALUES('%s', '%s')";

    if (count($role_ids) > 0) {

      foreach ($role_ids as $roleid) {

        $result = db_query($sql, $hostid, $roleid);
        if (!$result) {
          drupal_set_message('Problem adding host-role-mapping to database', DRUPAL_MSG_TYPE_ERR);
        }
      }
    }
  }

  drupal_goto("hosts/overview");

  return;
}

function zabbix_bridge_import_hosts() {
  // Get a list of all hosts from zabbix
  $zabbixhosts = zabbix_bridge_get_all_hosts();

  zabbix_bridge_debug(print_r($zabbixhosts, TRUE));
  // Go through the hosts and try to map them to the drupal users
  foreach ($zabbixhosts as $zabbixhost) {
    $zabbixuserid = zabbix_bridge_get_userid_by_hostgroupid($zabbixhost['hostgroupid']);

    zabbix_bridge_add_host_to_zabbix_userid($zabbixuserid, $zabbixhost['hostid'], $zabbixhost['name'], $zabbixhost['enabled'], $zabbixhost['templateids']);
  }

  drupal_goto("admin/settings/zabbix_bridge");

  return TRUE;
}


