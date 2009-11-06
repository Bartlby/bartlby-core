<?php
	include "config.php";
	include "layout.class.php";
	include "bartlby-ui.class.php";
	
	$btl = new BartlbyUi($Bartlby_CONF, false, false);
	$layout= new Layout();
	$layout->setTitle("Error");
	
	
$layout->OUT .= "<script>
		function doRetry() {
			document.location.href='start.php';	
		}
		function doStartup() {
			document.location.href='error.php?msg=BARTLBY::START';	
		}
		function doSHMClean() {
			document.location.href='error.php?msg=BARTLBY::SHMCLEAN';	
		}
		</script>
";
	
	$layout->Table("100%");

	
	
	switch ($_GET[msg]) {
		case 'BARTLBY::LOGIN':
			$omsg = "Login failed <br><a href='index.php'>Go to login page</A>";
		break;
		case 'BARTLBY::SHMCLEAN';
			
			
			$r=bartlby_shm_destroy($btl->CFG);
			$omsg = "SHM cleaned($r)";
			
		break;
		case 'BARTLBY::SHM::STALE':
			$omsg .= "it looks that there is a shared memory segment set up, wich is assigned to this bartlby instance<br>";
			$omsg .= "but the process is not found, or eather your  kernel doesnt have /proc/ FS support<br>";
			$omsg .= "<input type=button value='remove shared memory segment' onClick='doSHMClean();'>";
		break;
		case 'BARTLBY::START':
			//Set Env
			//Call bartlby.startup start
			$base_dir=bartlby_config($btl->CFG, "basedir");
			if(!$base_dir) {
				$omsg="basedir config not set";
			} else {
				$cmd="export BARTLBY_HOME='$base_dir'; cd \$BARTLBY_HOME; ./bartlby.startup restartwithout 2>&1";
				
				$fp=popen($cmd, "r");
				while(!feof($fp)) {
					$omsg .= fgets($fp, 1024);	
				}
				pclose($fp);	
				/*
				if(preg_match("/PIDFILE:.*exists/", $omsg)) {
					$pid_file=bartlby_config($btl->CFG, "pidfile_dir");
					$omsg .= "<br>retrying after pid file deletion (" . $pid_file . "/bartlby.pid)<br>";
					unlink($pid_file . "/bartlby.pid");
					$fp=popen($cmd, "r");
					$omsg .= fgets($fp, 1024);
					pclose($fp);	
				}*/
				
				
			}
			
		break;
		case 'BARTLBY::NOT::RUNNING':
			$omsg = "Bartlby doesnt seem to be running<br><input type=button value='try to startup bartlby' onClick='doStartup();'>";
		break;
		
		default:
			$omsg="ERROR is undefined";	
	}
	
	$omsg =$btl->FinScreen(str_replace("::", "_", $_GET[msg]));
	
	$layout->Tr(
	$layout->Td(
			Array(
				0=>$omsg
				
			)
		)

	);
	
	$layout->Tr(
	$layout->Td(
			Array(
				0=>"<input type=button value='retry' onClick='doRetry();'>"
				
			)
		)

	);
	
	
	$layout->TableEnd();
	$layout->display();
	
?>
