
	global_reload=1;
	function toggleReload() {
		if(global_reload == 1) {
				global_reload=0;
		} else {
				global_reload=1;
		}
	}
	function GenericToggleFix(elID, st) {
	//alert(elID);
	//alert(st);
		obj=document.getElementById(elID);
		//alert(obj);
		obj.style.display=st;  
	}
	function GenericToggle(elID) {
		obj=document.getElementById(elID);
		obj.style.display=!(obj.style.display=="block")? "block" : "none";  
	}
	function jsLogout() {
		r=confirm("You really want to logout?");	
		if(r == true) {
			document.location.href='logout.php';	
		}
	}
	function doToggle(elID) {
		switch(elID) {
			case 'main':
				elID="Monitoring";
			break;
			case 'report':
				elID="Reporting";
			break;
			case 'client':
				elID="Server/s";
			break;
			case 'services':
				elID="Service/s";
			break;
			case 'downtimes':
				elID="Downtime/s";
			break;
			case 'worker':
				elID="Worker/s";
			break;
			case 'core':
				elID="Core";
			break;
			
		}
		imgPlus='themes/'+js_theme_name+'/images/plus.gif';
		imgMinus='themes/'+js_theme_name+'/images/minus.gif';
		obj=document.getElementById(elID + "_sub");
		obj.style.display=!(obj.style.display=="block")? "block" : "none";  
		
		
		obji=document.getElementById(elID + "_plus");
		cImg="images" + obji.src.substring(obji.src.lastIndexOf("/"), obji.src.length);
		
		
		obji.src=!(cImg==imgMinus)? imgMinus : imgPlus;  
		
	}

	var buffer_suggest = 
	{
	        bufferText: false,
	        bufferTime: 500,
	        
	        modified : function(strId, fcn, scr)
	        {
	                setTimeout('buffer_suggest.compareBuffer("'+strId+'","'+document.getElementById(strId).value+'","'+ fcn +'", "'+scr+'");', this.bufferTime);
	        },
	        
	        compareBuffer : function(strId, strText, fcn, scr)
	        {
	            if (strText == document.getElementById(strId).value && strText != this.bufferText)
	            {
	                this.bufferText = strText;
	                buffer_suggest.makeRequest(strId, fcn, scr);
	            }
	        },
	        
	        makeRequest : function(strId, fcn, scr)
	        {
	            	            
	            eval(fcn + "(document.getElementById(strId).value, scr)");
	        }
	}



function serviceManageIconChange(f) {
	selval=f.server_icon.options[f.server_icon.selectedIndex].value;
	ph = document.getElementById("picholder");
	ph.innerHTML="<img src='server_icons/" + selval + "'>";
		
}
function openMap() {
	window.open('create_map.php','','width=1024,height=786, scrollbar=yes, scrollbars=yes')
}
function doReloadButton() {
	var obj = document.getElementById("reload");
        obj.style.visibility = "visible";
}

var menuwidth='250px' //default menu width
var menubgcolor='999999'  //menu bgcolor
var disappeardelay=250  //menu disappear speed onMouseout (in miliseconds)
var hidemenu_onclick="yes" //hide menu when user clicks within menu?

/////No further editting needed

var ie4=document.all
var ns6=document.getElementById&&!document.all

if (ie4||ns6)
document.write('<div id="dropmenudiv" style="visibility:hidden;width:'+menuwidth+';background-color:'+menubgcolor+'" onMouseover="clearhidemenu()" onMouseout="dynamichide(event)"></div>')

function getposOffset(what, offsettype){
var totaloffset=(offsettype=="left")? what.offsetLeft : what.offsetTop;
var parentEl=what.offsetParent;
while (parentEl!=null){
totaloffset=(offsettype=="left")? totaloffset+parentEl.offsetLeft : totaloffset+parentEl.offsetTop;
parentEl=parentEl.offsetParent;
}
return totaloffset;
}


function showhide(obj, e, visible, hidden, menuwidth){
if (ie4||ns6)
dropmenuobj.style.left=dropmenuobj.style.top=-500
if (menuwidth!=""){
dropmenuobj.widthobj=dropmenuobj.style
dropmenuobj.widthobj.width=menuwidth
}
if (e.type=="click" && obj.visibility==hidden || e.type=="mouseover")
obj.visibility=visible
else if (e.type=="click")
obj.visibility=hidden
}

function iecompattest(){
return (document.compatMode && document.compatMode!="BackCompat")? document.documentElement : document.body
}

function clearbrowseredge(obj, whichedge){
var edgeoffset=0
if (whichedge=="rightedge"){
var windowedge=ie4 && !window.opera? iecompattest().scrollLeft+iecompattest().clientWidth-15 : window.pageXOffset+window.innerWidth-15
dropmenuobj.contentmeasure=dropmenuobj.offsetWidth
if (windowedge-dropmenuobj.x < dropmenuobj.contentmeasure)
edgeoffset=dropmenuobj.contentmeasure-obj.offsetWidth
}
else{
var topedge=ie4 && !window.opera? iecompattest().scrollTop : window.pageYOffset
var windowedge=ie4 && !window.opera? iecompattest().scrollTop+iecompattest().clientHeight-15 : window.pageYOffset+window.innerHeight-18
dropmenuobj.contentmeasure=dropmenuobj.offsetHeight
if (windowedge-dropmenuobj.y < dropmenuobj.contentmeasure){ //move up?
edgeoffset=dropmenuobj.contentmeasure+obj.offsetHeight
if ((dropmenuobj.y-topedge)<dropmenuobj.contentmeasure) //up no good either?
edgeoffset=dropmenuobj.y+obj.offsetHeight-topedge
}
}
return edgeoffset
}

function populatemenu(what){
if (ie4||ns6)
dropmenuobj.innerHTML=what.join("")
}


function dropdownmenu(obj, e, menucontents, menuwidth){
if (window.event) event.cancelBubble=true
else if (e.stopPropagation) e.stopPropagation()
clearhidemenu()
dropmenuobj=document.getElementById? document.getElementById("dropmenudiv") : dropmenudiv
populatemenu(menucontents)

if (ie4||ns6){
showhide(dropmenuobj.style, e, "visible", "hidden", menuwidth)
dropmenuobj.x=getposOffset(obj, "left")
dropmenuobj.y=getposOffset(obj, "top")
dropmenuobj.style.left=dropmenuobj.x-clearbrowseredge(obj, "rightedge")+"px"
dropmenuobj.style.top=dropmenuobj.y-clearbrowseredge(obj, "bottomedge")+obj.offsetHeight+"px"
}

return clickreturnvalue()
}

function clickreturnvalue(){
if (ie4||ns6) return false
else return true
}

function contains_ns6(a, b) {
while (b.parentNode)
if ((b = b.parentNode) == a)
return true;
return false;
}

function dynamichide(e){
if (ie4&&!dropmenuobj.contains(e.toElement))
delayhidemenu()
else if (ns6&&e.currentTarget!= e.relatedTarget&& !contains_ns6(e.currentTarget, e.relatedTarget))
delayhidemenu()
}

function hidemenu(e){
if (typeof dropmenuobj!="undefined"){
if (ie4||ns6)
dropmenuobj.style.visibility="hidden"
}
}

function delayhidemenu(){
if (ie4||ns6)
delayhide=setTimeout("hidemenu()",disappeardelay)
}

function clearhidemenu(){
if (typeof delayhide!="undefined")
clearTimeout(delayhide)
}

if (hidemenu_onclick=="yes")
document.onclick=hidemenu





