<?
set_time_limit(0);

function dnl($i) {
	return sprintf("%02d", $i);
}
include "layout.class.php";
include "config.php";
include "bartlby-ui.class.php";
$btl=new BartlbyUi($Bartlby_CONF);
$btl->hasRight("core.event_queue");
$layout= new Layout();
$layout->set_menu("core");
$layout->setTitle("Bartlby Last Event's");
$layout->Table("100%");

//Check if profiling is enabled
	$evnts="<table width=400>";
	
	for($x=128; $x>=0; $x--) {
		$msg=bartlby_event_fetch($btl->CFG, $x);
		if($msg[id] == 0) {
			continue;	
		}
		$evnts .= "<tr><td><font size=1>" . date("d.m.Y H:i:s", $msg[time]) . "</td><td> ID:<b>" . $msg[id] . "</b></td><td>" . $msg[message] . "</td></tr>";	
	}

	
	$evnts .="</table>";
	
	$layout->Tr(
	$layout->Td(
			Array(
				0=>Array(
					'colspan'=> 2,
					'show'=>$evnts
					)
			)
		)

	);	


$layout->TableEnd();

$layout->display();

