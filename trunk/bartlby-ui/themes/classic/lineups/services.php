<?=$this->disp_box("legend")?>

<table class="table table-striped table-bordered " id='services_table'>
						  <thead>
							  <tr>
							  	<th>Server</th>
								  <th>State</th>
								  <th>LastCheck</th>
								  <th>NextCheck</th>
								  <th>Service</th>
								  <th>Output</th>
								  <th>Options</th>
							  </tr>
						  </thead>
						    <tbody>


<?=$this->disp_box("server_box_.*")?>

</tbody>
</table>



<?=$this->disp_box("UNPLACED")?>
