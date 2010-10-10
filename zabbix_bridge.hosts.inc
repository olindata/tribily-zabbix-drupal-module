<?php
// $Id: zabbix_bridge.hosts.inc,v 1.7 2010/09/19 11:45:09 rmiddle Exp $

/**
 * @file
 * Settings form.
 *
 * @author raj
 */

include_once 'zabbix_functions.php';

/**
 * Form builder; Present the form to edit a given user's hosts.
 *
 * @ingroup forms
 */
function zabbix_hosts($operation, $hostid) {

    drupal_set_title('Hosts to monitor');

    //drupal_set_message (print_r(arg(),true));

    if (!isset($operation))
        $operation = 'add';

    $output = zabbix_hosts_table();

    $output .= drupal_get_form('zabbix_hosts_form', $account, $hostid, $operation);

    return $output;

}

function host_user_load($hostid) {
	$query = "SELECT * FROM {zabbix_hosts} WHERE hostid = %d";
	$rs = db_query($query, $hostid);

	if ($rs) {
		while ($data = db_fetch_object($rs)) {
			return $data;
		}
	}

	return FALSE;
}

function host_delete_confirm(&$form_state, $host) {
    $form['_host'] = array(
            '#type' => 'value',
            '#value' => $host,
    );

    return confirm_form(
        $form,
        t('Are you sure you want to delete this host?'),
        isset($_GET['destination']) ? $_GET['destination'] : "hosts",
        t('This action cannot be undone.'),
        t('Delete'),
        t('Cancel')
    );
}

function host_delete_confirm_submit($form, &$form_state) {
	$form_values = $form_state['values'];

	if ($form_state['values']['confirm']) {
		$host = $form_state['values']['_host'];

		if (zabbix_host_delete($form_state['values'], $host)) {
                    drupal_set_message(t('Host has been deleted successfully.'));
                } else {
                    drupal_set_message(t('Host deletion unsuccessful'), DRUPAL_MSG_TYPE_ERR);
                }

  	}

	drupal_goto("hosts");
}

function zabbix_host_delete($formvalues, $hostid) {
    
    $sql = "delete from {zabbix_hosts_roles} where hostid = %s";
    
    $result = db_result(db_query($sql, $hostid));

    var_dump($result);

    $sql = "delete from {zabbix_hosts} where hostid = %s";

    $result = db_result(db_query($sql, $hostid));

    var_dump($result);
    
    return db_result($result);


}

/**
 * Form builder; add/edit a user's hosts.
 * 
 * @ingroup forms
 * @see zabbix_hosts_form_validate()
 * @see zabbix_hosts_form_submit()
 */
function zabbix_hosts_form($form_state, $account, $hostid, $operation) {
    
    
    if ('add' == $operation) {
        $title = 'Add Host';
        $submit = 'Add';
    } else {
        $title = 'Update host';
        $submit = 'Update';
    }

    $form['hosts'] = array('#type' => 'fieldset',
        '#title' => t($title),
        '#weight' => -10
    );

    // the host's name
    $form['hosts']['name'] = array('#type' => 'textfield',
        '#title' => t('Name'),
        '#maxlength' => HOST_NAME_MAXLENGTH,
        '#required' => TRUE
    );

    // the server address, either IP or DNS based on connection type
    $form['hosts']['server'] = array('#type' => 'textfield',
        '#title' => t('Server Address'),
        '#maxlength' => HOST_SERVER_MAXLENGTH,
        '#required' => TRUE
    );

    // connection type - IP/DNS
    $form['hosts']['connect'] = array('#type' => 'select',
        '#title' => t('Connect To'),
        '#default_value' => variable_get('hosts_connect', 'dns'),
        '#options' => array(
            'ip' => t('IP Address'),
            'dns' => t('DNS')
        ),
        '#description' => t('Set the connection method'),
        '#required' => TRUE
    );

    // status - monitored/unmonitored
    $form['hosts']['status'] = array('#type' => 'select',
        '#title' => t('Status'),
        '#default_value' => variable_get('hosts_status', 'monitored'),
        '#options' => array(
            'monitored' => t('Monitored'),
            'unmonitored' => t('Unmonitored')
        ),
        '#description' => t('Specify monitoring status'),
        '#required' => TRUE
    );

    $result = db_query('SELECT roleid, name FROM {zabbix_role}')
            or drupal_set_message('Unable to retrieve zabbix roles', DRUPAL_MSG_TYPE_ERR);

    // make the template SELECT options
    $template_options = array();

    while($data = db_fetch_array($result)) {

        $template_options[$data['roleid']] = t($data['name']);

    }

    $form['hosts']['roles'] = array(
        '#type' => 'checkboxes',
        '#title' => t('Roles'),
        '#default_value' => variable_get('hosts_roles', array()),
        '#options' => $template_options,
        '#description' => t('Assign this host to one or more roles.')
    );

    $form['hosts']['submit'] = array('#type' => 'submit', '#value' => t($submit), '#weight' => 30);

    return $form;

}

/**
 * Validation function for the user host add/editing form.
 */
function zabbix_hosts_form_validate($form, &$form_state) {

    // regexes retrieved from http://stackoverflow.com/questions/106179/regular-expression-to-match-hostname-or-ip-address
    $reg['ip'] = "/^(([0-9]|[1-9][0-9]|1[0-9]{2}|2[0-4][0-9]|25[0-5])\.){3}([0-9]|[1-9][0-9]|1[0-9]{2}|2[0-4][0-9]|25[0-5])$/";
    $reg['dns'] = "/^(([a-zA-Z0-9]|[a-zA-Z0-9][a-zA-Z0-9\-]*[a-zA-Z0-9])\.)*([A-Za-z]|[A-Za-z][A-Za-z0-9\-]*[A-Za-z0-9])$/";

    $server = $form_state['values']['server'];
    $connect = $form_state['values']['connect'];

    // messages based on connection type
    $msg['ip'] = 'You must specify a valid IP address as the Server Address.';
    $msg['dns'] = 'You must specify a valid DNS as the Server Address.';

    // if the server address doesn't match the connect type, notify the user
    if (!preg_match($reg[$connect], $server)) {
        
        form_set_error('hosts', t($msg[$connect]));

    }

}

/**
 * Submit function for the user host add/editing form.
 */
function zabbix_hosts_form_submit($form, &$form_state) {

    // This logs into Zabbix, and returns false if it fails
    zabbix_api_login()
        or drupal_set_message('Unable to login: '.print_r(ZabbixAPI::getLastError(),true), DRUPAL_MSG_TYPE_ERR);

    //zabbix_bridge_debug($form);
    $account = user_load(array('uid' => arg(1)));
    
    zabbix_bridge_debug($form_state);

    $result = db_query('SELECT * FROM {zabbix_drupal_account} WHERE drupal_uid=%s', $account->uid)
            or drupal_set_message('Unable to retrieve drupal-zabbix associations', DRUPAL_MSG_TYPE_ERR);

    $data = db_fetch_array($result);

    if (!$data)
        drupal_set_message('No drupal-zabbix association exists for this user!', DRUPAL_MSG_TYPE_ERR);

    $connect = $form_state['values']['connect'];
    $server = $form_state['values']['server'];
    $status = $form_state['values']['status'];

    $host = array();
    $host['host'] = $form_state['values']['name'];
    $host['groups'] = array($data['zabbix_hostgrp_id']);
    $host['useipmi'] = 0;

    // 0 - status monitored, 1 - status not monitored
    $host['status'] = ('monitored' == $status) ? 0 : 1;

    if ('ip' == $connect) {
        $host['useip'] = 1;
        $host['ip'] = $server;
    }
    else {
        $host['dns'] = $server;
    }

    zabbix_bridge_debug($host);

    $roles = $form_state['values']['roles'];
    $role_ids = array();

    // get the role ids
    foreach($roles as $key=>$value) {
        if (0 === $value) {
            continue;
        }
        $role_ids[] = $key;
    }

    // retrieve zabbix template ids based on role ids
    $result = db_query('SELECT * FROM {zabbix_roles_templates} WHERE roleid IN (%s)', implode(',', $role_ids));

    if (!$result) {
        $message = 'Unable to retrieve role-template associations';
    }
    else {
        $template_ids = array();
        while ($data = db_fetch_array($result)) {
            $template_ids[] = $data['templateid'];
        }

        $created_host_id = ZabbixAPI::fetch_string('host', 'create', array( $host ));

        if (!$created_host_id) {
            $message = 'Unable to add host. Please contact us at info@tribily.com.';
        }
        else {
            $message = 'Host successfully added.';

            zabbix_bridge_debug($created_host_id);

            $template_add = ZabbixAPI::fetch_string('template', 'massAdd', array( 'templates' => $template_ids,
                                                                                'hosts' => array( 'hostid' => $created_host_id ) ));

            zabbix_bridge_debug($template_add);

            if (!$template_add) {
                $message = 'Unable to link templates: '.print_r(ZabbixAPI::getLastError(), true);
            }
            else {
                $message = 'Host successfully added.';
            }
        }
    }

    // add the host mapping to the drupal database so we can use it for display without contating zabbix
    $sql = 'INSERT INTO {zabbix_hosts} (userid, hostid, zabbixhostid, ipaddress, dns, hostname, enabled)';
    $sql .= " VALUES('%s', '%s', '%s', '%s', '%s', '%s', '%s')";

    $result = db_query($sql, $account->uid, NULL, $created_host_id, $host['ip'], $host['dns'], $host['host'], $host['status']);

    if (!$result) {
        $message = 'Problem adding hostmapping to database';
    } else {
        $hostid = db_last_insert_id('zabbix_hosts', 'hostid');
    }

    // addd the host-role mapping to the drupal db so we can use it for display without contacting zabbix
    $sql = 'INSERT INTO {zabbix_hosts_roles} (hostid, roleid)';
    $sql .= " VALUES('%s', '%s')";

    foreach($role_ids as $roleid) {

        $result = db_query($sql, $hostid, $roleid);
        if (!$result) {
            $message = 'Problem adding host-role-mapping to database';
        }

    }



    cache_clear_all();

    drupal_set_message(t($message));

    return;

}

?>
