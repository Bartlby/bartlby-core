<?
dl("bartlby.so");
$cfgfile="/storage/SF.NET/BARTLBY/bartlby-core/bartlby.cfg";
var_dump(bartlby_set_worker_id($cfgfile, 83, 4));
exit(1);




//$btl=bartlby_open();
//$info=bartlby_get_info($btl);
//var_dump($info);
//var_dump(bartlby_get_service($cfgfile, 1));
//var_dump(bartlby_get_info($cfgfile, 1));

$mod_service=bartlby_modify_service("/storage/SF.NET/BARTLBY/bartlby-core/bartlby.cfg", 3186 , 14, "bartlby_swap.sh","timeouttest","",1, 00, 24, 00, 59,10,1,"", 0, 12);
echo "bartlby_modify_server()\n";
var_dump($mod_service);
exit;

echo "bartlby_get_service_map() \n";
$svcmap=bartlby_get_service_map($cfgfile);
var_dump($svcmap);
exit;


$info=@bartlby_get_info($cfgfile);
if(!$info) {
	echo "NOT RUNN\n";
	exit;	
}
echo "bartlby_version() \n";
var_dump(bartlby_version());
echo "\n";


$info=bartlby_get_info($cfgfile);
echo "bartlby_get_info()\n";
var_dump($info);

$svc=bartlby_get_service($cfgfile,1);
echo "bartlby_get_service()\n";
var_dump($svc);


$wrk=bartlby_get_worker($cfgfile, 0);
echo "bartlby_get_worker()\n";
var_dump($wrk);


$cfg=bartlby_config($cfgfile, "data_library");
echo "bartlby_config()\n";
var_dump($cfg);


$lib=bartlby_lib_info($cfgfile);
echo "bartlby_lib_info()\n";
var_dump($lib);

$add_server=bartlby_add_server($cfgfile, "Name", "127.0.0.1", "9030");
echo "bartlby_add_server()\n";
var_dump($add_server);

$mod_server=bartlby_modify_server($cfgfile, $add_server, "Modified Name", "127.0.0.1", "9030");
echo "bartlby_modify_server()\n";
var_dump($mod_server);


$get_server=bartlby_get_server_by_id($cfgfile, $add_server);
echo "bartlby_get_server_by_id()\n";
var_dump($get_server);


$del_server=bartlby_delete_server($cfgfile,$add_server);
echo "bartlby_delete_server()\n";
var_dump($del_server);


//&bartlby_config, &server_id, &plugin,&service_name,&plugin_arguments,&notify_enabled,&hour_from,&hour_to,&min_from,
//&min_to,&check_interval, &service_type,&service_var,&service_passive_timeout
$add_service=bartlby_add_service($cfgfile, 1, "testplg", "svcname", "plgargs", 1, 10,11,12,13,200,0,"VAR|Var1", 20);
echo "bartlby_add_service()\n";
var_dump($add_service);

$mod_service=bartlby_modify_service($cfgfile,$add_service, 1, "UPPPtestplg", "UPPsvcname", "plgargs", 1, 10,11,12,13,200,0,"VAR|Var1", 20);
echo "bartlby_modify_server()\n";
var_dump($mod_service);


$get_service=bartlby_get_service_by_id($cfgfile, $add_service);
echo "bartlby_get_service_by_id()\n";
var_dump($get_service);

$del_service=bartlby_delete_service($cfgfile, $add_service);
echo "bartlby_delete_service()\n";
var_dump($del_service);


system("clear");
echo "bartlby_get_worker()\n";
$a=bartlby_get_worker($cfgfile, 1);
var_dump($a);

//svc->mail, svc->icq, svc->services, svc->notify_levels, svc->active, svc->name
echo "bartlby_add_worker()\n";
$add=bartlby_add_worker($cfgfile, "mail@host.at", "89352629", "|38|", "|39|", 1, "helo", "pass");

var_dump($add);

echo "bartlby_get_worker_by_id()\n";
$byid=bartlby_get_worker_by_id($cfgfile, $add);
var_dump($byid);

echo "bartlby_modify_worker()\n";
$mod=bartlby_modify_worker($cfgfile, $add, "mail@host.at1", "19352629", "aaaa", "vvvv", 1, "helo1", "pass1");

var_dump($mod);

echo "bartlby_get_worker_by_id()\n";
$byid=bartlby_get_worker_by_id($cfgfile, $add);
var_dump($byid);

echo "bartlby_delete_worker()\n";
$del=bartlby_delete_worker($cfgfile, $add);
var_dump($del);

echo "bartlby_reload()\n";
$r=bartlby_reload($cfgfile);
var_dump($r);

$t=bartlbe_toggle_service_notify($cfgfile, 0);
echo "bartlbe_toggle_service_notify()";
var_dump($t);
$t=bartlbe_toggle_service_notify($cfgfile, 0);
echo "bartlbe_toggle_service_notify()";
var_dump($t);


$t=bartlbe_toggle_service_active($cfgfile, 0);
echo "bartlbe_toggle_service_active()";
var_dump($t);

$t=bartlbe_toggle_service_active($cfgfile, 0);
echo "bartlbe_toggle_service_active()";
var_dump($t);
?>
