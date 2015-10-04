<?php
require("config.php");

if($_GET[Sign]==NULL) die('Error');

$con = mysql_connect($server,$user,$pwd);
if (!$con) die('Could not connect: ' . mysql_error());
mysql_select_db($DBname, $con);
$Exist=0;$result=0;
$Exist =mysql_query("SELECT * FROM GameUpdate where DiskSN='$_GET[Sign]';");
$result=mysql_num_rows($Exist);
echo "$result $_GET[Sign] $_GET[Status]";
if ($result){
	if ($_GET[Status])
		$sql="UPDATE GameUpdate SET Success = Success+1,DateTime = now() where DiskSN='$_GET[Sign]' ;";
	else
		$sql="UPDATE GameUpdate SET Fail = Fail + 1,DateTime = now() where DiskSN='$_GET[Sign]';" ;
}
else{
	if ($_GET[Status])
		$sql="INSERT INTO GameUpdate (DiskSN, Success, Fail, DateTime) VALUES('$_GET[Sign]',1,0,now())";
	else
		$sql="INSERT INTO GameUpdate (DiskSN, Success, Fail, DateTime) VALUES('$_GET[Sign]',0,1,now())";
}
if (!mysql_query($sql,$con)) die('Error: ' . mysql_error());

echo "OK";

mysql_close($con)
?>