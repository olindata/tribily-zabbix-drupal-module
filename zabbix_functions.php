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

define('MEDIA_TYPE_EMAIL', 0);
define('MEDIA_TYPE_EXEC', 1);
define('MEDIA_TYPE_SMS', 2);
define('MEDIA_TYPE_JABBER', 3);

define('TRIGGER_VALUE_FALSE', 0);
define('TRIGGER_VALUE_TRUE', 1);
define('TRIGGER_VALUE_UNKNOWN', 2);

/**
 *
 * @param <type> $hostid
 * @return <type>
 */
function zabbix_bridge_drupal_to_zabbix_hostid($hostid) {

    $sql = 'select zabbixhostid from {zabbix_hosts} where hostid = %s';
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
        $results = pager_query("select h.hostid, h.hostname, h.zabbixhostid, h.enabled, r.name as role_name, r.description as role_desc from {zabbix_hosts} h left join {zabbix_hosts_roles} hr on hr.hostid = h.hostid left join {zabbix_role} r on r.roleid = hr.roleid where h.userid = %s", $count, $id, null, $user->uid);
    } else {
        $header = array('Username', 'Email', 'Zabbix Hostid', 'Hostname', 'Enabled', 'Role name', 'Role Description', 'Actions');
        $results = pager_query("select u.name, u.mail, h.hostid, h.zabbixhostid, h.hostname, h.enabled, r.name as role_name, r.description as role_desc from {zabbix_hosts} h left join {zabbix_hosts_roles} hr on hr.hostid = h.hostid left join {zabbix_role} r on r.roleid = hr.roleid left join {users} u on u.uid = h.userid", $count, $id);
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
                    l('Update', 'hosts/update/' . $node->hostid)
                ),
            );
        } else {

            $rows[] = array(
                $lastzabbixhostid == $node->zabbixhostid ? '' : $node->hostname,
                $lastzabbixhostid == $node->zabbixhostid ? '' : ($node->enabled == 0 ? 'enabled' : 'disabled'),
                $node->role_name,
                $node->role_desc,
                $lastzabbixhostid == $node->zabbixhostid ? '' : (
                    l($node->enabled == 0 ? 'Disable' : 'Enable', 'hosts/enable-disable/' . $node->hostid) . ' | ' .
                    l('Delete', 'hosts/delete/' . $node->hostid) . ' | ' .
                    l('Update', 'hosts/update/' . $node->hostid)
                ),
            );
        }
        $lastzabbixhostid = $node->zabbixhostid;
    }

    if ($lastzabbixhostid == -1) {
        if (!isset($userid)) {
            $rows[] = array(t('No Hosts have been defined yet.'), '', '', '', '', '', '', '',);
        } else {
            $rows[] = array(t('No Hosts have been defined yet.'), '', '', '', '');
        }

    }

    $table_attributes = array('id' => 'hosts-table', 'align' => 'center');
    $output = theme('table', $header, $rows, $table_attributes) . theme('pager', $count, $id);

    return $output;
}

/**
 * 
 * @return string
 */
function zabbix_roles_table () {

    $header = array('Roleid', 'Name', 'Description', 'Zabbix Templateid', 'Actions');
    $rows = array();
    $count = PAGER_COUNT;
    $id = TABLE_ID_USER_MAPPING;

    $results = pager_query("select zr.*, zrt.templateid from zabbix_role zr left join zabbix_roles_templates zrt on zrt.roleid = zr.roleid", $count, $id);


    while ($node = db_fetch_object($results)) {
        $rows[] = array(
            $node->roleid,
            $node->name,
            $node->description,
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

    $results = pager_query("select zda.id, du.uid, du.name, du.mail, zda.zabbix_uid, zda.zabbix_usrgrp_id, zda.zabbix_hostgrp_id                         from                            zabbix_drupal_account zda                            left join users du on du.uid = zda.drupal_uid", $count, $id);


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
        } else {

            drupal_set_message($msg, DRUPAL_MSG_TYPE_STATUS);
        }
    }
}

function zabbix_bridge_get_all_hostgroupids_from_drupal() {
    $hostgroupids = array();

    $sql = "select zabbix_hostgrp_id from {zabbix_drupal_account} where zabbix_hostgrp_id is not null";
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
    } else {
        $zabbixtemplateids = ZabbixAPI::fetch_array('template', 'get');
    }

    foreach ($zabbixtemplateids as $key => $value) {
        if ($extended) {
            $templateids[] = array('templateid' => $key, 'name' => $value['host']);
        } else {
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
            foreach ($value['templates'] as $template) {
                $templateids[] = $template['templateid'];
            }

            $hosts[] = array('hostid' => $value['hostid'],
                             'name' => $value['host'],
                             'enabled' => $value['status'],
                             'hostgroupid' => $value['groups'][0]['groupid'],
                             'templateids' => $templateids);

        } else {
            $hosts[] = $value['hostid'];
        }

    }

    return $hosts;
}

function zabbix_bridge_get_userid_by_hostgroupid($hostgroupid){
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
        $zabbixuserid,
        $zabbixhostid,
        $zabbixhostname,
        $zabbixhostenabled,
        $zabbixhosttemplateids) {

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

    $sql = 'select zabbixmediaid from {zabbix_emails} where emailid = %s';
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
        $results = pager_query("select e.emailid, e.email, e.zabbixmediaid, zs.name severity, e.enabled from {zabbix_emails} e left join {zabbix_emails_severities} zes on e.emailid = zes.emailid left join {zabbix_severities} zs on zes.severityid = zs.severityid where e.userid = %s", $count, $id, null, $user->uid);
    } else {
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

    if (!is_array($severities)) {

        $tmp = $severities;
        $severities = array();

        $severities[] = $tmp;

    }

    $sql = 'select value from {zabbix_severities} where severityid in (' . db_placeholders($severities, 'int') . ')';
    $result = db_query($sql, $severities);

    $severity = 0;

    while ($rs = db_fetch_array($result)) {

        $severity += pow(2, $rs['value']);

    }

    return $severity;

}

?>
