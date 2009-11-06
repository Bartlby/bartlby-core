<script>
		function updateWorkerState(id, el) {
			xajax_setWorkerState(id, el.options[el.selectedIndex].value);
			
		}
</script>

<table>
	<tr>
			<?php
				$a=$plcs[workers];
				for($x=0; $x<count($a); $x++) {
						echo "<tr height=20>";
						echo "<td>";
						echo $a[$x][k];
						echo "</td>";
						echo "<td>";
						echo $a[$x][opts];
						echo "</td>";
						echo "<td><div id=wstate" . $a[$x][v] . " name=" . $a[$x][v] . ">";
					
						echo "</div></td>";
						
						echo "</tr>";
				}
				
			
			?>
	</tr>

</table>