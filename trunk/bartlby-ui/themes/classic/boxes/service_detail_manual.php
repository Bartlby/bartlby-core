<table  width='100%'>
                <form name='pp' action='bartlby_action.php' method=POST>
                
                <tr>
                        <td width=150 class='font2'>Submit manually:</td>
                        <td align=left ><?php echo $plcs[state_dropdown]?><input type='hidden' name='action' value='submit_passive'><input type='hidden' name='service_id' value='<?php echo $plcs[service][service_id]?>'><input type='text' name='passive_text'><input type=submit value='store'></font></td>
                        <td>&nbsp;</td>
                </tr>
		<tr>
			<td colspan=3><i><font size=1>state is not permanent, next intervall may set the true value again</font></i></td>
		</tr>
                </form>
        </table>

