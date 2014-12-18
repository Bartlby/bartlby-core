<?
error_reporting(1);


//SAMPLE CALL VIA REST API:
// you require the SDK folder to be on the sending machine (and php5 + php5-mcrypt installed) - and a priv/pub key pair + api usage allowed in backend
	//echo -e 'asdasd\nadsas\nasdas'|php bartlby_submit_trap.php  -i localhost -p 9031 -d -s  /var/www/bartlby-ui/api/sdk/ -r -e http://localhost -K 30391def8ac1bf34310ad32f2edf929a7ae642fc -P cbafc69f1134e866d8ac291d23c45e46d21f70b2


//SAMPLE Call if you have direct access to portier
//This is uncrypted unauthenticated
//echo -e 'asdasd\nadsas\nasdas'|php bartlby_submit_trap.php  -i localhost -p 9031 -d


$shortopts  = "";
$shortopts .= "i:";  // IP of Portier
$shortopts .= "p:"; // Port of Portier
$shortopts .= "d"; // Debug Output
$shortopts .= "s:"; // SDK Folder
$shortopts .= "r"; // Use RestAPI
$shortopts .= "e:"; // Rest EndPoint

$shortopts .= "K:"; //PRIV KEY
$shortopts .= "P:"; //PUB KEY



$longopts  = array();
$options = getopt($shortopts, $longopts);

var_dump($options);
$data = "";
while($f = fgets(STDIN)){
	$data .= $f;

}
$obj["method"]="submit_trap";
$obj["data"]=$data;
$jso = json_encode($obj);

if ($options["e"] == "" ) {
	$fp = fsockopen($options["i"], $options["p"]);
	if($options["d"] == false) echo "Sending: " . $jso . "\n";
	fwrite($fp, $jso);
	$r= fgets($fp);
	if($options["d"] == false) echo "Result: " . $r . "\n";
	fclose($fp);
}  else {
	chdir($options["s"]);
	include "bartlby_sdk.php";
	$priv_key=$options["K"];
	$pub_key=$options["P"];


	$sdk = new BartlbyAPISDK($priv_key, $pub_key, $options["e"]);

	$json_data=$jso;
	$params="";
	$method="POST";
	$request_uri="/api/v1/portier/cmd";

	$r = $sdk->doRequest($request_uri,$method, $params, $json_data);
	$arr = json_decode($r,true);
	var_dump($arr);


}



?>
