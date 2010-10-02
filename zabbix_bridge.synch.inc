<?php

/**
 * @file
 * Settings form.
 */

define('DRUPAL_MSG_TYPE_ERR', 'error');
define('DRUPAL_MSG_TYPE_STATUS', 'status');

/**
 * Form builder; Present the form to edit a given user's hosts.
 * Added by Walter
 *
 * @ingroup forms
 */
function zabbix_bridge_synch($register = FALSE) {

    $account = user_load(array('uid' => arg(1)));
    drupal_set_title('Synchronise Zabbix <=> Drupal');

    return drupal_get_form('zabbix_bridge_synch_form', $account);

}

/**
 * Form builder; 
 * Added by Walter
 * 
 * @ingroup forms
 * @see zabbix_hosts_form_validate()
 * @see zabbix_hosts_form_submit()
 */
function zabbix_bridge_synch_form($form_state, $account) {
    
    $form['hosts'] = array('#type' => 'fieldset',
        '#title' => t('To be implemented'),
        '#weight' => -10
    );


    $form['hosts']['submit'] = array('#type' => 'submit', '#value' => t('Add'), '#weight' => 30);

    return $form;

}

/**
 * Submit function for the user host add/editing form.
 */
function zabbix_bridge_synch_form_submit($form, &$form_state) {

    drupal_set_message(t('To add later'));

    return;

}

?>
