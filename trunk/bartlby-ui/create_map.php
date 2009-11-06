<?php
	include "config.php";
	include "layout.class.php";
	include "bartlby-ui.class.php";
	$btl=new BartlbyUi($Bartlby_CONF);
	$info=$btl->getInfo();
	$layout= new Layout();
	$layout->setTemplate("nonav.html");
?>
<script type="text/javascript" src="js/dom-drag.js"></script>
<script type="text/javascript">

/***********************************************
* Cool DHTML tooltip script II- © Dynamic Drive DHTML code library (www.dynamicdrive.com)
* This notice MUST stay intact for legal use
* Visit Dynamic Drive at http://www.dynamicdrive.com/ for full source code
***********************************************/

var offsetfromcursorX=12 //Customize x offset of tooltip
var offsetfromcursorY=10 //Customize y offset of tooltip

var offsetdivfrompointerX=10 //Customize x offset of tooltip DIV relative to pointer image
var offsetdivfrompointerY=14 //Customize y offset of tooltip DIV relative to pointer image. Tip: Set it to (height_of_pointer_image-1).

document.write('<div id="dhtmltooltip"></div>') //write out tooltip DIV
document.write('<img id="dhtmlpointer" src="themes/<?=$layout->theme?>/images/arrow2.gif">') //write out pointer image

var ie=document.all
var ns6=document.getElementById && !document.all
var enabletip=false
if (ie||ns6)
var tipobj=document.all? document.all["dhtmltooltip"] : document.getElementById? document.getElementById("dhtmltooltip") : ""

var pointerobj=document.all? document.all["dhtmlpointer"] : document.getElementById? document.getElementById("dhtmlpointer") : ""

function ietruebody(){
return (document.compatMode && document.compatMode!="BackCompat")? document.documentElement : document.body
}

function ddrivetip(thetext, thewidth, thecolor){

if (ns6||ie){

if (typeof thewidth!="undefined") tipobj.style.width=thewidth+"px"
if (typeof thecolor!="undefined" && thecolor!="") tipobj.style.backgroundColor=thecolor
tipobj.innerHTML=thetext
enabletip=true
return false
}
}

function positiontip(e){
if (enabletip){
var nondefaultpos=false
var curX=(ns6)?e.pageX : event.clientX+ietruebody().scrollLeft;
var curY=(ns6)?e.pageY : event.clientY+ietruebody().scrollTop;
//Find out how close the mouse is to the corner of the window
var winwidth=ie&&!window.opera? ietruebody().clientWidth : window.innerWidth-20
var winheight=ie&&!window.opera? ietruebody().clientHeight : window.innerHeight-20

var rightedge=ie&&!window.opera? winwidth-event.clientX-offsetfromcursorX : winwidth-e.clientX-offsetfromcursorX
var bottomedge=ie&&!window.opera? winheight-event.clientY-offsetfromcursorY : winheight-e.clientY-offsetfromcursorY

var leftedge=(offsetfromcursorX<0)? offsetfromcursorX*(-1) : -1000

//if the horizontal distance isn't enough to accomodate the width of the context menu
if (rightedge<tipobj.offsetWidth){
//move the horizontal position of the menu to the left by it's width
tipobj.style.left=curX-tipobj.offsetWidth+"px"
nondefaultpos=true
}
else if (curX<leftedge)
tipobj.style.left="5px"
else{
//position the horizontal position of the menu where the mouse is positioned
tipobj.style.left=curX+offsetfromcursorX-offsetdivfrompointerX+"px"
pointerobj.style.left=curX+offsetfromcursorX+"px"
}

//same concept with the vertical position
if (bottomedge<tipobj.offsetHeight){
tipobj.style.top=curY-tipobj.offsetHeight-offsetfromcursorY+"px"
nondefaultpos=true
}
else{
tipobj.style.top=curY+offsetfromcursorY+offsetdivfrompointerY+"px"
pointerobj.style.top=curY+offsetfromcursorY+"px"
}
tipobj.style.visibility="visible"
if (!nondefaultpos)
pointerobj.style.visibility="visible"
else
pointerobj.style.visibility="hidden"
}
}

function hideddrivetip(){
if (ns6||ie){
enabletip=false
tipobj.style.visibility="hidden"
pointerobj.style.visibility="hidden"
tipobj.style.left="-1000px"
tipobj.style.backgroundColor=''
tipobj.style.width=''
}
}

document.onmousemove=positiontip

</script>

<style type="text/css">

#dhtmltooltip{
position: absolute;
left: -300px;
width: 150px;
border: 1px solid black;
padding: 2px;
background-color: lightyellow;
visibility: hidden;
z-index: 100;
/*Remove below line to remove shadow. Below line should always appear last within this CSS*/
filter: progid:DXImageTransform.Microsoft.Shadow(color=gray,direction=135);
}

#dhtmlpointer{
position:absolute;
left: -300px;
z-index: 101;
visibility: hidden;
}

</style>



<script>
	var layer_array = new Array();
	var active_layer=null;
	var layerCount=0;
	
	function pushLayer(la) {
		layer_array.push(la);	
		layerCount++;		
	}
	function layer_pos() {
		document.storeit.storeString.value='';
		
		for(x=0; x<layer_array.length; x++) {
			if(layer_array[x].innerHTML != "") {
				document.storeit.storeString.value += "$layer["+layer_array[x].id+"][top]='" + layer_array[x].style.top +"';\n";
				document.storeit.storeString.value += "$layer["+layer_array[x].id+"][left]='" + layer_array[x].style.left +"';\n";
				document.storeit.storeString.value += "$layer["+layer_array[x].id+"][title]='" + layer_array[x].title +"';\n";
			}
			
		}
		document.storeit.submit();
		
		
		
			
	}
	function addLayer(title, HTML) {
		var newElem = document.createElement("div"); // neues DIV-Tag erstellen
		newElem.setAttribute("style", "position:relative; width:100px; height: 100px;"); // DIV stylen
		newElem.setAttribute("id", "uniq"+layerCount); // DIV eine ID geben, damit später noch darauf zugegriffen werden kann
		newElem.innerHTML = HTML; // HTML-Code in das DIV einfügen
		newElem.title=title;
		
		//newElem.onDblClick=function() {RemoveLayer(newElem)};
		newElem.setAttribute("onDblClick", "RemoveLayer(" + newElem.id + ")", true);
		//"RemoveLayer(" + newElem + ")"
		document.getElementsByTagName("body")[0].appendChild(newElem); // DIV-Tag in den Body einfügen 
		
		
		
		Drag.init(newElem);	
		pushLayer(newElem);
		
		
		
		return newElem.id;
	}
	function addLayerForm() {
		capt=document.addfm.newCaption.value;
		ico=document.addfm.icon[document.addfm.icon.selectedIndex].value;
		
		addTitleIcon(capt, ico);
		cancelAddLayer();
	}
	function addTitleIcon(capt, ico) {
		title=capt + "/" + ico;
		return addLayer(title, "<img src='server_icons/"+ico+"'><br>" + capt);	
	}
	function showAdd() {
		cancelLayerOpts();
		var obj = document.getElementById("addNew");
        	obj.style.visibility = "visible";	
	}
	function cancelAddLayer() {
		var obj = document.getElementById("addNew");
        	obj.style.visibility = "hidden";			
	}
	function RemoveLayer(wich) {
		active_layer=wich;
		
		showLayerOpts();
	}
	function showLayerOpts() {
		cancelAddLayer();
		var obj = document.getElementById("layeropts");
        	obj.style.visibility = "visible";	
	}
	function cancelLayerOpts() {
		var obj = document.getElementById("layeropts");
        	obj.style.visibility = "hidden";	
	}
	function ModifyLayer() {
		alert("Modify: " + active_layer.id);
	}
	function RemoLayer() {
		//alert("Remove:" + active_layer.id);	
		if(active_layer.id.substring(0, 6) == "server") {
			alert("servers can't be deleted");
		} else {
			active_layer.innerHTML="";
		}
		cancelLayerOpts();
	}
	function repositionIT() {
		/* echo "var layerTop = new Array();\n";
		echo "var layerLeft = new Array();\n";
		echo "var layerTitle = new Array();\n";
		echo "var layerId = new Array();\n";
		*/
		
		for(x=0; x<layerTop.length; x++) {
			
			obj = document.getElementById(layerId[x]);	
			if(obj) {
				
				obj.style.top=layerTop[x];	
				obj.style.left=layerLeft[x];
			} else {
				//Add a new One :-)
				ar=layerTitle[x].split("/");
				
				
				obj = document.getElementById(addTitleIcon(ar[0], ar[1]));	
				obj.style.top=layerTop[x];	
				obj.style.left=layerLeft[x];
			}
		}
	}
</script>
<input type="button" onClick='layer_pos();' value="Store">
<input type="button" onClick='showAdd();' value="Add">
<?php

	
	$layout->Table("100%");
	
	$server_ico="<select name='icon'>";
	$dhl=opendir("server_icons");
	while($file = readdir($dhl)) {
		//$sr=bartlby_get_server_by_id($btl->CFG, $k);
		
		//$isup=$btl->isServerUp($k);
		//if($isup == 1 ) { $isup="UP"; } else { $isup="DOWN"; }
		if(preg_match("/.*\.[png|gif]/", $file)) {
			
			$server_ico .="<option value='$file'>$file</option>";	
			
		}
		
	}
	closedir($dhl);
	$server_ico .="</select>";
	
	$map=$btl->getSVCMap();
	$ssum=0;
	while(list($server, $svcs) = each($map)) {
		if($btl->isServerUp($svcs[0][server_id], $map)) {
			$is_up="green";	
		} else {
			$is_up="red";
		}
		
		$qck[$svcs[0][server_name]][0]=0;
		$qck[$svcs[0][server_name]][1]=0;
		$qck[$svcs[0][server_name]][2]=0;
		$qck[$svcs[0][server_name]][downtime]=0;
		$qck[$svcs[0][server_name]][acks]=0;
		for($y=0; $y<count($svcs); $y++) {
			$ssum++;
			$qck[$svcs[$y][server_name]][$svcs[$y][current_state]]++;	
			$qck[$svcs[$y][server_name]][10]=$svcs[$y][server_id];
			$qck[$svcs[$y][server_name]][server_icon]=$svcs[$y][server_icon];
			if($svcs[$y][is_downtime] == 1) {
				$qck[$svcs[$y][server_name]][$svcs[$y][current_state]]--;
				$qck[$svcs[$y][server_name]][downtime]++;
				
			}
			if($svcs[$y][service_ack] == 2) {
				$qck[$svcs[$y][server_name]][acks]++;	
				$acks_outstanding++;
				
			}
		}
		
		
		$srvmsg = "<table><tr>";
		$srvmsg .= "<tr><td colspan=2><b>" . $svcs[0][server_name] . "</b></td></tr>";
		$srvmsg .= "<tr><td><font color=green>OK: </font></td><td>" . $qck[$svcs[0][server_name]][0] . "</td></tr>";
		$srvmsg .= "<tr><td><font color=orange>Warning: </font></td><td>" . $qck[$svcs[0][server_name]][1] . "</td></tr>";
		$srvmsg .= "<tr><td><font color=red>Critical: </font></td><td>" . $qck[$svcs[0][server_name]][2] . "</td></tr>";
		$srvmsg .= "<tr><td>Downtimes:  </td><td>" . $qck[$svcs[0][server_name]][downtime] . "</td></tr>";
		$srvmsg .= "<tr><td>Acks outstanding:  </td><td>" . $qck[$svcs[0][server_name]][acks] . "</td></tr>";
		$srvmsg .= "</tr></table>";
		
		$mover="ddrivetip('" . $srvmsg . "', 300);";
		$mout="hideddrivetip();";
		$layout->OUT .= "<div onClick=\"" . $mout . "\" onMouseOut=\"" . $mout . "\" onMouseOver=\"" . $mover . "\" xtitle='" . $svcs[0][server_name] . "/" . $svcs[0][server_icon] . "' onDblClick='RemoveLayer(this);' id='server" . $svcs[0][server_id] . "' style=\"position: relative;width:100px;height:100px\"><img  src='server_icons/" . $svcs[0][server_icon] . "'><br><font color='$is_up'>" . $svcs[0][server_name] . "</font></div>\n";	
		$layout->OUT .= '<script type="text/javascript">var hndl' . $svcs[0][server_id] . '=document.getElementById("server' . $svcs[0][server_id] . '"); Drag.init(hndl' . $svcs[0][server_id] . ', null);pushLayer(hndl' . $svcs[0][server_id] . ');</script>' . "\n";
	}
	
	$layout->TableEnd();
	
	
	//Load Defaults ;-)
	$fp=@fopen("create_map.dat","r");
		if($fp) {
			while(!feof($fp)) {
				$estr .= @fgets($fp, 1024);
			}
		}
	@fclose($fp);

	@eval($estr);
	echo "<script>\n";
	echo "var layerTop = new Array();\n";
	echo "var layerLeft = new Array();\n";
	echo "var layerTitle = new Array();\n";
	echo "var layerId = new Array();\n";
	echo "layerCount +=" . count($layer)+2 . ";";
	$cnt=0;
	while(list($k, $v) = @each($layer)) {
		echo "layerTop[$cnt] = '" . $v[top] . "';\n";
		echo "layerLeft[$cnt] = '" . $v[left] . "';\n";
		echo "layerTitle[$cnt] = '" . $v[title] . "';\n";
		echo "layerId[$cnt] = '" . $k . "';\n";
		$cnt++;
	}
	reset($map);
	echo "</script>\n";
	$srvmsg = "<table><tr>";
	$srvmsg .= "<tr><td colspan=2><b>Bartlby</b></td></tr>";
	$srvmsg .= "<tr><td>Servers:  </td><td>" . count($map) . "</td></tr>";
	$srvmsg .= "<tr><td>Services:  </td><td>" . $ssum . "</td></tr>";
	$srvmsg .= "<tr><td>Uptime:  </td><td>" . $btl->intervall(time()-$btl->info[startup_time]) . "</td></tr>";
	$srvmsg .= "</tr></table>";
	$mover="ddrivetip('" . $srvmsg . "', 300);";
	$mout="hideddrivetip();";
	echo '<div onMouseOut="' . $mout . '" onMouseOver="' . $mover . '" id="bartlby" style="position: relative; left:0; top:0;width:100px;height:100px"><img src="themes/' . $layout->theme . '/images/btl-logo.gif"></div>';

	echo $layout->OUT;
?>


<script language="javascript">
var aThumb = document.getElementById("bartlby");
pushLayer(aThumb);
Drag.init(aThumb, null);
repositionIT();
</script>



<div id="addNew" style="visibility:hidden; position: absolute; height: 300px; width: 200px;  left: 412px; top:100px; color:#000000; background-color: yellow; z-index:3;">
<form name="addfm">
<table width=100%>
	<tr>
		<td colspan=2><b>Add Element</td>
	</tr>
	<tr>
		<td>Caption:</td>
		<td><input type=text name="newCaption"></td>
	</tr>
	<tr>
		<td>Icon:</td>
		<td><?=$server_ico?></td>
	</tr>
	<tr>
		<td colspan=2><input type="button" value="add" onClick="addLayerForm();">
		<input type="button" value="cancel" onClick="cancelAddLayer();">
		</td>
	</tr>
	
</table>
</form>
</div>


<div id="layeropts" style="visibility:hidden; position: absolute; height: 300px; width: 200px;  left: 412px; top:100px; color:#000000; background-color: yellow; z-index:3;">
<form name="layeroptsfm">
<table width=100%>
	
	<tr>
		<td colspan=2><input type="button" value="remove" onClick="RemoLayer();">
		<input type="button" value="cancel" onClick="cancelLayerOpts();">
		</td>
	</tr>
	
</table>
</form>
</div>

<div id="output" style="visibility:hidden; position: absolute; height: 300px; width: 200px;  left: 600; top:100px; color:#000000; background-color: yellow; z-index:3;">
<form method=post name="storeit" action='bartlby_action.php'>
<input type=hidden name='action' value='storeMap'>
<textarea name='storeString' cols=50 rows=20>
</textarea>
</form>
</div>



