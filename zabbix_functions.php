<?php
/**
 * Description of ZabbixAPI-strong
 *
 * @author olindata
 */
include_once('ZabbixAPI.class.php');

function zabbix_api_login() {
    
    // Get the credentials configured in this module
    $API_url  = variable_get('zabbix_bridge_API_url', NULL);
    $API_user = variable_get('zabbix_bridge_API_user', NULL);
    $API_pass = variable_get('zabbix_bridge_API_pass', NULL);

    if (!isset($API_url, $API_user, $API_pass)) {
        drupal_set_message('Unable to login. API credentials must be set in module setting before using this module!', DRUPAL_MSG_TYPE_ERR);
    }

    // This logs into Zabbix, and returns false if it fails
    return ZabbixAPI::login($API_url, $API_user, $API_pass);    
    
}

?>
