<?php

// $Id$

include_once('lib/ZabbixAPI.class.php');

define('HOST_NAME_MAXLENGTH', 64);
define('MEDIA_NAME_MAXLENGTH', 100);
define('HOST_SERVER_MAXLENGTH', 64);
define('DRUPAL_MSG_TYPE_ERR', 'error');
define('DRUPAL_MSG_TYPE_STATUS', 'status');
define('PAGER_COUNT', 100);
define('TABLE_ID_USER_MAPPING', 100);
define('TABLE_ID_HOSTS_MAPPING', 101);
define('TABLE_ID_EMAILS_MAPPING', 102);

//the permissions defined/used by this module
define('PERM_ADMIN', 'administer site configuration');
define('PERM_HOSTS_SELECT', 'access hosts');
define('PERM_HOSTS_INSERT', 'add hosts');
define('PERM_HOSTS_DELETE', 'delete hosts');
define('PERM_HOSTS_UPDATE', 'update hosts');
define('PERM_JABBER_SELECT', 'access jabbers');
define('PERM_JABBER_INSERT', 'add jabbers');
define('PERM_JABBER_DELETE', 'delete jabbers');
define('PERM_JABBER_UPDATE', 'update jabbers');
define('PERM_MOBILES_SELECT', 'access mobiles');
define('PERM_MOBILES_INSERT', 'add mobiles');
define('PERM_MOBILES_DELETE', 'delete mobiles');
define('PERM_MOBILES_UPDATE', 'update mobiles');
define('PERM_EMAIL_SELECT', 'access emails');
define('PERM_EMAIL_INSERT', 'add emails');
define('PERM_EMAIL_DELETE', 'delete emails');
define('PERM_EMAIL_UPDATE', 'update emails');

// from zabbix/include/defines.inc.php
define('PERM_READ_WRITE', 3);
define('PERM_READ_ONLY', 2);
define('PERM_READ_LIST', 1);
define('PERM_DENY', 0);

define('EVENT_SOURCE_TRIGGERS', 0);
define('EVENT_SOURCE_DISCOVERY', 1);
define('EVENT_SOURCE_AUTO_REGISTRATION', 2);

define('ACTION_STATUS_ENABLED', 0);
define('ACTION_STATUS_DISABLED', 1);

define('ACTION_DEFAULT_MSG', '{TRIGGER.NAME}: {STATUS}');

define('ACTION_EVAL_TYPE_AND_OR', 0);
define('ACTION_EVAL_TYPE_AND', 1);
define('ACTION_EVAL_TYPE_OR', 2);

define('CONDITION_TYPE_HOST_GROUP', 0);
define('CONDITION_TYPE_HOST', 1);
define('CONDITION_TYPE_TRIGGER', 2);
define('CONDITION_TYPE_TRIGGER_NAME', 3);
define('CONDITION_TYPE_TRIGGER_SEVERITY', 4);
define('CONDITION_TYPE_TRIGGER_VALUE', 5);
define('CONDITION_TYPE_TIME_PERIOD', 6);
define('CONDITION_TYPE_DHOST_IP', 7);
define('CONDITION_TYPE_DSERVICE_TYPE', 8);
define('CONDITION_TYPE_DSERVICE_PORT', 9);
define('CONDITION_TYPE_DSTATUS', 10);
define('CONDITION_TYPE_DUPTIME', 11);
define('CONDITION_TYPE_DVALUE', 12);
define('CONDITION_TYPE_HOST_TEMPLATE', 13);
define('CONDITION_TYPE_EVENT_ACKNOWLEDGED', 14);
define('CONDITION_TYPE_APPLICATION', 15);
define('CONDITION_TYPE_MAINTENANCE', 16);
define('CONDITION_TYPE_NODE', 17);
define('CONDITION_TYPE_DRULE', 18);
define('CONDITION_TYPE_DCHECK', 19);
define('CONDITION_TYPE_PROXY', 20);
define('CONDITION_TYPE_DOBJECT', 21);
define('CONDITION_TYPE_HOST_NAME', 22);

define('TRIGGER_SEVERITY_NOT_CLASSIFIED', 0);
define('TRIGGER_SEVERITY_INFORMATION', 1);
define('TRIGGER_SEVERITY_WARNING', 2);
define('TRIGGER_SEVERITY_AVERAGE', 3);
define('TRIGGER_SEVERITY_HIGH', 4);
define('TRIGGER_SEVERITY_DISASTER', 5);

define('CONDITION_OPERATOR_EQUAL', 0);
define('CONDITION_OPERATOR_NOT_EQUAL', 1);
define('CONDITION_OPERATOR_LIKE', 2);
define('CONDITION_OPERATOR_NOT_LIKE', 3);
define('CONDITION_OPERATOR_IN', 4);
define('CONDITION_OPERATOR_MORE_EQUAL', 5);
define('CONDITION_OPERATOR_LESS_EQUAL', 6);
define('CONDITION_OPERATOR_NOT_IN', 7);

define('OPERATION_OBJECT_USER', 0);
define('OPERATION_OBJECT_GROUP', 1);

define('EVENT_ACK_DISABLED', '0');
define('EVENT_ACK_ENABLED', '1');

define('OPERATION_TYPE_MESSAGE', 0);
define('OPERATION_TYPE_COMMAND', 1);
define('OPERATION_TYPE_HOST_ADD', 2);
define('OPERATION_TYPE_HOST_REMOVE', 3);
define('OPERATION_TYPE_GROUP_ADD', 4);
define('OPERATION_TYPE_GROUP_REMOVE', 5);
define('OPERATION_TYPE_TEMPLATE_ADD', 6);
define('OPERATION_TYPE_TEMPLATE_REMOVE', 7);
define('OPERATION_TYPE_HOST_ENABLE', 8);
define('OPERATION_TYPE_HOST_DISABLE', 9);

//refers to database field media_type.type
//  NOTE: this is equal for every zabbix install
define('MEDIA_TYPE_EMAIL', 0);
define('MEDIA_TYPE_EXEC', 1);
define('MEDIA_TYPE_SMS', 2);
define('MEDIA_TYPE_JABBER', 3);

//refers to database field media_type.mediatypeid
// NOTE: this is/can be different for every zabbix install!!
define('ALERT_TYPE_EMAIL', 1);
define('ALERT_TYPE_JABBER', 2);
define('ALERT_TYPE_SMS', 3);

define('TRIGGER_VALUE_FALSE', 0);
define('TRIGGER_VALUE_TRUE', 1);
define('TRIGGER_VALUE_UNKNOWN', 2);

define('API_OUTPUT_SHORTEN', 'shorten');
define('API_OUTPUT_REFER', 'refer');
define('API_OUTPUT_EXTEND', 'extend');
define('API_OUTPUT_COUNT', 'count');
define('API_OUTPUT_CUSTOM', 'custom');

define('STR_CUST_HOSTGROUP', 'Customer Hostgroup ');
define('STR_CUST_USERGROUP', 'Customer Usergroup ');

define('STR_DEFAULT_ACTION_SUBJECT', '{STATUS}: {TRIGGER.NAME}');
define('STR_DEFAULT_ACTION_TEXT', 'Alert: {TRIGGER.NAME}\nStatus: {STATUS}\nSeverity: {TRIGGER.SEVERITY}\nHost: {IPADDRESS}\nHost dns - {HOST.DNS1}\nURL: {TRIGGER.URL}\nLast Value: {ITEM.LASTVALUE}\nEvent age - {EVENT.AGE}\nEvent start date and time - {EVENT.DATE} , {EVENT.TIME}\nAcknowledgement status - {EVENT.ACK.STATUS}\nAcknowledgement history - {EVENT.ACK.HISTORY}');
define('STR_DEFAULT_ACTION_SUBJECT_MOBILE', '{STATUS}');
define('STR_DEFAULT_ACTION_TEXT_MOBILE', '{TRIGGER.NAME}');
define('STR_DEFAULT_ACTION_SUBJECT_JABBER', '{STATUS}');
define('STR_DEFAULT_ACTION_TEXT_JABBER', 'Trigger: {TRIGGER.NAME}\nDate / Time: {EVENT.DATE} - {EVENT.TIME}\nHostname: {HOSTNAME}\nComment: {TRIGGER.COMMENT}');

/**
 *
 * @param <type> $hostid
 * @return <type>
 */
function zabbix_bridge_drupal_to_zabbix_hostid($hostid) {

  $sql = "select zabbixhostid from {zabbix_hosts} where hostid = '%d'";
  $result = db_query($sql, $hostid);

  $host = db_fetch_object($result);

  zabbix_bridge_debug(print_r($host, TRUE));

  return $host->zabbixhostid;
}

/**
 *
 * @global <type> $user
 * @param <type> $userid
 * @return string
 */
function zabbix_hosts_table($userid = NULL) {
  global $user;

  $rows = array();
  $count = PAGER_COUNT;
  $id = TABLE_ID_HOSTS_MAPPING;

  if (isset($userid)) {
    $header = array('Hostname', 'Enabled', 'Role name', 'Role Description', 'Actions');
    $results = pager_query("select h.hostid, h.hostname, h.zabbixhostid, h.enabled, r.name as role_name, r.description as role_desc from {zabbix_hosts} h left join {zabbix_hosts_roles} hr on hr.hostid = h.hostid left join {zabbix_role} r on r.roleid = hr.roleid where h.userid = '%s' order by h.hostname, r.name", $count, $id, null, $user->uid);
  }
  else {
    $header = array('Username', 'Email', 'Zabbix Hostid', 'Hostname', 'Enabled', 'Role name', 'Role Description', 'Actions');
    $results = pager_query("select u.name, u.mail, h.hostid, h.zabbixhostid, h.hostname, h.enabled, r.name as role_name, r.description as role_desc from {zabbix_hosts} h left join {zabbix_hosts_roles} hr on hr.hostid = h.hostid left join {zabbix_role} r on r.roleid = hr.roleid left join {users} u on u.uid = h.userid order by u.name, h.hostname, r.name", $count, $id);
  }


  $lastzabbixhostid = -1;

  while ($node = db_fetch_object($results)) {
    if (!isset($userid)) {
      $rows[] = array(
          $node->name,
          $node->mail,
          $lastzabbixhostid == $node->zabbixhostid ? '' : $node->zabbixhostid,
          $lastzabbixhostid == $node->zabbixhostid ? '' : $node->hostname,
          $lastzabbixhostid == $node->zabbixhostid ? '' : ($node->enabled == 0 ? 'enabled' : 'disabled'),
          $node->role_name,
          $node->role_desc,
          $lastzabbixhostid == $node->zabbixhostid ? '' : (
                  l($node->enabled == 0 ? 'Disable' : 'Enable', 'hosts/enable-disable/' . $node->hostid) . ' | ' .
                  l('Delete', 'hosts/delete/' . $node->hostid) . ' | ' .
                  l('Update', 'hosts/update/' . $node->hostid) . ' | ' .
                  l('Triggers', 'hosts/' . $node->hostid . '/triggers')
                  ),
      );
    }
    else {

      $rows[] = array(
          $lastzabbixhostid == $node->zabbixhostid ? '' : $node->hostname,
          $lastzabbixhostid == $node->zabbixhostid ? '' : ($node->enabled == 0 ? 'enabled' : 'disabled'),
          $node->role_name,
          $node->role_desc,
          $lastzabbixhostid == $node->zabbixhostid ? '' : (
                  l($node->enabled == 0 ? 'Disable' : 'Enable', 'hosts/enable-disable/' . $node->hostid) . ' | ' .
                  l('Delete', 'hosts/delete/' . $node->hostid) . ' | ' .
                  l('Update', 'hosts/update/' . $node->hostid) . ' | ' .
                  l('Triggers', 'hosts/' . $node->hostid . '/triggers')
                  ),
      );
    }
    $lastzabbixhostid = $node->zabbixhostid;
  }

  if ($lastzabbixhostid == -1) {
    if (!isset($userid)) {
      $rows[] = array(t('No Hosts have been defined yet.'), '', '', '', '', '', '', '',);
    }
    else {
      $rows[] = array(t('No Hosts have been defined yet.'), '', '', '', '');
    }
  }

  $table_attributes = array('id' => 'hosts-table', 'align' => 'center');
  $output = theme('table', $header, $rows, $table_attributes) . theme('pager', $count, $id);

  return $output;
}

/**
 * thsi function outputs all the tables for the role mappings, one for each roletype
 */
function zabbix_roletype_tables() {

  $output = '';

  // get all role types from the db
  $sql = "select typeid, typename from {zabbix_role_type} order by sortorder";
  $result = db_query($sql);

  while ($record = db_fetch_object($result)) {
    
    $output .= "<h3>$record->typename</h3>";

    $output .= zabbix_roles_table($record->typeid);
  }

  return $output;

}

/**
 * returns the table with all roles for the sepcified roletypeid
 * @param <type> $typeid
 * @return string
 */
function zabbix_roles_table($typeid) {

  $header = array('Roleid', 'Name', 'Description', 'Image', 'Zabbix Templateid', 'Actions');
  $rows = array();
  $count = PAGER_COUNT;
  $id = TABLE_ID_USER_MAPPING;

  $results = pager_query("select zr.*, zrt.templateid from {zabbix_role} zr left join {zabbix_roles_templates} zrt on zrt.roleid = zr.roleid where zr.typeid = $typeid order by zr.sortorder", $count, $id);


  while ($node = db_fetch_object($results)) {

    // TODO: this shouldn't be static!
    $imagesrc = $node->imagesrc == NULL ? url('/sites/default/files/zabbix-roles/none.png') : url($node->imagesrc);

    $rows[] = array(
        $node->roleid,
        $node->name,
        $node->description,
        '<img src="'. $imagesrc ."\" alt=\"$imagesrc\" />",
        $node->templateid,
        l('Update', 'zabbix-role-mapping/update/' . $node->roleid) . ' | ' .
        l('Delete', 'zabbix-role-mapping/delete/' . $node->roleid),
    );
  }

  if (count($rows) == 0) {
    $rows[] = array(t('No templates have been defined yet'), '', '', '', '', '');
  }

  $table_attributes = array('id' => 'roles-table', 'align' => 'center');

  $output = theme('table', $header, $rows, $table_attributes) . theme('pager', $count, $id);

  return $output;
}

/**
 *
 * @return string
 */
function zabbix_user_mapping_table() {
  $header = array('Username', 'Email', 'Zabbix Userid', 'Zabbix Usergroupid', 'Zabbix Hostgroupid', 'Actions');
  $rows = array();
  $count = PAGER_COUNT;
  $id = TABLE_ID_USER_MAPPING;

  $results = pager_query("select zda.id, du.uid, du.name, du.mail, zda.zabbix_uid, zda.zabbix_usrgrp_id, zda.zabbix_hostgrp_id from {zabbix_drupal_account} zda left join {users} du on du.uid = zda.drupal_uid order by du.name", $count, $id);


  while ($node = db_fetch_object($results)) {
    $rows[] = array(
        $node->name,
        $node->mail,
        $node->zabbix_uid,
        $node->zabbix_usrgrp_id,
        $node->zabbix_hostgrp_id,
        l('Update', 'zabbix-user-mapping/update/' . $node->id) . ' | ' .
        l('Delete', 'zabbix-user-mapping/delete/' . $node->id) . ' | ' .
        l('Generate missing zabbix items', 'zabbix-user-mapping/generate/' . $node->id),
    );
  }

  if (count($rows) == 0) {
    $rows[] = array(t('No mappings have been defined yet'), '', '', '', '', '');
  }

  $table_attributes = array('id' => 'mapping-table', 'align' => 'center');

  $output = theme('table', $header, $rows, $table_attributes) . theme('pager', $count, $id);

  return $output;
}

/**
 *
 * @return <type>
 */
function zabbix_api_login() {

  // Get the credentials configured in this module
  $API_url = variable_get('zabbix_bridge_API_url', NULL);
  $API_user = variable_get('zabbix_bridge_API_user', NULL);
  $API_pass = variable_get('zabbix_bridge_API_pass', NULL);

  if (!isset($API_url, $API_user, $API_pass)) {
    drupal_set_message('Unable to login. API credentials must be set in module setting before using this module!', DRUPAL_MSG_TYPE_ERR);
  }

  // This logs into Zabbix, and returns FALSE if it fails
  return ZabbixAPI::login($API_url, $API_user, $API_pass);
}

/**
 * for printing out debugging info, use this instead of echo and what not.
 */
function zabbix_bridge_debug($msg, $verbosemsg = '') {
  // print the message only if debug is enabled!

  $show_verbose = FALSE;

  if ("yes" == variable_get('zabbix_bridge_debug', 'no')) {

    if ("yes" == variable_get('zabbix_bridge_debug_verbose', 'no')) {

      if ($verbosemsg && strlen($verbosemsg)) {

        $show_verbose = TRUE;
      }
    }

    if ($show_verbose) {

      drupal_set_message($msg . "\n Extensive msg: \n" . $verbosemsg, DRUPAL_MSG_TYPE_STATUS);
    }
    else {

      drupal_set_message($msg, DRUPAL_MSG_TYPE_STATUS);
    }
  }
}

function zabbix_bridge_get_all_hostgroupids_from_drupal($zabbixuserid = null) {
  $hostgroupids = array();

  if (isset($zabbixuserid)) {
    $sqlwhere = 'zabbix_uid = '.$zabbixuserid;
  } else {
    $sqlwhere = 'zabbix_hostgrp_id is not null';
  }
  $sql = "select zabbix_hostgrp_id from {zabbix_drupal_account} where ".$sqlwhere;
  $result = db_query($sql);

  while ($node = db_fetch_object($result)) {
    $hostgroupids[] = $node->zabbix_hostgrp_id;
  }

  return $hostgroupids;
}

function zabbix_bridge_get_all_templateids_from_zabbix($extended = FALSE) {

  $zabbixtemplateids = array();
  $templateids = array();

  // This logs into Zabbix, and returns FALSE if it fails
  zabbix_api_login()
          or drupal_set_message('Unable to login: ' . print_r(ZabbixAPI::getLastError(), TRUE), DRUPAL_MSG_TYPE_ERR);

  if ($extended) {
    $zabbixtemplateids = ZabbixAPI::fetch_array('template', 'get', array('output' => array('host')));
  }
  else {
    $zabbixtemplateids = ZabbixAPI::fetch_array('template', 'get');
  }

  foreach ($zabbixtemplateids as $key => $value) {
    if ($extended) {
      $templateids[] = array('templateid' => $key, 'name' => $value['host']);
    }
    else {
      $templateids[] = $key;
    }
  }

  return $templateids;
}

/**
 *
 * @return array
 */
function zabbix_bridge_get_all_hosts($withtemplates = TRUE) {
  $hosts = array();
  $zabbixresult = array();

  // This logs into Zabbix, and returns FALSE if it fails
  zabbix_api_login()
          or drupal_set_message('Unable to login: ' . print_r(ZabbixAPI::getLastError(), TRUE), DRUPAL_MSG_TYPE_ERR);

  $hostgroupids = array();
  $hostgroupids = zabbix_bridge_get_all_hostgroupids_from_drupal();

  // This is a workaround. Zabbxi API call host.get will nto return the list
  // of templates for a host unless you ask for all hosts that are in a list
  // of templates.
  // Therefore, we get a list of all templateids, and ask for all hosts with
  // those templates. Note that the real selection criteria is the hostgroups
  // we have just retrieved on the drupal side.

  $alltemplateids = array();
  $alltemplateids = zabbix_bridge_get_all_templateids_from_zabbix();

  $zabbixresult = ZabbixAPI::fetch_array('host', 'get', array(
              'output' => array('hostid', 'host', 'status'),
              'groupids' => $hostgroupids,
              'templateids' => $alltemplateids));




  foreach ($zabbixresult as $value) {

    if ($withtemplates) {
      $templateids = array();
      if ($value['templates']) {
        foreach ($value['templates'] as $template) {
          $templateids[] = $template['templateid'];
        }

        $hosts[] = array('hostid' => $value['hostid'],
            'name' => $value['host'],
            'enabled' => $value['status'],
            'hostgroupid' => $value['groups'][0]['groupid'],
            'templateids' => $templateids);
      }
    }
    else {
      $hosts[] = $value['hostid'];
    }
  }

  return $hosts;
}

function zabbix_bridge_get_userid_by_hostgroupid($hostgroupid) {
  $sql = "select drupal_uid from {zabbix_drupal_account} where zabbix_hostgrp_id = '%s'";
  $result = db_query($sql, $hostgroupid);

  $user = db_fetch_object($result);

  return $user->drupal_uid;
}

/**
 *
 * @param int $zabbixuserid
 * @param int $zabbixhostid
 * @param string $zabbixhostname
 * @param int $zabbixhostenabled
 * @param array $zabbixhosttemplateids
 */
function zabbix_bridge_add_host_to_zabbix_userid(
$zabbixuserid, $zabbixhostid, $zabbixhostname, $zabbixhostenabled, $zabbixhosttemplateids) {

  $sql = "insert into {zabbix_hosts} (userid, zabbixhostid, hostname, enabled) values ('%s', '%s', '%s', '%s')";
  $result = db_query($sql, $zabbixuserid, $zabbixhostid, $zabbixhostname, $zabbixhostenabled);
  if ($result)
    $hostid = db_last_insert_id('zabbix_hosts', 'hostid');

  foreach ($zabbixhosttemplateids as $value) {

    $sql = "select roleid from {zabbix_roles_templates} where templateid = '%s'";
    $result = db_query($sql, $value);
    $role = db_fetch_object($result);

    $sql = "insert into {zabbix_hosts_roles} (hostid, roleid) values ('%s', '%s')";
    $result = db_query($sql, $hostid, $role->roleid);
  }
}

/**
 *
 * @param <type> $emailid
 * @return <type>
 */
function zabbix_bridge_drupal_to_zabbix_emailid($emailid) {

  $sql = "select zabbixmediaid from {zabbix_emails} where emailid = '%s'";
  $result = db_query($sql, $emailid);

  $email = db_fetch_object($result);

  zabbix_bridge_debug(print_r($email, TRUE));

  return $email->zabbixmediaid;
}

/**
 *
 * @global <type> $user
 * @param <type> $userid
 * @return string
 */
function zabbix_emails_table($userid = NULL) {
  global $user;

  $rows = array();
  $count = PAGER_COUNT;
  $id = TABLE_ID_EMAILS_MAPPING;

  if (isset($userid)) {
    $header = array('Email', 'Severity', 'Enabled');
    $results = pager_query("select e.emailid, e.email, e.zabbixmediaid, zs.name severity, e.enabled from {zabbix_emails} e left join {zabbix_emails_severities} zes on e.emailid = zes.emailid left join {zabbix_severities} zs on zes.severityid = zs.severityid where e.userid = '%s'", $count, $id, null, $user->uid);
  }
  else {
    $header = array('Username', 'Email Id', 'Email', 'Zabbix Media Id', 'Severity', 'Enabled');
    $results = pager_query("select u.name, e.emailid, e.email, e.zabbixmediaid, zs.name severity, e.enabled from {zabbix_emails} e left join {zabbix_emails_severities} zes on e.emailid = zes.emailid left join {zabbix_severities} zs on zes.severityid = zs.severityid left join {users} u on u.uid = e.userid", $count, $id);
  }

  $lastzabbixmediaid = -1;

  while ($node = db_fetch_object($results)) {

    if (!isset($userid)) {

      $rows[] = array(
          $node->name,
          $node->emailid,
          $node->email,
          $lastzabbixmediaid == $node->zabbixmediaid ? '' : $node->zabbixmediaid,
          $node->severity,
          $lastzabbixmediaid == $node->zabbixmediaid ? '' : ($node->enabled == 0 ? 'enabled' : 'disabled'),
          $lastzabbixmediaid == $node->zabbixmediaid ? '' : (
                  l($node->enabled == 0 ? 'Disable' : 'Enable', 'emails/enable-disable/' . $node->emailid) . ' | ' .
                  l('Delete', 'emails/delete/' . $node->emailid) . ' | ' .
                  l('Update', 'emails/update/' . $node->emailid)

                  ),
      );
    }
    else {

      $rows[] = array(
          $lastzabbixmediaid == $node->zabbixmediaid ? '' : $node->email,
          $node->severity,
          $lastzabbixmediaid == $node->zabbixmediaid ? '' : ($node->enabled == 0 ? 'enabled' : 'disabled'),
          $lastzabbixmediaid == $node->zabbixmediaid ? '' : (

                  l($node->enabled == 0 ? 'Disable' : 'Enable', 'emails/enable-disable/' . $node->emailid) . ' | ' .
                  l('Delete', 'emails/delete/' . $node->emailid) . ' | ' .
                  l('Update', 'emails/update/' . $node->emailid)

                  ),
      );
    }

    $lastzabbixmediaid = $node->zabbixmediaid;
  }

  if ($lastzabbixmediaid == -1) {

    if (!isset($userid)) {

      $rows[] = array(t('No Emails have been defined yet.'), '', '', '', '', '');
    }
    else {

      $rows[] = array(t('No Emails have been defined yet.'), '', '', '');
    }
  }

  $table_attributes = array('id' => 'emails-table', 'align' => 'center');
  $output = theme('table', $header, $rows, $table_attributes) . theme('pager', $count, $id);

  return $output;
}

/**
 *
 * @param <type> $severities
 * @return int
 */
function zabbix_bridge_calculate_severity($severities) {

  // if the supplied param is not an array, make it one!
  if (!is_array($severities)) {
    $tmp = $severities;
    $severities = array();
    $severities[] = $tmp;
  }

  // Get the severities from the database
  $sql = 'select value from {zabbix_severities} where severityid in (' . db_placeholders($severities, 'int') . ')';
  $result = db_query($sql, $severities);

  // $severity needs to be a flag, add the power of 2 to the (zabbix-id)'th to the desired severity levels
  $severity = 0;
  while ($rs = db_fetch_array($result)) {
    $severity += pow(2, $rs['value']);
  }

  return $severity;
}

function zabbix_bridge_get_all_emails_from_zabbix($extended = false) {

    $zabbixmedia = array();
    $media = array();

    // This logs into Zabbix, and returns false if it fails
    zabbix_api_login()
            or drupal_set_message('Unable to login: ' . print_r(ZabbixAPI::getLastError(), TRUE), DRUPAL_MSG_TYPE_ERR);

    $zabbixmedia = ZabbixAPI::fetch_array('user', 'getMedia', array( 'extendoutput' => $extended, 'mediatypeid' => 1 ));

    foreach ($zabbixmedia as $key => $value) {

        if ($extended) {

            $media[] = $value;

        }
        else {

            $media[] = $key;

        }

    }

    return $media;

}

/**
 *
 * @param <type> $mobileid
 * @return <type>
 */
function zabbix_bridge_drupal_to_zabbix_mobileid($mobileid) {

  $sql = "select zabbixmediaid from {zabbix_mobiles} where mobileid = '%s'";
  $result = db_query($sql, $mobileid);

  $mobile = db_fetch_object($result);

  zabbix_bridge_debug(print_r($mobile, TRUE));

  return $mobile->zabbixmediaid;
}

/**
 *
 * @global <type> $user
 * @param <type> $userid
 * @return string
 */
function zabbix_mobiles_table($userid = NULL) {
  global $user;

  $rows = array();
  $count = PAGER_COUNT;
  $id = TABLE_ID_EMAILS_MAPPING;

  if (isset($userid)) {
    $header = array('Mobile', 'Severity', 'Enabled');
    $results = pager_query("select m.mobileid, m.number, m.zabbixmediaid, zs.name severity, m.enabled from {zabbix_mobiles} m left join {zabbix_mobiles_severities} zms on m.mobileid = zms.mobileid left join {zabbix_severities} zs on zms.severityid = zs.severityid where m.userid = '%s'", $count, $id, null, $user->uid);
  }
  else {
    $header = array('Username', 'Mobile Id', 'Mobile Number', 'Zabbix Media Id', 'Severity', 'Enabled');
    $results = pager_query("select u.name, m.mobileid, m.number, m.zabbixmediaid, zs.name severity, m.enabled from {zabbix_mobiles} m left join {zabbix_mobiles_severities} zms on m.mobileid = zms.mobileid left join {zabbix_severities} zs on zms.severityid = zs.severityid left join {users} u on u.uid = m.userid", $count, $id);
  }

  $lastzabbixmediaid = -1;

  while ($node = db_fetch_object($results)) {

    if (!isset($userid)) {

      $rows[] = array(
          $node->name,
          $node->mobileid,
          $node->number,
          $lastzabbixmediaid == $node->zabbixmediaid ? '' : $node->zabbixmediaid,
          $node->severity,
          $lastzabbixmediaid == $node->zabbixmediaid ? '' : ($node->enabled == 0 ? 'enabled' : 'disabled'),
          $lastzabbixmediaid == $node->zabbixmediaid ? '' : (
                  l($node->enabled == 0 ? 'Disable' : 'Enable', 'mobiles/enable-disable/' . $node->mobileid) . ' | ' .
                  l('Delete', 'mobiles/delete/' . $node->mobileid) . ' | ' .
                  l('Update', 'mobiles/update/' . $node->mobileid)

                  ),
      );
    }
    else {

      $rows[] = array(
          $lastzabbixmediaid == $node->zabbixmediaid ? '' : $node->number,
          $node->severity,
          $lastzabbixmediaid == $node->zabbixmediaid ? '' : ($node->enabled == 0 ? 'enabled' : 'disabled'),
          $lastzabbixmediaid == $node->zabbixmediaid ? '' : (

                  l($node->enabled == 0 ? 'Disable' : 'Enable', 'mobiles/enable-disable/' . $node->mobileid) . ' | ' .
                  l('Delete', 'mobiles/delete/' . $node->mobileid) . ' | ' .
                  l('Update', 'mobiles/update/' . $node->mobileid)

                  ),
      );
    }

    $lastzabbixmediaid = $node->zabbixmediaid;
  }

  if ($lastzabbixmediaid == -1) {

    if (!isset($userid)) {

      $rows[] = array(t('No mobile numbers have been defined yet.'), '', '', '', '', '');
    }
    else {

      $rows[] = array(t('No mobile numbers have been defined yet.'), '', '', '');
    }
  }

  $table_attributes = array('id' => 'mobiles-table', 'align' => 'center');
  $output = theme('table', $header, $rows, $table_attributes) . theme('pager', $count, $id);

  return $output;
}

function zabbix_bridge_get_all_mobiles_from_zabbix($extended = false) {

    $zabbixmedia = array();
    $media = array();

    // This logs into Zabbix, and returns false if it fails
    zabbix_api_login()
            or drupal_set_message('Unable to login: ' . print_r(ZabbixAPI::getLastError(), TRUE), DRUPAL_MSG_TYPE_ERR);

    $zabbixmedia = ZabbixAPI::fetch_array('user', 'getMedia', array( 'extendoutput' => $extended, 'mediatypeid' => 3 ));

    foreach ($zabbixmedia as $key => $value) {

        if ($extended) {

            $media[] = $value;

        }
        else {

            $media[] = $key;

        }

    }

    return $media;

}

/**
 *
 * @param <type> $jabberid
 * @return <type>
 */
function zabbix_bridge_drupal_to_zabbix_jabberid($jabberid) {

    $sql = "select zabbixmediaid from {zabbix_jabbers} where jabberid = '%s'";
    $result = db_query($sql, $jabberid);

    $jabber = db_fetch_object($result);

    zabbix_bridge_debug(print_r($jabber, TRUE));

    return $jabber->zabbixmediaid;
}

/**
 *
 * @global <type> $user
 * @param <type> $userid
 * @return string
 */
function zabbix_jabbers_table($userid = NULL) {
    global $user;

    $rows = array();
    $count = PAGER_COUNT;
    $id = TABLE_ID_EMAILS_MAPPING;

    if (isset($userid)) {
        $header = array('Jabber', 'Severity', 'Enabled');
        $results = pager_query("select j.jabberid, j.jabber, j.zabbixmediaid, zs.name severity, j.enabled from {zabbix_jabbers} j left join {zabbix_jabbers_severities} zjs on j.jabberid = zjs.jabberid left join {zabbix_severities} zs on zjs.severityid = zs.severityid where j.userid = '%s'", $count, $id, null, $user->uid);
    }
    else {
        $header = array('Username', 'Jabber Id', 'Jabber', 'Zabbix Media Id', 'Severity', 'Enabled');
        $results = pager_query("select u.name, j.jabberid, j.jabber, j.zabbixmediaid, zs.name severity, j.enabled from {zabbix_jabbers} j left join {zabbix_jabbers_severities} zjs on j.jabberid = zjs.jabberid left join {zabbix_severities} zs on zjs.severityid = zs.severityid left join {users} u on u.uid = j.userid", $count, $id);
    }

    $lastzabbixmediaid = -1;

    while ($node = db_fetch_object($results)) {

        if (!isset($userid)) {

            $rows[] = array(
                    $node->name,
                    $node->jabberid,
                    $node->jabber,
                    $lastzabbixmediaid == $node->zabbixmediaid ? '' : $node->zabbixmediaid,
                    $node->severity,
                    $lastzabbixmediaid == $node->zabbixmediaid ? '' : ($node->enabled == 0 ? 'enabled' : 'disabled'),
                    $lastzabbixmediaid == $node->zabbixmediaid ? '' : (
                        l($node->enabled == 0 ? 'Disable' : 'Enable', 'jabbers/enable-disable/' . $node->jabberid) . ' | ' .
                        l('Delete', 'jabbers/delete/' . $node->jabberid) . ' | ' .
                        l('Update', 'jabbers/update/' . $node->jabberid)

                        ),
                    );
        }
        else {

            $rows[] = array(
                    $lastzabbixmediaid == $node->zabbixmediaid ? '' : $node->jabber,
                    $node->severity,
                    $lastzabbixmediaid == $node->zabbixmediaid ? '' : ($node->enabled == 0 ? 'enabled' : 'disabled'),
                    $lastzabbixmediaid == $node->zabbixmediaid ? '' : (

                        l($node->enabled == 0 ? 'Disable' : 'Enable', 'jabbers/enable-disable/' . $node->jabberid) . ' | ' .
                        l('Delete', 'jabbers/delete/' . $node->jabberid) . ' | ' .
                        l('Update', 'jabbers/update/' . $node->jabberid)

                        ),
                    );
        }

        $lastzabbixmediaid = $node->zabbixmediaid;
    }

    if ($lastzabbixmediaid == -1) {

        if (!isset($userid)) {

            $rows[] = array(t('No jabber ids have been defined yet.'), '', '', '', '', '');
        }
        else {

            $rows[] = array(t('No jabber ids have been added. Click "Add Jabber id" below to get started!'), '', '', '');
        }
    }

    $table_attributes = array('id' => 'jabbers-table', 'align' => 'center');
    $output = theme('table', $header, $rows, $table_attributes) . theme('pager', $count, $id);

    return $output;
}

function zabbix_bridge_get_all_jabbers_from_zabbix($extended = false) {

    $zabbixmedia = array();
    $media = array();

    // This logs into Zabbix, and returns false if it fails
    zabbix_api_login()
        or drupal_set_message('Unable to login: ' . print_r(ZabbixAPI::getLastError(), TRUE), DRUPAL_MSG_TYPE_ERR);

    $zabbixmedia = ZabbixAPI::fetch_array('user', 'getMedia', array( 'extendoutput' => $extended, 'mediatypeid' => 2 ));

    foreach ($zabbixmedia as $key => $value) {

        if ($extended) {

            $media[] = $value;

        }
        else {

            $media[] = $key;

        }

    }

    return $media;

}


function zabbix_bridge_get_all_users_from_zabbix() {

  // This logs into Zabbix, and returns false if it fails
  zabbix_api_login()
      or drupal_set_message('Unable to login: ' . print_r(ZabbixAPI::getLastError(), TRUE), DRUPAL_MSG_TYPE_ERR);

  $userids = array();
  $options = array('select_usrgrps' => TRUE, 'extendoutput' => TRUE);
  
  $userids = ZabbixAPI::fetch_array('user', 'get', $options)
          or drupal_set_message('Unable to get zabbix users: ' . serialize(ZabbixAPI::getLastError(), TRUE), DRUPAL_MSG_TYPE_ERR);

  return $userids;

}

/**
 *
 * @param integer $hostid the zabbix hostid for the host we want the triggers for
 * @return object the object with all triggers for this host
 */
function zabbix_bridge_get_all_triggers_from_zabbix($hostid) {

  // This logs into Zabbix, and returns false if it fails
  zabbix_api_login()
      or drupal_set_message('Unable to login: ' . print_r(ZabbixAPI::getLastError(), TRUE), DRUPAL_MSG_TYPE_ERR);

  $triggerids = array();
  $options = array();

  $options['hostids'] = array($hostid);
  $options['sortfield'] = 'templateid';
  $options['output'] = API_OUTPUT_EXTEND;
  $options['extendoutput'] = 'true';

  $triggerids = ZabbixAPI::fetch_array('trigger', 'get', $options)
          or drupal_set_message('Unable to get zabbix triggers: ' . serialize(ZabbixAPI::getLastError(), TRUE), DRUPAL_MSG_TYPE_ERR);

  return $triggerids;
}

/**
 * returns an array with all zabbix actions for the specified hostgroup
 *
 * @param $hostgroupid integer The zabbix id of the hostgrop we want all actions for
 * @return array The array of actions
 */
function zabbix_bridge_get_all_actions_from_zabbix($hostgroupid) {

  // This logs into Zabbix, and returns false if it fails
  zabbix_api_login()
      or drupal_set_message('Unable to login: ' . print_r(ZabbixAPI::getLastError(), TRUE), DRUPAL_MSG_TYPE_ERR);

  $actionids = array();
  $options = array();
  $actionids_result = array();

  $options['groupids'] = array($hostgroupid);
  $options['select_conditions'] = 'true';
  $options['select_operations'] = 'true';
  $options['output'] = API_OUTPUT_EXTEND;
  $options['extendoutput'] = 'true';

  $actionids = ZabbixAPI::fetch_array('action', 'get', $options)
          or drupal_set_message('Unable to get zabbix actions: ' . serialize(ZabbixAPI::getLastError(), TRUE), DRUPAL_MSG_TYPE_ERR);

  return $actionids;
  
}

/**
 * Creates an action in zabbix that will alert through text message
 *
 * @param string $actionname            name of the action to create
 * @param integer $zabbixusergroupid    the usergroupid used in zabbix to which this action should alert
 * @param integer $zabbixhostgroupid    the hostgroupid used in zabbix for which this action should be executed
 */
function zabbix_bridge_create_mobile_action($actionname, $zabbixusergroupid, $zabbixhostgroupid) {

  $actionparams = array();

  // still needs testing/completing
  $actionparams[] = array('name' => $actionname,
      'eventsource' => EVENT_SOURCE_TRIGGERS,
      'evaltype' => ACTION_EVAL_TYPE_AND_OR,
      'status' => ACTION_STATUS_ENABLED,
      'esc_period' => 60,
      'def_shortdata' => variable_get('zabbix_bridge_alert_sms_subject', STR_DEFAULT_ACTION_SUBJECT_MOBILE),
      'def_longdata' => variable_get('zabbix_bridge_alert_sms', STR_DEFAULT_ACTION_TEXT_MOBILE),
      'r_shortdata' => variable_get('zabbix_bridge_alert_sms_subject_recovery', STR_DEFAULT_ACTION_SUBJECT_MOBILE),
      'r_longdata' => variable_get('zabbix_bridge_alert_sms_recovery', STR_DEFAULT_ACTION_TEXT_MOBILE),
      'recovery_msg' => 1,
      'conditions' => array(
          array(
              'conditiontype' => CONDITION_TYPE_TRIGGER_SEVERITY,
              'value' => TRIGGER_SEVERITY_HIGH,
              'operator' => CONDITION_OPERATOR_EQUAL,
          ),
          array(
              'conditiontype' => CONDITION_TYPE_TRIGGER_SEVERITY,
              'value' => TRIGGER_SEVERITY_DISASTER,
              'operator' => CONDITION_OPERATOR_EQUAL,
          ),
          array(
              'conditiontype' => CONDITION_TYPE_HOST_GROUP,
              'value' => $zabbixhostgroupid,
              'operator' => CONDITION_OPERATOR_EQUAL,
          ),
          array(
              'conditiontype' => CONDITION_TYPE_MAINTENANCE,
              'operator' => CONDITION_OPERATOR_NOT_IN,
          ),
          array(
              'conditiontype' => CONDITION_TYPE_TRIGGER_VALUE,
              'value' => TRIGGER_VALUE_TRUE,
              'operator' => CONDITION_OPERATOR_EQUAL,
          ),
      ),
      'operations' => array(
          array(
              'object' => OPERATION_OBJECT_GROUP,
              'objectid' => $zabbixusergroupid,
              'operationtype' => OPERATION_TYPE_MESSAGE,
              'default_msg' => 1,
              'esc_period' => 0,
              'esc_step_from' => 1,
              'esc_step_to' => 1,
              'evaltype' => 0,
              'mediatypeid' => ALERT_TYPE_SMS,
          ),
      ),
  );
  $actionids = array();
  $actionids = ZabbixAPI::fetch_array('action', 'create', $actionparams)
          or drupal_set_message('Unable to create zabbix action: ' . serialize(ZabbixAPI::getLastError()), DRUPAL_MSG_TYPE_ERR);

  zabbix_bridge_debug('Zabbix action created: ' . print_r($actionids, TRUE), print_r($actionparams, TRUE));

}

/**
 * Creates an action in zabbix that will alert through jabber
 *
 * @param string $actionname            name of the action to create
 * @param integer $zabbixusergroupid    the usergroupid used in zabbix to which this action should alert
 * @param integer $zabbixhostgroupid    the hostgroupid used in zabbix for which this action should be executed
 */
function zabbix_bridge_create_jabber_action($actionname, $zabbixusergroupid, $zabbixhostgroupid) {

  $actionparams = array();

  // still needs testing/completing
  $actionparams[] = array('name' => $actionname,
      'eventsource' => EVENT_SOURCE_TRIGGERS,
      'evaltype' => ACTION_EVAL_TYPE_AND_OR,
      'status' => ACTION_STATUS_ENABLED,
      'esc_period' => 60,
      'def_shortdata' => variable_get('zabbix_bridge_alert_jabber_subject', STR_DEFAULT_ACTION_SUBJECT_JABBER),
      'def_longdata' => variable_get('zabbix_bridge_alert_jabber', STR_DEFAULT_ACTION_TEXT_JABBER),
      'r_shortdata' => variable_get('zabbix_bridge_alert_jabber_subject_recovery', STR_DEFAULT_ACTION_SUBJECT_JABBER),
      'r_longdata' => variable_get('zabbix_bridge_alert_jabber_recovery', STR_DEFAULT_ACTION_TEXT_JABBER),
      'recovery_msg' => 1,
      'conditions' => array(
          array(
              'conditiontype' => CONDITION_TYPE_HOST_GROUP,
              'value' => $zabbixhostgroupid,
              'operator' => CONDITION_OPERATOR_EQUAL,
          ),
          array(
              'conditiontype' => CONDITION_TYPE_MAINTENANCE,
              'operator' => CONDITION_OPERATOR_NOT_IN,
          ),
          array(
              'conditiontype' => CONDITION_TYPE_TRIGGER_VALUE,
              'value' => TRIGGER_VALUE_TRUE,
              'operator' => CONDITION_OPERATOR_EQUAL,
          ),
      ),
      'operations' => array(
          array(
              'object' => OPERATION_OBJECT_GROUP,
              'objectid' => $zabbixusergroupid,
              'operationtype' => OPERATION_TYPE_MESSAGE,
              'default_msg' => 1,
              'esc_period' => 0,
              'esc_step_from' => 1,
              'esc_step_to' => 1,
              'evaltype' => 0,
              'mediatypeid' => ALERT_TYPE_JABBER,
          ),
      ),
  );
  $actionids = array();
  $actionids = ZabbixAPI::fetch_array('action', 'create', $actionparams)
          or drupal_set_message('Unable to create zabbix action: ' . serialize(ZabbixAPI::getLastError()), DRUPAL_MSG_TYPE_ERR);

  zabbix_bridge_debug('Zabbix action created: ' . print_r($actionids, TRUE), print_r($actionparams, TRUE));

}


/**
 * Creates the zabbix actions to respond to triggers and send email alerts
 *
 * @param string $actionname
 * @param int $severity
 * @param int $zabbixusergroupid
 * @param int $zabbixhostgroupid
 */
function zabbix_bridge_create_basic_actions($actionname, $severity, $zabbixusergroupid, $zabbixhostgroupid) {

  // only for severity level warning the operator needs that and lower, the other seveirties need equal
  switch ($severity) {
    case TRIGGER_SEVERITY_WARNING:
      $conditionoperator  = CONDITION_OPERATOR_LESS_EQUAL;
      $escalationperiod   = 7200;
      $subject            = variable_get('zabbix_bridge_alert_info_warn_subject', STR_DEFAULT_ACTION_SUBJECT);
      $text               = variable_get('zabbix_bridge_alert_info_warn', STR_DEFAULT_ACTION_TEXT);
      $recovery_subject   = variable_get('zabbix_bridge_alert_info_warn_subject_recovery', STR_DEFAULT_ACTION_SUBJECT);
      $recovery_text      = variable_get('zabbix_bridge_alert_info_warn_recovery', STR_DEFAULT_ACTION_TEXT);
      break;
    case TRIGGER_SEVERITY_AVERAGE:
      $conditionoperator = CONDITION_OPERATOR_EQUAL;
      $escalationperiod = 1800;
      $subject            = variable_get('zabbix_bridge_alert_average_subject', STR_DEFAULT_ACTION_SUBJECT);
      $text               = variable_get('zabbix_bridge_alert_average', STR_DEFAULT_ACTION_TEXT);
      $recovery_subject   = variable_get('zabbix_bridge_alert_average_subject_recovery', STR_DEFAULT_ACTION_SUBJECT);
      $recovery_text      = variable_get('zabbix_bridge_alert_average_recovery', STR_DEFAULT_ACTION_TEXT);
      break;
    case TRIGGER_SEVERITY_HIGH:
      $conditionoperator = CONDITION_OPERATOR_EQUAL;
      $escalationperiod = 600;
      $subject            = variable_get('zabbix_bridge_alert_high_subject', STR_DEFAULT_ACTION_SUBJECT);
      $text               = variable_get('zabbix_bridge_alert_high', STR_DEFAULT_ACTION_TEXT);
      $recovery_subject   = variable_get('zabbix_bridge_alert_high_subject_recovery', STR_DEFAULT_ACTION_SUBJECT);
      $recovery_text      = variable_get('zabbix_bridge_alert_high_recovery', STR_DEFAULT_ACTION_TEXT);
      break;
    case TRIGGER_SEVERITY_DISASTER:
      $conditionoperator = CONDITION_OPERATOR_EQUAL;
      $escalationperiod = 60;
      $subject            = variable_get('zabbix_bridge_alert_disaster_subject', STR_DEFAULT_ACTION_SUBJECT);
      $text               = variable_get('zabbix_bridge_alert_disaster', STR_DEFAULT_ACTION_TEXT);
      $recovery_subject   = variable_get('zabbix_bridge_alert_disaster_subject_recovery', STR_DEFAULT_ACTION_SUBJECT);
      $recovery_text      = variable_get('zabbix_bridge_alert_disaster_recovery', STR_DEFAULT_ACTION_TEXT);
      break;
  }




  // create the action
  $actionparams = array();

  // still needs testing/completing
  $actionparams[] = array('name' => $actionname,
      'eventsource' => EVENT_SOURCE_TRIGGERS,
      'evaltype' => ACTION_EVAL_TYPE_AND_OR,
      'status' => ACTION_STATUS_ENABLED,
      'esc_period' => $escalationperiod,
      'def_shortdata' => $subject,
      'def_longdata' => $text,
      'r_shortdata' => $recovery_subject,
      'r_longdata' => $recovery_text,
      'recovery_msg' => 1,
      'conditions' => array(
          array(
              'conditiontype' => CONDITION_TYPE_TRIGGER_SEVERITY,
              'value' => $severity,
              'operator' => $conditionoperator,
          ),
          array(
              'conditiontype' => CONDITION_TYPE_HOST_GROUP,
              'value' => $zabbixhostgroupid,
              'operator' => CONDITION_OPERATOR_EQUAL,
          ),
          array(
              'conditiontype' => CONDITION_TYPE_MAINTENANCE,
              'operator' => CONDITION_OPERATOR_NOT_IN,
          ),
          array(
              'conditiontype' => CONDITION_TYPE_TRIGGER_VALUE,
              'value' => TRIGGER_VALUE_TRUE,
              'operator' => CONDITION_OPERATOR_EQUAL,
          ),
      ),
      'operations' => array(
          array(
              'object' => OPERATION_OBJECT_GROUP,
              'objectid' => $zabbixusergroupid,
              'operationtype' => OPERATION_TYPE_MESSAGE,
              'default_msg' => 1,
              'esc_period' => 0,
              'esc_step_from' => 1,
              'esc_step_to' => 0,
              'evaltype' => 0,
              'mediatypeid' => ALERT_TYPE_EMAIL,
//Note: 1 Means email, yet it is NOT MEDIA_TYPE_EMAIL. This is because there are
//      several media_type records with type MEDIA_TYPE_EMAIL. This is done so
//      you can have multiple email gateways for instance. Naming is very confusing!
//
//
//                'opconditions' => array(
//                                            'conditiontype' => CONDITION_TYPE_EVENT_ACKNOWLEDGED,
//                                            'operator' => CONDITION_OPERATOR_EQUAL,
//                                            'value' => EVENT_ACK_DISABLED,
//                ),
          ),
      ),
  );
  $actionids = array();
  $actionids = ZabbixAPI::fetch_array('action', 'create', $actionparams)
          or drupal_set_message('Unable to create zabbix action: ' . serialize(ZabbixAPI::getLastError()), DRUPAL_MSG_TYPE_ERR);

  zabbix_bridge_debug('Zabbix action created: ' . print_r($actionids, TRUE), print_r($actionparams, TRUE));
}
