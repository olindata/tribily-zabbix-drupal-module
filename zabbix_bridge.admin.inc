<?php

// $Id$

/**
 * @file
 * Settings form for admin configuration

 *
 */

include_once 'zabbix_functions.php';
/**
 *
 * @return <type>
 */
function zabbix_bridge_overview_form() {

  drupal_set_title('Overview of zabbix_bridge');

  $output = '<h2>' . t('User defined hosts') . '</h2>';

  $output .= t('These are all the hosts with their roles created by all users.');

  $output .= zabbix_hosts_table();

  $output .= l('Import from Zabbix', 'hosts/import');

  $output .= '<h2>' . t('Mapping of Drupal vs Zabbix objects') . '</h2>';

  $output .= t('This is teh list of currently configured mappings for drupal users to their corresponding zabbix object id\s');

  $output .= zabbix_user_mapping_table();

  $output .= l('Add mapping', 'zabbix-user-mapping/add');

  $output .= '<h2>' . t('Roles and their corresponding Zabbix Templates') . '</h2>';

  $output .= t('These are all the roles with their corresponding zabbix templateids.');

  $output .= zabbix_roles_table();

  $output .= l('Add mapping', 'zabbix-role-mapping/add') . ' | ' . l('Import from zabbix', 'zabbix-role-mapping/import');

  $output .= zabbix_emails_table();

  $output .= l('Add email', 'emails/add') . ' | ' . l('Import from zabbix', 'emails/import');

  $output .= zabbix_mobiles_table();

  $output .= l('Add mobile number', 'mobiles/add') . ' | ' . l('Import from zabbix', 'mobiles/import');

  $output .= zabbix_jabbers_table();

  $output .= l('Add jabber', 'jabbers/add') . ' | ' . l('Import from zabbix', 'jabbers/import');

  return $output;
}

/**
 *
 * @return <type>
 */
function zabbix_bridge_settings_form() {
  $form['zabbix_bridge_credentials'] = array(
      '#type' => 'fieldset',
      '#title' => t('Zabbix credentials'),
  );
  $form['zabbix_bridge_credentials']['zabbix_bridge_API_url'] = array(
      '#type' => 'textfield',
      '#title' => t('Zabbix API url'),
      '#default_value' => variable_get('zabbix_bridge_API_url', zabbix_bridge_API_url),
      '#required' => TRUE,
      '#description' => t('Url used for API calls. If you don\'t know what to put here, try the full path to your zabbix install, like: http://example.com/zabbix')
  );

  $form['zabbix_bridge_credentials']['zabbix_bridge_API_user'] = array(
      '#type' => 'textfield',
      '#title' => t('Zabbix API user'),
      '#default_value' => variable_get('zabbix_bridge_API_user', zabbix_bridge_API_user),
      '#required' => TRUE,
      '#description' => t('User used for API calls. Has to have API calling enabled, and has to be a super-admin.')
  );

  $form['zabbix_bridge_credentials']['zabbix_bridge_API_pass'] = array(
      '#type' => 'password_confirm',
      '#title' => t('Zabbix API password'),
      '#required' => TRUE,
      '#size' => 25,
      '#description' => t('Password used for API calls. This is the zabbix password of the API user.')
  );


  // Add zabbix trace deletion options
  $trace_options = array(
      'yes' => 'Yes',
      'no' => 'No'
  );

  $form['zabbix_bridge_trace'] = array(
      '#type' => 'radios',
      '#title' => t('Delete Zabbix Traces Upon Account Deletion'),
      '#default_value' => variable_get('zabbix_bridge_trace', 'no'),
      '#description' => t('If checked, deleting a user account will remove all Zabbix traces (user, usergroup, host, hostgroup, media, actions)'),
      '#options' => $trace_options
  );

  // add debug message options
  $debug_options = array(
      'yes' => 'Yes',
      'no' => 'No'
  );

  $form['zabbix_bridge_debug'] = array(
      '#type' => 'radios',
      '#title' => T('Enable Debug Messages'),
      '#default_value' => variable_get('zabbix_bridge_debug', 'no'),
      '#description' => t('Determines whether debug messages show up or not. This should NOT be enabled in production!'),
      '#options' => $debug_options
  );

  $form['zabbix_bridge_debug_verbose'] = array(
      '#type' => 'radios',
      '#title' => T('Enable Verbose Debug Messages'),
      '#default_value' => variable_get('zabbix_bridge_debug_verbose', 'no'),
      '#description' => t('Determines whether debug messages show full information. Less readable, more debuggable :)'),
      '#options' => $debug_options
  );



  return system_settings_form($form);
}
