<!--
bspPlan = Array(31);
bspMDate = new Date;
adGlobal=1900;
if(bspMDate.getYear() >= 1900) {
	adGlobal=0;	
}


function bspMarkHours (d,m,y) {
	el = document.fm1.chkb;
	for(x=0; x<el.length; x++) {
		el[x].checked=el[x].checked ? false : true;	
	}
}
function bspDay(d,m,y) {
	bsp_debug("got day: " + d + "." + m + "." + y);	
	fday=new Date(y,m, 1);
	o="<table class=bsp>";
	o += "<tr><td colspan=6  class=bsp_header><b>" + d + "." + (m+1) + "." + (y) +"</b></td></tr>";
	o += "<tr><td colspan=6  class=bsp_header><b>Month</b></td><td colspan=12   class=bsp_header><b>" + (m+1) + "." + (y) + " </b></td></tr>";
	o += "<tr>";
	for(x=0; x<24; x++) {
		if(x%6 == 0) o += "</tr><tr>";
		o += "<td id=\"" + x + "_e\" class=bsp1 coslpan=2 align=left valign=top><div id='hr'>" + x + ":00</div><div id='chk'><input  type=checkbox name=chkb[] id=chkb value='" + x + "'></div>";
		o += "<br>";
		o += "<div id='actives_" + x + "'></div><br>";
		o += "<div id='standbys_" + x + "'></div></td>";	
	}		
	o += "</table>";
	
	onClick='xajax_ExtensionAjax(\"OcL\", \"xajax_ocl_add_form\",)'
	
	el1=document.getElementById("bsp_action");
	el1.innerHTML="<input type=button onClick=\"bspDate(1," + m + "," + y + ")\" value='Month'>";
	el1.innerHTML += "<input type=button onClick='xajax_ExtensionAjax(\"OcL\", \"xajax_ocl_add_active\", xajax.getFormValues(\"fm1\"), " + d + "," + (m+1) + "," + y + ")' value='Active'>";
	el1.innerHTML += "<input type=button onClick='xajax_ExtensionAjax(\"OcL\", \"xajax_ocl_add_standby\", xajax.getFormValues(\"fm1\"), " + d + "," + (m+1) + "," + y + ")' value='Standby'>";
	el1.innerHTML += "<input type=button onClick=\"bspMarkHours(" + d + "," + m + "," + y + ")\" value='Mark All'>";
	
	
	
	xajax_ExtensionAjax("OcL", "xajax_get_bsp_hours", d,(m+1),y);
	
	el=document.getElementById("bsp_main");
	el.innerHTML=o;
	
	bsp_debug("day initialised");
}

function bsp_mover_day(d,m,y) {
	bsp_debug("mouse over: " + d + " " + m + " " + y);
}
function bsp_prev() {
	pm = bspMDate.getMonth() - 1;
	py = bspMDate.getYear() + adGlobal;
	
	if(pm < 0) {
		py--;
		pm=11;	
	}
	bspDate(1, pm, py);
}
function bsp_next() {
	pm = bspMDate.getMonth() + 1;
	py = bspMDate.getYear() + adGlobal;
	
	if(pm >  11) {
		py++;	
		pm=0;
	}
	bspDate(1, pm, py);
}
function bspDate(d,m,y) {
	
	da=new Date(y,m,d);
	
	bspMDate=da;
	init_bsp();
	
	/*
	draw month	
	*/
	o="<table class=bsp><tr class=bsp>";
	
	o += "<tr><td colspan=7  class=bsp_header><b>"  + (bspMDate.getMonth()+1) + "." + (adGlobal+bspMDate.getYear()) + "</b></td></tr>";
	temp_day=new Date(y,m,1);
	bsp_debug("temp_day: " + temp_day.toString());
	//temp_day.setDate(bspMDate.getDate+x);
	
	for(x=0; x<bspPlan.length; x++) {
		temp_day=new Date(y,m,1);
		temp_day.setDate(bspMDate.getDate()+x);
		
		
		if(x%7 == 0) o += "</tr><tr>";
		
		o += "<td id=\"" + d + m + y + "_day" + "\" class=bsp onMouseOver=\"bsp_mover_day(" + temp_day.getDate() +"," + temp_day.getMonth() + ", " + (temp_day.getYear()+adGlobal) + ")\" onClick=\"bspDay(" + temp_day.getDate() +"," + temp_day.getMonth() + ", " + (temp_day.getYear()+adGlobal) + ")\"><a href='#'>" + temp_day.getDate() + "." + (temp_day.getMonth()+1) + "." + (adGlobal+temp_day.getYear()) + "</A></td>";			
	}
	
	o += "</table>";
	el=document.getElementById("bsp_main");
	el1=document.getElementById("bsp_action");
	el1.innerHTML="<input type=button onClick=\"bsp_prev()\" value='Prev.'>";
	el1.innerHTML +="<input type=button onClick=\"bsp_next()\" value='Next'>";
	
	/*
	el.disabled=true;
	xajax_ExtensionAjax("OcL", "xajax_get_bsp_entrys", "2007-02","a");
	*/
	
	el.innerHTML=o;
	
	bsp_debug("month initialised");
	
	
	
}

function bsp_debug(s) {
	did=document.getElementById("debug");
	did.innerHTML = s + "<br>" + did.innerHTML;
	
	
}

function init_bsp() {
	bsp_debug("Init");
	for(x=0; x<bspPlan.length; x++) {
		bspPlan[x] = new Object();	
		bsp_debug("New Object Container: " + x);
	}	
	bsp_debug("master date:" + bspMDate.toString());
}

//-->