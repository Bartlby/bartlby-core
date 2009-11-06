<?php
	
	include "config.php";
	include "layout.class.php";
	include "bartlby-ui.class.php";
	
	include "extensions/OcL/OcL.class.php";
	
	
	
	$btl=new BartlbyUi($Bartlby_CONF);
	$btl->hasRight("ocl_add");
	$sg = new OcL();
	
	
	$layout= new Layout();
	$layout->setTitle("OcL: Add Entry");
	
	$layout->OUT .= "<script>function GrpChk() {
			window.open('grpstr.php?str='+document.fm1.service_var.value, 'grp', 'width=600, height=600, scrollbars=yes');
		}
	</script>";
	$layout->set_menu("OcL");
	$layout->Form("fm1", "extensions_wrap.php");
	$layout->Table("100%");
	
	
	
	$typos[0][s]=0;
	$typos[0][k]="phone";
	$typos[0][v]="phone";
	
	$typos[1][s]=0;
	$typos[1][k]="sms";
	$typos[1][v]="sms";
	
	
	$typos[2][s]=0;
	$typos[2][k]="email";
	$typos[2][v]="email";
	
	$typos[2][s]=0;
	$typos[2][k]="various";
	$typos[2][v]="various";
	

$layout->Tr(
	$layout->Td(
			Array(
				array("colspan" => 2, "show" => "<b>Add OnCall-Log-Entry</b>")
			)
		)

);	


$layout->Tr(
	$layout->Td(
			Array(
				0=>"Date:",
				1=>$layout->Field("ocl_date", "textbox", date("d.m.Y H:i"))
			)
		)

);	
$layout->Tr(
	$layout->Td(
			Array(
				0=>"Subject:",
				1=>$layout->Field("ocl_subject", "textbox", "")
			)
		)

);	
$layout->Tr(
	$layout->Td(
			Array(
				0=>"Duration:",
				1=>$layout->Field("ocl_duration", "textbox", "")
			)
		)

);	
$layout->Tr(
	$layout->Td(
			Array(
				0=>"Type-of-Activation:",
				1=>$layout->DropDown("ocl_type", $typos)
			)
		)

);
$layout->Tr(
	$layout->Td(
			Array(
				0=>"Caller:",
				1=>$layout->Field("ocl_caller", "textbox", "")
			)
		)

);		
$layout->Tr(
	$layout->Td(
			Array(
				0=>"Worker:",
				$btl->user
			)
		)

);	
$layout->Tr(
	$layout->Td(
			Array(
				0=>"Detailed description:",
				$layout->TextArea("ocl_error_long", "",10,70) . $layout->Field("script", "hidden", "OcL/save.php")
			)
		)

);

$layout->Tr(
	$layout->Td(
		array(
			0=>"Group definition",
			
			1=>$layout->Field("service_var", "hidden", "") . "<a href='javascript:GrpChk();'>Open Service selector</A>"
			
		)
	)
);





$layout->Tr(
	$layout->Td(
			Array(
				0=>Array(
					'colspan'=> 2,
					"align"=>"right",
					'show'=>$layout->Field("Subm", "button", "next->", "" ," onClick='xajax_ExtensionAjax(\"OcL\", \"xajax_ocl_add_form\", xajax.getFormValues(\"fm1\"))'")
					)
			)
		)

);
	$layout->FormEnd();
	
	$layout->TableEnd();
	$layout->display();