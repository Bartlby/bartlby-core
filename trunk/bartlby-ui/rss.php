<?
	include "layout.class.php";
	include "config.php";
	include "bartlby-ui.class.php";

	
	
	$btl=new BartlbyUi($Bartlby_CONF);
	$map = $btl->GetSVCMap($_GET[service_state]);	

	$xml = "<?xml version=\"1.0\" encoding=\"ISO-8859-1\" ?>
		<!DOCTYPE rss PUBLIC \"-//Netscape Communications//DTD RSS 0.91//EN\" \"http://my.netscape.com/publish/formats/rss-0.91.dtd\">
		<rss version=\"0.91\">
			<channel>
			<title>www.PC-Special.net Computer-Hilfe-Forum, Tipps und Tricks, Scripte, Downloads und mehr</title>
			<link>http://www.pc-special.net</link>
			<description>Die neuesten Artikel auf www.PC-Special.net - die Helpline für Webmaster und PC-Probleme im Internet.</description>
			<language>de-de</language>
			<copyright>2005 bartlby</copyright>
			
";
		while(list($k, $servs) = @each($map)) {
			$displayed_servers++;
			
			for($x=0; $x<count($servs); $x++) {
				$xml .= "<item>";
				$xml .= "<title>" . $servs[$x][server_name] . ":" . $servs[$x][service_name] . "[" . $btl->getState($servs[$x][current_state]) . "]</title>";
				$xml .= "<description>" . $servs[$x][new_server_text] . "</description>";
				$xml .= "<link>" . $btl->BASE_URL . "services.php?service_id=" . $servs[$x][service_id] . "</link>";
				$xml .= "</item>";	
			}
		}



$xml .= "
</channel>
</rss>
";

echo $xml;
	
?>