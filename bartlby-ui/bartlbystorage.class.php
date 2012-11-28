<?php
	class bartlbyStorage {
		function bartlbyStorage($plugin_name) {
			$this->plugin_name=$plugin_name;
			$this->base_dir="store";
			$this->save_path=$this->base_dir . "/" . $this->plugin_name;
			if(!is_dir($this->save_path)) {
				if(!@mkdir($this->save_path)) {
					return false;
				}
			}
		}
		function save_key($key, $value) {
			$sk = md5($key);
			$fp = @fopen($this->save_path . "/" . $sk, "w");
			
			if(!$fp) {
				return false;
			}
			fwrite($fp, $value);
			fclose($fp);
			return true;
			
		}
		function load_key($key) {
			$sk = md5($key);
			if(!file_exists($this->save_path . "/" . $sk)) {
				return false;
			}
			return file_get_contents($this->save_path . "/" . $sk);
			
		}
	}

?>