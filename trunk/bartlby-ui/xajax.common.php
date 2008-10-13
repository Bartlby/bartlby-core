<?
$xajax = new xajax("xajax_dispatcher.php");
$xajax->registerFunction("AddModifyWorker");
$xajax->registerFunction("AddModifyClient");
$xajax->registerFunction("AddModifyService");
$xajax->registerFunction("CreateReport");
$xajax->registerFunction("CreatePackage");
$xajax->registerFunction("AddDowntime");
$xajax->registerFunction("QuickLook");
$xajax->registerFunction("ServerSearch");
$xajax->registerFunction("jumpToServerId");

$xajax->registerFunction("toggle_service_check");
$xajax->registerFunction("toggle_service_notify_check");

$xajax->registerFunction("toggle_server_check");
$xajax->registerFunction("toggle_extension");
$xajax->registerFunction("toggle_server_notify_check");

$xajax->registerFunction("service_noaction");
$xajax->registerFunction("set_service_search_noact");


$xajax->registerFunction("ServiceSearch");
$xajax->registerFunction("jumpToServiceId");

$xajax->registerFunction("UserSearch");
$xajax->registerFunction("PluginSearch");
$xajax->registerFunction("SelectPlugin");

$xajax->registerFunction("jumpToUserId");

$xajax->registerFunction("removeDIV");

$xajax->registerFunction("updatePerfHandler");

$xajax->registerFunction("ExtensionAjax");
$xajax->registerFunction("group_search");
$xajax->registerFunction("forceCheck");

$xajax->registerFunction("BulkServiceSearch");
$xajax->registerFunction("IphoneOverView");

?>
