<script>
	var bulk_selected_svcs = new Array();
	var bulk_result = new Array();
	
	function bulk_select_all() {
		bc=document.service_sel.bulk_services;
		for(x=0; x<bc.length; x++) {
			bc[x].checked=true;
		}		
	}

	function bulk_add_selected() {
		bc=document.service_sel.bulk_services;
		for(x=0; x<bc.length; x++) {
			if(bc[x].checked == true) {
				res=bc[x].value.split(";");
				//bulk_service_add(res[0], res[1]);
				bulk_result.push(new Array(res[0], res[1]));
				
			}
		}
		bulk_redraw_result();
		
	}
	function bulk_service_add(id, label) {
		bulk_selected_svcs.push(new Array(id, label));
		bulk_add_to_result(id, label);
	}
	function bulk_force_check() {
		for(x=0; x<bulk_result.length; x++) {
			sel_svc=bulk_result[x];
			id=sel_svc[0];
			label=sel_svc[1];
			
			document.getElementById("bulk_status").innerHTML="Forcing Check on: " + label;
			xajax_forceCheck(-1, id);

			
		}
		document.getElementById("bulk_status").innerHTML="DONE!";
	}
	function bulk_add_to_result(id, label) {
		for(x=0; x<bulk_result.length; x++) {
			c=bulk_result[x];
			if(c[0] == id) return;
		}
		document.getElementById("service_selected").innerHTML += label + " <a href=\"javascript:bulk_remove_from_result(" + id + ")\">remove</A><br>";
		bulk_result.push(new Array(id, label));
	}
	function bulk_remove_from_result(id) {
		for(x=0; x<bulk_result.length; x++) {
			c=bulk_result[x];
			if(c[0] == id) {
				bulk_result[x] = -1;			
			}
		}
		bulk_redraw_result();
	}
	function bulk_redraw_result() {
		document.getElementById("service_selected").innerHTML = "";
		for(x=0; x<bulk_result.length; x++) {
			c=bulk_result[x];
			if(c!=-1) {
				label=c[1];
				id=c[0];			
			
				document.getElementById("service_selected").innerHTML += label + " <a href=\"javascript:bulk_remove_from_result(" + id + ")\">remove</A><br>";
			}
		}
		
	}

</script>
<form name="service_sel" id=service_sel>
<div id=service_search>
	Search: <input type=text name=serv_search id=serv_search onkeyup="buffer_suggest.modified('serv_search', 'xajax_BulkServiceSearch');">
<b>Status</b>
<div id=bulk_status>
	
	
	
</div>
</div>
<div id=serviceXY>
	<input type=button onClick='bulk_add_selected()' value='Add Selected'>
	<input type=button onClick="bulk_select_all()" value='Select All from Result'>
</div>
<b>Search Result</b><br>
<div id=service_result>
	
</div>
<b>Already  Selected</b><br>
<div id=service_selected>
	
</div>
<b>Actions</b><br>
<div id=serviceXY>
	
	<input type=button onClick='bulk_force_check()' value='Force Check'>
	
</div>

</form>
