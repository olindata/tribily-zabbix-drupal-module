<?php
/**
 * Description of ZabbixAPI-strong
 *
 * @author olindata
 */
include_once('ZabbixAPI.class.php');

define('HOST_NAME_MAXLENGTH', 64);
define('HOST_SERVER_MAXLENGTH', 64);
define('DRUPAL_MSG_TYPE_ERR', 'error');
define('DRUPAL_MSG_TYPE_STATUS', 'status');

        // from zabbix/include/defines.inc.php
        define('PERM_READ_WRITE',	3);
        define('PERM_READ_ONLY',	2);
        define('PERM_READ_LIST',	1);
        define('PERM_DENY',		0);

        define('EVENT_SOURCE_TRIGGERS',			0);
        define('EVENT_SOURCE_DISCOVERY',		1);
        define('EVENT_SOURCE_AUTO_REGISTRATION',2);

        define('ACTION_STATUS_ENABLED',		0);
        define('ACTION_STATUS_DISABLED',	1);

        define('ACTION_DEFAULT_MSG', '{TRIGGER.NAME}: {STATUS}');

        define('ACTION_EVAL_TYPE_AND_OR',	0);
        define('ACTION_EVAL_TYPE_AND',		1);
        define('ACTION_EVAL_TYPE_OR',		2);

	define('CONDITION_TYPE_HOST_GROUP',			0);
	define('CONDITION_TYPE_HOST',				1);
	define('CONDITION_TYPE_TRIGGER',			2);
	define('CONDITION_TYPE_TRIGGER_NAME',		3);
	define('CONDITION_TYPE_TRIGGER_SEVERITY',	4);
	define('CONDITION_TYPE_TRIGGER_VALUE',		5);
	define('CONDITION_TYPE_TIME_PERIOD',		6);
	define('CONDITION_TYPE_DHOST_IP',			7);
	define('CONDITION_TYPE_DSERVICE_TYPE',		8);
	define('CONDITION_TYPE_DSERVICE_PORT',		9);
	define('CONDITION_TYPE_DSTATUS',			10);
	define('CONDITION_TYPE_DUPTIME',			11);
	define('CONDITION_TYPE_DVALUE',				12);
	define('CONDITION_TYPE_HOST_TEMPLATE',		13);
	define('CONDITION_TYPE_EVENT_ACKNOWLEDGED',	14);
	define('CONDITION_TYPE_APPLICATION',		15);
	define('CONDITION_TYPE_MAINTENANCE',		16);
	define('CONDITION_TYPE_NODE',				17);
	define('CONDITION_TYPE_DRULE',				18);
	define('CONDITION_TYPE_DCHECK',				19);
	define('CONDITION_TYPE_PROXY',				20);
	define('CONDITION_TYPE_DOBJECT',			21);
	define('CONDITION_TYPE_HOST_NAME',			22);

	define('TRIGGER_SEVERITY_NOT_CLASSIFIED',	0);
	define('TRIGGER_SEVERITY_INFORMATION',		1);
	define('TRIGGER_SEVERITY_WARNING',			2);
	define('TRIGGER_SEVERITY_AVERAGE',			3);
	define('TRIGGER_SEVERITY_HIGH',				4);
	define('TRIGGER_SEVERITY_DISASTER',			5);

	define('CONDITION_OPERATOR_EQUAL',		0);
	define('CONDITION_OPERATOR_NOT_EQUAL',	1);
	define('CONDITION_OPERATOR_LIKE',		2);
	define('CONDITION_OPERATOR_NOT_LIKE',	3);
	define('CONDITION_OPERATOR_IN',			4);
	define('CONDITION_OPERATOR_MORE_EQUAL',	5);
	define('CONDITION_OPERATOR_LESS_EQUAL',	6);
	define('CONDITION_OPERATOR_NOT_IN',		7);

	define('OPERATION_OBJECT_USER',		0);
	define('OPERATION_OBJECT_GROUP',	1);

        define('EVENT_ACK_DISABLED',	'0');
	define('EVENT_ACK_ENABLED',		'1');

	define('OPERATION_TYPE_MESSAGE',		0);
	define('OPERATION_TYPE_COMMAND',		1);
	define('OPERATION_TYPE_HOST_ADD',		2);
	define('OPERATION_TYPE_HOST_REMOVE',	3);
	define('OPERATION_TYPE_GROUP_ADD',		4);
	define('OPERATION_TYPE_GROUP_REMOVE',	5);
	define('OPERATION_TYPE_TEMPLATE_ADD',	6);
	define('OPERATION_TYPE_TEMPLATE_REMOVE',7);
	define('OPERATION_TYPE_HOST_ENABLE',	8);
	define('OPERATION_TYPE_HOST_DISABLE',	9);

	define('MEDIA_TYPE_EMAIL',		0);
	define('MEDIA_TYPE_EXEC',		1);
	define('MEDIA_TYPE_SMS',		2);
	define('MEDIA_TYPE_JABBER',		3);

	define('TRIGGER_VALUE_FALSE',		0);
	define('TRIGGER_VALUE_TRUE',		1);
	define('TRIGGER_VALUE_UNKNOWN',		2);


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
