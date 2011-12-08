<?php
require_once 'zabbix_functions.php';

function zabbix_dashboard_dashboard()
{
	// We are using our own curl stuff instead of the ZabbixAPI Class because our cause (get the sid) is special and no ordinary api operation
	global $user;

	$url     = variable_get('zabbix_bridge_API_url', NULL);
	$apiUrl  = $url.ZabbixAPI::ZABBIX_API_URL;
	$name    = $user->name;
	$md5hash = $user->pass;
	$json    = array("jsonrpc" => "2.0", "method" => "user.authenticate", "params" => array("user" => $name, "password" => $md5hash, "overrideHashing" => true), "id" => 1);

	$ch = curl_init($apiUrl);
	curl_setopt($ch,CURLOPT_URL,$apiUrl);
	curl_setopt($ch,CURLOPT_POST,1);
	curl_setopt($ch,CURLOPT_CUSTOMREQUEST, "POST");
	curl_setopt($ch,CURLOPT_POSTFIELDS, json_encode($json));
	curl_setopt($ch,CURLOPT_RETURNTRANSFER,true);
	curl_setopt($ch,CURLOPT_HTTPHEADER,array("Content-Type: application/json-rpc"));
	$ret = curl_exec($ch);
	$ret = json_decode($ret);

	if(isset($ret->error))
	{
		drupal_set_message('Unable to login. It seems that something is wrong here. Please contact the Site Administrator.', DRUPAL_MSG_TYPE_ERR);
		return "Oh noes!";
	}

	$sid = $ret->result;

	header("Location: ".$url."index.php?setCookie=1&setSid=".$sid);
	exit();

}

