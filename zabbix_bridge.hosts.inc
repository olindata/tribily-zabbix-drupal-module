<?php
// $Id: zabbix_bridge.hosts.inc,v 1.7 2010/09/19 11:45:09 rmiddle Exp $

/**
 * @file
 * Settings form.
 */

define('HOST_NAME_MAXLENGTH', 50);

/**
 * Form builder; Present the form to edit a given user's hosts.
 * Added by Raj
 *
 * @ingroup forms
 */
function zabbix_hosts($register = FALSE) {
    $account = user_load(array('uid' => arg(1)));
    drupal_set_title(check_plain($account->name) . ' Hosts');

    return drupal_get_form('zabbix_hosts_form', $account);
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

    // This logs into Zabbix, and returns false if it fails
    ZabbixAPI::login('http://zabbix-dev.tribily.com/','admin','zabbix')
        or die('Unable to login: '.print_r(ZabbixAPI::getLastError(),true));

    // This gets the version of the zabbix server
    $version = ZabbixAPI::fetch_string('apiinfo','version')
        or die('Unable to get Zabbix Version: '.print_r(ZabbixAPI::getLastError(),true));
    echo_debug("Server running Zabbix API Version: $version\n");

    $form['hosts'] = array('#type' => 'fieldset',
        '#title' => t('Add Host'),
        '#weight' => -10
    );

    $form['hosts']['name'] = array('#type' => 'textfield',
        '#title' => t('Name'),
        '#maxlength' => HOST_NAME_MAXLENGTH,
        '#required' => TRUE
    );

    $form['hosts']['server'] = array('#type' => 'textfield',
        '#title' => t('Server Address'),
        '#maxlength' => HOST_SERVER_MAXLENGTH,
        '#required' => TRUE
    );

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

    /* Not needed
    $form['hosts']['port'] = array('#type' => 'textfield',
        '#title' => t('Zabbix Agent Port'),
        '#maxlength' => USERNAME_MAX_LENGTH
    );
     *
     */

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

    /* set this to FALSE by default */
    /*
    $form['hosts']['ipmi'] = array(
        '#type' => 'checkbox',
        '#title' => t('Use IPMI'),
    );*/

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

    if ('' == $form_state['values']['server']) {

        form_set_error('hosts', t('You must specify a server address!'));

    }

    /* TODO: regex checking
    if ('' == $form_state['values']['server'] && 'ip' == $form_state['values']['connect']) {

        form_set_error('hosts', t('You must specify an IP adress for connection type IP!'));

    }
     *
     */

}

/**
 * Submit function for the user host add/editing form.
 */
function zabbix_hosts_form_submit($form, &$form_state) {

    var_dump($form);
    var_dump($form_state);

    cache_clear_all();

    drupal_set_message(t('Host successfully added.'));

    return;

}

?>
