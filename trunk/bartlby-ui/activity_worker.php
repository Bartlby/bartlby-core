<?
include "layout.class.php";
include "config.php";
include "bartlby-ui.class.php";
$btl=new BartlbyUi($Bartlby_CONF);

$layout= new Layout();
$layout->set_menu("worker");
$layout->setTitle("");


$servs=$btl->GetWorker();
$optind=0;




	while(list($k, $v) = @each($servs)) {
		
		
		$v1=bartlby_get_worker_by_id($btl->CFG, $v[worker_id]);

		$workers[$optind][c]="";
		$workers[$optind][v]=$v[worker_id];	
		$workers[$optind][k]=$v1[name];
		$workers[$optind][opts]=$btl->getWorkerOptions($v1,$layout);
		$optind++;
	
	}
	
	
	

	
$title="Update Worker Notification Status";  
$content = "asdf";

$layout->create_box($title, $content, "core_info", array(
	'workers' => $workers,
), "activity_worker");



$layout->display("activity_worker");