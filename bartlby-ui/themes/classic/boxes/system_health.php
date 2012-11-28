<?
/*
$layout->create_box($health_title, $health_content,"system_health", array(
			'prozent_float' => $prozent_float,
			'color' => $color
		), "system_health");
*/
?>
<?
if($plcs[color] == "green") {
	
	$progress_css = "progress-success";
}
if($plcs[color] == "red") {
	
	$progress_css = "progress-warning";
}

if($plcs[color] == "yello") {
	
	$progress_css = "progress-danger";
}



?>
	

		
		<div class="progress <?=$progress_css?>" >
							<div class="bar" style="width: <?=$plcs[prozent_float]?>%;"><?=$plcs[prozent_float]?>%</div>
						</div>
