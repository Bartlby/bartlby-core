// Copyright (c) 2012 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.





var req = new XMLHttpRequest();



function autoreloader() {
		settingsDefault(); 
		loadData();
		 d = new Date();
    st = document.querySelector("#status");
    st.innerHTML=d;
}




 document.addEventListener('DOMContentLoaded', function() {
		settingsDefault(); 
		loadData();
    document.querySelector('#alerts_only').addEventListener('click', savequery);
    document.querySelector('#downtimes').addEventListener('click', savequery);
    document.querySelector('#hide_warnings').addEventListener('click', savequery);
    document.querySelector('#hide_infos').addEventListener('click', savequery);
    setInterval(autoreloader, 5000);
   
});


function loadData() {
			
			mt = document.getElementById("mtable");
			mdiv = document.getElementById("maindiv");
			if(mt) {
				//mdiv.removeChild(mt);
				console.log("REMOVED");
			}
			req.open(
			    "GET",
			    localStorage["bartlby_url"] + "/services.php?json_output=1",
			    true);
			req.onload = loadPage;
			req.send(null);
}


function savequery() {
	calerts_only = document.getElementById("alerts_only");
	cdowntimes = document.getElementById("downtimes");
	chide_infos = document.getElementById("hide_infos");
	chide_warnings = document.getElementById("hide_warnings");
	
	
	console.log(calerts_only.checked);
	
	
	localStorage["warnings"]=chide_warnings.checked;
	localStorage["infos"]=chide_infos.checked;
	localStorage["alerts_only"]=calerts_only.checked;
	localStorage["downtimes"]=cdowntimes.checked;
	
	
	
	loadData();
	
}

function settingsDefault() {
	
	calerts_only = document.getElementById("alerts_only");
	cdowntimes = document.getElementById("downtimes");
	chide_infos = document.getElementById("hide_infos");
	chide_warnings = document.getElementById("hide_warnings");
	
	
	console.log(calerts_only.checked);
	
	
	
	if(chrome.extension.getBackgroundPage().localStorage.alerts_only == "true") {
		calerts_only.checked=true;
	} else {
		calerts_only.checked=false;
	}
	
		if(chrome.extension.getBackgroundPage().localStorage.warnings == "true") {
		chide_warnings.checked=true;
	} else {
		chide_warnings.checked=false;
	}
	
		if(chrome.extension.getBackgroundPage().localStorage.infos == "true") {
		chide_infos.checked=true;
	} else {
		chide_infos.checked=false;
	}
	
		if(chrome.extension.getBackgroundPage().localStorage.downtimes == "true") {
		cdowntimes.checked=true;
	} else {
		cdowntimes.checked=false;
	}
	
	
	
	
	
}
function loadPage() {
  //var photos = req.responseXML.getElementsByTagName("photo");


	alerts_only = document.getElementById("alerts_only");
	downtimes = document.getElementById("downtimes");
	hide_infos = document.getElementById("hide_infos");
	hide_warnings = document.getElementById("hide_warnings");
	

	obj = JSON.parse(req.responseText);
	
	o = "<table width=500  class='table table-striped table-bordered ' id=mtable>";
	o +="<thead>";
	o += "<tr>";
	//o += "<th>Server</th>"
	o += "<th>State</th>";

	o += "<th>Service</th>";
	o += "<th>Output</th>";
	
	o += "</tr>";
	o += "</thead>";
	
	var d = document.getElementById("maindiv");
	
	for(x=0; x<obj.length; x++) {
		lbl = 'label-info';
		
		if(obj[x].current_state == 0) {
			lbl="label-success";
		}
				if(obj[x].current_state == 2) {
			lbl="label-important";
		}
			if(obj[x].current_state == 1) {
			lbl="label-warning";
		}
		if(obj[x].is_downtime == 1) {
			lbl = "label-info";
		}
		if(alerts_only.checked) {
			
				if(obj[x].current_state == 0) {
					continue;
				}
		}	
		if(downtimes.checked ) {
			if(obj[x].is_downtime == 1) {
				continue;
			}
			
		}
		if(hide_warnings.checked) {
				if(obj[x].current_state == 1) {
					continue;
				}
				
		}
		if(hide_infos.checked) {
				if(obj[x].current_state == 4) {
					continue;
				}
		}
		
		sa = '<span class="label ' + lbl + '">';
		se = '</span>';
	
		o += "<tr>";
		//o += "<td width=100>" +  obj[x].server_name + "</td>";
		o += "<td>" + sa + obj[x].state_readable + se + "</td>";
		o += "<td width=2>" + obj[x].service_name + "</td>";
		o += "<td width=200>" + obj[x].new_server_text.substring(0,100) + "</td>";
		o += "</tr>"; 
	}
	o +="</table>";
	d.innerHTML=o;
	document.body.appendChild(d);
	chrome.browserAction.setBadgeText({text:obj.length+1});


 
}


