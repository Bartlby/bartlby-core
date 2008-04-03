<?
include "layout.class.php";
include "config.php";
include "bartlby-ui.class.php";
$btl=new BartlbyUi($Bartlby_CONF);

$layout= new Layout();
$layout->setTemplate("nonav.html");

$layout->OUT .= "<script>
		function appl() {
			
			var fstr='';
			for(x=0; x<document.fm1.mm.value; x++) {
				
				e=eval('document.fm1.b_'+x);
				
				f=eval('document.fm1.s_'+x+'.value');
				
				if(e.options[e.selectedIndex].value != -1) {
					fstr += f + '=' + e.options[e.selectedIndex].value + '|';
				}
				//alert(e.options[e.selectedIndex].value);
				//va=e.options[e.selectedIndex].value;
				//alert(va);
					
			}
			
			
			window.opener.document.forms[\"fm1\"].service_var.value='|' + fstr.substring(0, fstr.length-1) + '|';
		 	window.close();
		}
		</script>
";

$layout->Form("fm1", "bartlby_action.php");
$layout->Table("100%");





$layout->Tr(
	$layout->Td(
			Array(
				0=>Array(
					'colspan'=> 3,
					'class'=>'header',
					'show'=>'Group Definition'
					)
			)
		)

);

$x=0;
$ges=0;
$map = $btl->GetSVCMap();
	
while(list($k, $servs) = each($map)) {
	
	
	for($x=0; $x<count($servs); $x++) {
		
		$ibox[0][v]=-1;	
		$ibox[0][k]="unused";
		
		$ibox[1][c]="green";
		$ibox[1][v]=0;	
		$ibox[1][k]="OK";
		$ibox[2][c]="orange";        
		$ibox[2][v]=1;	  
		$ibox[2][k]="Warning";
		$ibox[3][c]="red";        
		$ibox[3][v]=2;	  
		$ibox[3][k]="Critical";
	
		$h=$_GET[str];
		$ibox[0][s]=1;
		$ibox[1][s]=0;
		$ibox[2][s]=0;
		$ibox[3][s]=0;
		
		if(strstr((string)$h, (string)$servs[$x][service_id] . "=0")) {
			$ibox[1][s]=1;
		}
		
		if(strstr((string)$h, (string)$servs[$x][service_id] . "=1")) {
			$ibox[2][s]=1;	
		}
		
		if(strstr((string)$h, (string)$servs[$x][service_id] . "=2")) {
			$ibox[3][s]=1;
				
		}
		
		
		$class="header1";
		
		
		$svc++;
		
		if($x == 0) {
			$server_color="header";
			
			$SERVER=$servs[$x][server_name];
		} else {
			$server_color="black";
			$SERVER="&nbsp;";
			
		}
		
			$layout->Tr(
				$layout->Td(
					array(
						0=>array(
							"show" => "<b>$SERVER</b>",
							"class" => $class
							),
						1=>array(
							"show" => $servs[$x][service_name],
							"class" => $class
							),
						2=>array(
							"show" => $layout->DropDown("b_" . $ges, $ibox) . "<input type=hidden value='" . $servs[$x][service_id]  . "' name='s_" . $ges . "'>",
							"class" => $class
							)
						
					)
				)
			);
			
		
		
		$ges++;
		
	}
}


$layout->Tr(
	$layout->Td(
			Array(
				0=>array(
					"show"=>"<input type=hidden name=mm value='$ges'><input type=button value='Apply' onClick=appl();>",
					'colspan'=> 3,
					'class'=>'header'
					)
			)
		)

);


$layout->TableEnd();
$layout->FormEnd();
$layout->display();