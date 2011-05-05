
<?php
	$site = "www.kane-box.com";
	$port = 8080;

	$fp = fsockopen($site,$port,$errno,$errstr,10);
	if(!$fp)
	{
		$fd = fopen("welcome.html", r);
		$content = fread($fd, filesize("welcome.html"));
		fclose($fd);
		echo $content;
	}
	else{
		header("Location: http://www.kane-box.com:8080");
		//echo "Connect was successful - no errors on Port ".$port." at ".$site;
		fclose($fp);
		exit;
	}
	//
?>
