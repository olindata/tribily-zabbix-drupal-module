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
    global $user;

    drupal_set_title('Hosts to monitor');

    if ($operation == '')
        $operation = 'add';

    $output = zabbix_hosts_table($user->uid);

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

function host_enable_disable_confirm(&$form_state, $host) {
    $form['_host'] = array(
            '#type' => 'value',
            '#value' => $host,
    );

    $sql = 'select enabled from {zabbix_hosts} where hostid = %s';
    $enabled = db_result(db_query($sql, $host));

    $form['_current_state'] = array(
            '#type' => 'value',
            '#value' => $enabled,
    );

    return confirm_form(
        $form,
        t('Are you sure you want to change the enabled state of this host?'),
        isset($_GET['destination']) ? $_GET['destination'] : "hosts",
        $enabled ? t('This host will be monitored until you disable it.'):
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

		if (zabbix_host_enable_disable($form_state['values'], $zabbixhostid, $enabled)) {
                    drupal_set_message(t('Host has been updated successfully.'));
                } else {
                    drupal_set_message(t('Host update unsuccessful'), DRUPAL_MSG_TYPE_ERR);
                }

  	}

	drupal_goto("hosts");
}

/**
 *
 * @param array $form
 * @param array $form_state
 */
function host_delete_confirm_submit($form, &$form_state) {
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

function zabbix_host_enable_disable($formvalues, $hostid, $enabled) {

    zabbix_api_login()
        or drupal_set_message('Unable to login. '.print_r(ZabbixAPI::getLastError(),true), DRUPAL_MSG_TYPE_ERR);

    $result = ZabbixAPI::fetch_string('host', 'update', array('hostid' => $hostid, 'status' => $enabled ? 0 : 1));

    if (!$result) {
        drupal_set_message('Unable to enable/disable host. '.print_r(ZabbixAPI::getLastError(),true), DRUPAL_MSG_TYPE_ERR);
    } else {
        $sql = "update {zabbix_hosts} set enabled = '%s' where zabbixhostid = %s";
        $result = db_query($sql, $enabled ? 0: 1, $hostid);
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
function zabbix_host_delete($formvalues, $hostid) {

    $sql = "select zabbixhostid from {zabbix_hosts} where hostid = %s";
    $zabbixhostid = db_result(db_query($sql, $hostid));

    zabbix_api_login()
        or drupal_set_message('Unable to login. '.print_r(ZabbixAPI::getLastError(),true), DRUPAL_MSG_TYPE_ERR);

    ZabbixAPI::fetch_string('host', 'delete', array($zabbixhostid))
        or drupal_set_message('Unable to delete host. '.print_r(ZabbixAPI::getLastError(),true), DRUPAL_MSG_TYPE_ERR);

    $sql = "delete from {zabbix_hosts_roles} where hostid = %s";
    $result = db_query($sql, $hostid);

    $sql = "delete from {zabbix_hosts} where hostid = %s";
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
 * Submit function for the user host add/editing form.
 */
function zabbix_hosts_form_submit($form, &$form_state) {

    // This logs into Zabbix, and returns false if it fails

    global $user;

    // use global $user for now, arg(1) doesn't seem to return uid here
    $account = user_load(array('uid' => $user->uid));
    
    zabbix_bridge_debug(print_r($form_state, true));
    zabbix_bridge_debug(print_r($account, true));

    $result = db_query('SELECT * FROM {zabbix_drupal_account} WHERE drupal_uid=%s', $account->uid)
            or drupal_set_message('Unable to retrieve drupal-zabbix associations', DRUPAL_MSG_TYPE_ERR);

    $data = db_fetch_array($result);

    if (!$data)
        drupal_set_message('No drupal-zabbix association exists for this user!', DRUPAL_MSG_TYPE_ERR);

    $server = $form_state['values']['server'];
    $status = $form_state['values']['status'];

    $host = array();
    $host['host'] = $form_state['values']['name'];
    $host['groups'] = array($data['zabbix_hostgrp_id']);
    $host['useipmi'] = 0;

    // 0 - status monitored, 1 - status not monitored
    $host['status'] = ('monitored' == $status) ? 0 : 1;

    zabbix_bridge_debug(print_r($host, true));

    $roles = $form_state['values']['roles'];
    $role_ids = array();
    $rolesadded = 0;

    // get the role ids
    foreach($roles as $key=>$value) {
        if (0 === $value) {
            continue;
        }
        $role_ids[] = $key;
        $rolesadded += 1;
    }

    if ($rolesadded > 0) {
        // retrieve zabbix template ids based on role ids
        $result = db_query('SELECT * FROM {zabbix_roles_templates} WHERE roleid IN (%s)', implode(',', $role_ids));
    } else {
        $result = TRUE;
    }

    if (!$result) {
        $message = 'Unable to retrieve role-template associations';
    }
    else {
        // This logs into Zabbix, and returns false if it fails
        zabbix_api_login()
            or drupal_set_message('Unable to login: '.print_r(ZabbixAPI::getLastError(),true), DRUPAL_MSG_TYPE_ERR);

        $created_host_id = ZabbixAPI::fetch_string('host', 'create', array( $host ));

        if (!$created_host_id) {
            $message = 'Unable to add host. Please contact us at info@tribily.com.';
            zabbix_bridge_debug(print_r(ZabbixAPI::getLastError(), true));
        }
        else {
            $message = 'Host successfully added.';

            zabbix_bridge_debug($created_host_id);

            if ($rolesadded > 0) {
                $template_ids = array();
                while ($data = db_fetch_array($result)) {
                    $template_ids[] = $data['templateid'];
                }
                $template_add = ZabbixAPI::fetch_string('template', 'massAdd', array( 'templates' => $template_ids,
                                                                                'hosts' => array( 'hostid' => $created_host_id ) ));

                zabbix_bridge_debug($template_add);

                if (!$template_add) {
                    $message = 'Unable to link templates: '.print_r(ZabbixAPI::getLastError(), true);
                }
                else {
                    $message = 'Host succesfully added with selected roles.';
                }
            }
        }
    }

    // add the host mapping to the drupal database so we can use it for display without contating zabbix
    $sql = 'INSERT INTO {zabbix_hosts} (userid, hostid, zabbixhostid, hostname, enabled)';
    $sql .= " VALUES('%s', '%s', '%s', '%s', '%s')";

    $result = db_query($sql, $account->uid, NULL, $created_host_id, $host['host'], $host['status']);

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
