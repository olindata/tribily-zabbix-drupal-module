<?php

/**
 * @file
 * User mapping functions. Used for mapping drupal users to their zabbix counterparts
 *
 * @author raj
 */
include_once 'zabbix_functions.php';

/**
 * @author walterheck
 *
 * @param <type> $form_state
 * @param <type> $action
 * @param <type> $drupal_uid
 * @return int
 */
function zabbix_bridge_user_mapping_form(&$form_state, $action, $id) {

    if ($action == 'update') {
        $sql = "select id, drupal_uid, zabbix_uid, zabbix_usrgrp_id, zabbix_hostgrp_id from {zabbix_drupal_account} where id = '%s'";
        $result = db_query($sql, $id);
        $zabbix_drupal_account = db_fetch_object($result);
    }



    $form['user_mapping'] = array(
        '#type' => 'fieldset',
        '#title' => t('Mapping Drupal <=> Zabbix'),
    );

    $form['user_mapping']['id'] = array(
        '#type' => 'hidden',
        '#value' => $action == 'update' ? $zabbix_drupal_account->id : '',
    );

    $form['user_mapping']['drupal_userid'] = array(
        '#type' => 'textfield',
        '#title' => t('Drupal userid'),
        '#value' => $action == 'update' ? $zabbix_drupal_account->drupal_uid : '',
        '#description' => t('The id that this user corresponds to in the {users}.uid field in the drupal database'),
    );

    $form['user_mapping']['zabbix_userid'] = array(
        '#type' => 'textfield',
        '#title' => t('Zabbix userid'),
        '#value' => $action == 'update' ? $zabbix_drupal_account->zabbix_uid : '',
        '#description' => t('The id that this user corresponds to in the users.userid field in the zabbix database'),
    );

    $form['user_mapping']['zabbix_usergroupid'] = array(
        '#type' => 'textfield',
        '#title' => t('Zabbix usergroupid'),
        '#value' => $action == 'update' ? $zabbix_drupal_account->zabbix_usrgrp_id : '',
        '#description' => t('The id of the usergroup this user is connected to.\n
                             The zabbix bridges uses this for permissions\n
                             It corresponds to usrgrp.usrgrpid in the zabbix database.'),
    );

    $form['user_mapping']['zabbix_hostgroupid'] = array(
        '#type' => 'textfield',
        '#title' => t('Zabbix hostgroupid'),
        '#value' => $action == 'update' ? $zabbix_drupal_account->zabbix_hostgrp_id : '',
        '#description' => t('The hostgroup that all hosts this user adds in the zabbix bridge will be added to.\n
                             The zabbix bridges uses this for permissions.\n
                             This corrsponds to groups.groupid in the zabbix database.'),
    );

    $form['hosts']['submit'] = array(
        '#type' => 'submit',
        '#value' => t('submit'),
        '#weight' => 30
    );

    return $form;
}

/**
 *
 * @param <type> $form
 * @param <type> $form_state
 */
function zabbix_bridge_user_mapping_form_submit($form, &$form_state) {

    
    print_r($form_state['clicked_button']['#post']);

    $id = $form_state['clicked_button']['#post']['id'];
    $drupal_uid = $form_state['clicked_button']['#post']['drupal_userid'] != '' ? $form_state['clicked_button']['#post']['drupal_userid'] : 'null';
    $zabbix_userid = $form_state['clicked_button']['#post']['zabbix_userid'] != '' ? $form_state['clicked_button']['#post']['zabbix_userid'] : 'null';
    $zabbix_usergroupid = $form_state['clicked_button']['#post']['zabbix_usergroupid'] != '' ? $form_state['clicked_button']['#post']['zabbix_usergroupid'] : 'null';
    $zabbix_hostgroupid = $form_state['clicked_button']['#post']['zabbix_hostgroupid'] != '' ? $form_state['clicked_button']['#post']['zabbix_hostgroupid'] : 'null';

    if ($id != '') {
        $sql = "update
                    {zabbix_drupal_account}
                set
                    drupal_uid = %s,
                    zabbix_uid = %s,
                    zabbix_usrgrp_id = %s,
                    zabbix_hostgrp_id = %s
                where
                    id = '%s'";
        $result = db_query($sql, $drupal_uid, $zabbix_userid, $zabbix_usergroupid, $zabbix_hostgroupid, $id);
        if ($result) {
            drupal_set_message(t('Drupal-zabbix account mapping updated succesfully'));
        } else {
            drupal_set_message(t('An error occured updating the Drupal-zabbix account mapping'), DRUPAL_MSG_TYPE_ERR);
        }


   } else {

        $sql = "insert into
                    {zabbix_drupal_account} (
                        drupal_uid,
                        zabbix_uid,
                        zabbix_usrgrp_id,
                        zabbix_hostgrp_id)
                values (
                    %s,
                    %s,
                    %s,
                    %s)";
        $result = db_query($sql, $id, $drupal_uid, $zabbix_userid, $zabbix_usergroupid, $zabbix_hostgroupid);
        if ($result) {
            drupal_set_message(t('Drupal-zabbix account mapping added succesfully'));
        } else {
            drupal_set_message(t('An error occured adding the Drupal-zabbix account mapping'), DRUPAL_MSG_TYPE_ERR);
        }
    }

    drupal_goto('admin/settings/zabbix_bridge');


}

?>
