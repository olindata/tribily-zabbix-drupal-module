<?php
// $Id: zabbix_bridge.hosts.inc,v 1.7 2010/09/19 11:45:09 rmiddle Exp $

/**
 * @file
 * Settings form.
 */

define('HOST_NAME_MAXLENGTH', 64);
define('HOST_SERVER_MAXLENGTH', 64);
define('DRUPAL_MSG_TYPE_ERR', 'error');
define('DRUPAL_MSG_TYPE_STATUS', 'status');

include_once 'zabbix_functions.php';

/**
 * Form builder; Present the form to edit a given user's hosts.
 * Added by Raj
 *
 * @ingroup forms
 */
function zabbix_hosts($register = FALSE) {

    $account = user_load(array('uid' => arg(1)));
    drupal_set_title('Hosts to monitor');

    $output = zabbix_hosts_table();

    $output .= drupal_get_form('zabbix_hosts_form', $account);

    return $output;

}

function zabbix_hosts_table() {
  global $user;

  $header = array('Hostname', 'IP Address', 'DNS', 'Enabled', 'Role name', 'Role Description', 'Actions');
  $rows = array();

  $results = db_query("select h.hostname, h.ipaddress, h.dns, h.enabled, r.name as role_name, r.description as role_desc from {zabbix_hosts} h left join {zabbix_hosts_roles} hr on hr.hostid = h.hostid left join {zabbix_role} r on r.roleid = hr.roleid where h.userid = ".$user->uid);

  while ($node = db_fetch_object($results)) {
    $rows[] = array(array('data' => $node->hostname, 'align' => 'center'),
                    $node->ipaddress,
                    l($node->dns,$node->dns),
                    $node->enabled,
                    $node->role_name,
                    $node->role_desc,
                    l('Enable',''),
                );
   }
  $table_attributes = array('id' => 'hosts-table', 'align' => 'center');
  return theme('table', $header, $rows, $table_attributes);

}

/**
 * Form builder; add/edit a user's hosts.
 * Added by Raj
 * 
 * @ingroup forms
 * @see zabbix_hosts_form_validate()
 * @see zabbix_hosts_form_submit()
 */
function zabbix_hosts_form($form_state, $account) {
    
    $form['hosts'] = array('#type' => 'fieldset',
        '#title' => t('Add Host'),
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

    /*
    // retrieve zabbix templates, known as roles in drupal
    $templates = ZabbixAPI::fetch_array('template', 'get', array())
            or die('Unable to get Zabbix templates: '.print_r(ZabbixAPI::getLastError(),true));

    $templateids = array();

    foreach($templates as $template) {

        $templateids[] = $template['templateid'];

    }

    //echo_debug($templateids);

    // get the template details (templates are stored as hosts)
    $template_hosts = ZabbixAPI::fetch_array('host', 'get', array(   'templated_hosts' => true,
                                                                'extendoutput' => true,
                                                                'hostids' => $templateids ))
            or die('Unable to get template Zabbix hosts: '.print_r(ZabbixAPI::getLastError(),true));

    //echo_debug($template_hosts);

    // make the template SELECT options
    $template_options = array();

    foreach($template_hosts as $template_host) {

        $template_options[$template_host['hostid']] = t($template_host['host']);

    }
     *
     */

    $form['hosts']['roles'] = array(
        '#type' => 'checkboxes',
        '#title' => t('Roles'),
        '#default_value' => variable_get('hosts_roles', array()),
        '#options' => $template_options,
        '#description' => t('Assign this host to one or more roles.')
    );

    $form['hosts']['submit'] = array('#type' => 'submit', '#value' => t('Add'), '#weight' => 30);

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

    //echo_debug($form);
    $account = user_load(array('uid' => arg(1)));
    
    echo_debug($form_state);

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

    echo_debug($host);

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

            $message = 'Unable to add host: '.print_r(ZabbixAPI::getLastError(),true);

        }
        else {

            $message = 'Host successfully added.';

            echo_debug($created_host_id);

            $template_add = ZabbixAPI::fetch_string('template', 'massAdd', array( 'templates' => $template_ids,
                                                                                'hosts' => array( 'hostid' => $created_host_id ) ));

            echo_debug($template_add);

            if (!$template_add) {

                $message = 'Unable to link templates: '.print_r(ZabbixAPI::getLastError(), true);

            }
            else {

                $message = 'Host successfully added.';

            }

        }
    }

    cache_clear_all();

    echo_debug($message);
    drupal_set_message(t($message));

    return;

}

?>
